#include "allcore/analysis.h"

#include <algorithm>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace allcore {

std::vector<std::string> tokenizeAcip(const std::string& raw) {
    std::string s;
    s.reserve(raw.size());
    for (size_t i = 0; i < raw.size();) {
        // UTF-8 curly apostrophes ’ (E2 80 99) and ‘ (E2 80 98) → '
        if (i + 2 < raw.size() && (unsigned char)raw[i] == 0xE2 &&
            (unsigned char)raw[i + 1] == 0x80 &&
            ((unsigned char)raw[i + 2] == 0x98 || (unsigned char)raw[i + 2] == 0x99)) {
            s += '\'';
            i += 3;
            continue;
        }
        char c = raw[i++];
        if (c == '[') {                       // folio/page markers [598]
            while (i < raw.size() && raw[i - 1] != ']') ++i;
            s += ' ';
            continue;
        }
        if (c == ',' || c == ';' || c == '\n' || c == '\r' || c == '\t') c = ' ';
        s += c;
    }
    std::vector<std::string> toks;
    std::istringstream in(s);
    std::string t;
    while (in >> t) {
        while (!t.empty() && (t.back() == '.' || t.back() == ':')) t.pop_back();
        if (!t.empty()) toks.push_back(t);
    }
    return toks;
}

namespace {

std::string joinTokens(const std::vector<std::string>& toks, int beg, int end) {
    std::string out;
    for (int i = beg; i < end; ++i) {
        if (i > beg) out += ' ';
        out += toks[i];
    }
    return out;
}

}  // namespace

AnalysisPrePass runPrePass(const Spine& spine, const std::string& acip_passage,
                           int max_phrase_syllables, int corpus_limit) {
    AnalysisPrePass pre;
    pre.tokens = tokenizeAcip(acip_passage);
    const int n = static_cast<int>(pre.tokens.size());

    // greedy longest-match per position over the ACIP headword column;
    // shorter nested matches at the same position are recorded too.
    for (int i = 0; i < n; ++i) {
        int longest = 0;
        for (int len = std::min(max_phrase_syllables, n - i); len >= 1; --len) {
            const std::string cand = joinTokens(pre.tokens, i, i + len);
            auto hits = spine.lookup(cand);
            if (hits.empty()) continue;
            const Entry& e = hits.front();
            TermAnchor a;
            a.acip = e.acip.empty() ? cand : e.acip;
            a.wylie = e.wylie;
            a.tibetan = e.tibetan;
            a.pronunciation = e.pronunciation;
            a.hgm_gloss = e.hgm_gloss;
            a.tier = e.tier;
            a.beg_tok = i;
            a.end_tok = i + len;
            pre.anchors.push_back(std::move(a));
            if (longest == 0) longest = len;
            if (len == 1) break;     // got the single-syllable reading too
        }
        (void)longest;
    }

    // corpus concordance: probe with the longest anchors first (they are the
    // distinctive phrases), fall back to nothing rather than guessing.
    std::vector<const TermAnchor*> by_len;
    for (const auto& a : pre.anchors) by_len.push_back(&a);
    std::sort(by_len.begin(), by_len.end(), [](auto* x, auto* y) {
        return (x->end_tok - x->beg_tok) > (y->end_tok - y->beg_tok);
    });
    for (const auto* a : by_len) {
        if ((int)pre.corpus_hits.size() >= corpus_limit) break;
        if (a->end_tok - a->beg_tok < 2) break;   // single syllables are noise
        std::string q = '"' + a->wylie + '"';
        for (const auto& seg : spine.corpusSearch(q, "", 2)) {
            if ((int)pre.corpus_hits.size() >= corpus_limit) break;
            bool dup = false;
            for (const auto& h : pre.corpus_hits)
                dup |= (h.course == seg.course && h.seq == seg.seq);
            if (!dup)
                pre.corpus_hits.push_back({seg.course, seg.seq, seg.wylie, seg.english});
        }
    }
    return pre;
}

AssembledPrompt buildAnalysisPrompt(const std::string& template_path,
                                    const AnalysisPrePass& pre,
                                    const std::string& acip_passage,
                                    const std::string& draft_english) {
    AssembledPrompt out;
    {
        std::ifstream f(template_path);
        if (!f) throw std::runtime_error("cannot read template: " + template_path);
        std::stringstream ss;
        ss << f.rdbuf();
        const std::string all = ss.str();
        // canonical file = implementation preamble + "---" + verbatim template;
        // send only the template itself to the model.
        const std::string divider = "\n---\n";
        auto pos = all.find(divider);
        out.system = (pos == std::string::npos) ? all : all.substr(pos + divider.size());
    }

    std::string u;
    u += "The application has already run its deterministic engine over the passage "
         "against the HGM spine (v0.27.2). Treat the data below as VERIFIED INPUT:\n\n";

    u += "## ENGINE: HGM term anchors (authoritative — rule 1)\n";
    u += "These are the dictionary's actual entries for terms found in the passage. "
         "In item 8, the \"HGM default English\" column MUST come from these glosses "
         "verbatim; any different choice is a DEVIATION to mark explicitly. "
         "auto-aligned tier = PROVISIONAL, label it as such.\n\n";
    for (const auto& a : pre.anchors) {
        u += "- " + a.acip + " (" + a.wylie + " · " + a.tibetan;
        if (!a.pronunciation.empty()) u += " · pron " + a.pronunciation;
        u += ")";
        if (!a.tier.empty()) u += " [" + a.tier + "]";
        if (!a.hgm_gloss.empty()) {
            u += " = ";
            for (size_t i = 0; i < a.hgm_gloss.size(); ++i) {
                if (i) u += "; ";
                u += a.hgm_gloss[i];
            }
        } else {
            u += " = (no HGM equivalent — reference layers only; do not invent one)";
        }
        u += "\n";
    }
    if (pre.anchors.empty())
        u += "(no dictionary anchors found — flag this in item 3)\n";

    u += "\n## ENGINE: corpus concordance (HGM's own translations of overlapping text)\n";
    if (pre.corpus_hits.empty()) {
        u += "(no corpus overlap found)\n";
    } else {
        for (const auto& h : pre.corpus_hits) {
            u += "- [" + h.course + ":" + std::to_string(h.seq) + "] T: " + h.wylie +
                 "\n  E: " + h.english + "\n";
        }
    }

    u += "\n## NOTE on item 14 (cross-references)\n";
    u += "The reference library is NOT yet indexed in this app. For item 14, use ONLY "
         "the corpus hits above as citable material; otherwise state plainly that the "
         "library is not available and list what you would have searched.\n";

    u += "\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n<<INPUT START>>\n\n";
    u += acip_passage;
    if (!draft_english.empty()) u += "\n\n" + draft_english;
    u += "\n\n<<INPUT END>>\n";
    out.user = u;
    return out;
}

std::string jsonEscape(const std::string& s) {
    std::string o;
    o.reserve(s.size() + 16);
    for (unsigned char c : s) {
        switch (c) {
            case '"': o += "\\\""; break;
            case '\\': o += "\\\\"; break;
            case '\n': o += "\\n"; break;
            case '\r': o += "\\r"; break;
            case '\t': o += "\\t"; break;
            default:
                if (c < 0x20) {
                    char buf[8];
                    std::snprintf(buf, sizeof buf, "\\u%04x", c);
                    o += buf;
                } else {
                    o += static_cast<char>(c);
                }
        }
    }
    return o;
}

std::string buildMessagesRequestJson(const AssembledPrompt& prompt,
                                     const std::string& model, int max_tokens) {
    std::string j = "{";
    j += "\"model\":\"" + jsonEscape(model) + "\",";
    j += "\"max_tokens\":" + std::to_string(max_tokens) + ",";
    j += "\"stream\":true,";
    j += "\"fallbacks\":\"default\",";
    // template first with cache_control: stable prefix, cached across passages
    j += "\"system\":[{\"type\":\"text\",\"text\":\"" + jsonEscape(prompt.system) +
         "\",\"cache_control\":{\"type\":\"ephemeral\"}}],";
    j += "\"messages\":[{\"role\":\"user\",\"content\":\"" + jsonEscape(prompt.user) +
         "\"}]";
    j += "}";
    return j;
}

std::string sseTextDelta(const std::string& d) {
    if (d.find("\"type\":\"content_block_delta\"") == std::string::npos) return "";
    auto p = d.find("\"text\":\"");
    if (p == std::string::npos) return "";
    p += 8;
    std::string v;
    bool esc = false;
    for (size_t i = p; i < d.size(); ++i) {
        char c = d[i];
        if (esc) {
            switch (c) {
                case 'n': v += '\n'; break;
                case 't': v += '\t'; break;
                case 'r': break;
                case 'u':
                    if (i + 4 < d.size()) {
                        unsigned cp = std::stoul(d.substr(i + 1, 4), nullptr, 16);
                        // encode BMP codepoint as UTF-8 (surrogates handled as-is
                        // pairs by two successive escapes for the GUI's purposes)
                        if (cp < 0x80) v += (char)cp;
                        else if (cp < 0x800) {
                            v += (char)(0xC0 | (cp >> 6));
                            v += (char)(0x80 | (cp & 0x3F));
                        } else {
                            v += (char)(0xE0 | (cp >> 12));
                            v += (char)(0x80 | ((cp >> 6) & 0x3F));
                            v += (char)(0x80 | (cp & 0x3F));
                        }
                        i += 4;
                    }
                    break;
                default: v += c;
            }
            esc = false;
        } else if (c == '\\') {
            esc = true;
        } else if (c == '"') {
            break;
        } else {
            v += c;
        }
    }
    return v;
}

AssembledPrompt buildBackTranslationPrompt(const AnalysisPrePass& pre,
                                           const std::string& acip_source,
                                           const std::string& english_draft) {
    AssembledPrompt out;
    out.system =
        "You are a translation REVIEWER for Tibetan-to-English drafts. You "
        "receive a Tibetan source (ACIP transliteration), engine-verified HGM "
        "term anchors, and an English draft. Produce ONLY a coverage diff:\n"
        "1. OMISSIONS — source content with no counterpart in the draft "
        "(cite the ACIP phrase).\n"
        "2. ADDITIONS — draft content with no basis in the source (cite the "
        "draft phrase).\n"
        "3. ANCHOR MISMATCHES — places where the draft's rendering of an "
        "anchored term differs from every HGM gloss listed for it (state "
        "the term, the draft's word, and the HGM options; deviation may be "
        "deliberate — flag, do not condemn).\n"
        "4. VERDICT — one sentence on overall coverage.\n"
        "Never retranslate the passage, never rewrite or improve the draft, "
        "never invent English equivalents for terms without HGM glosses. "
        "Quote at most short phrases. If the draft covers everything, say so "
        "plainly.";

    std::string u;
    u += "## ENGINE: HGM term anchors (verified)\n";
    for (const auto& a : pre.anchors) {
        u += "- " + a.acip + " (" + a.wylie + ")";
        if (!a.tier.empty()) u += " [" + a.tier + "]";
        if (!a.hgm_gloss.empty()) {
            u += " = ";
            for (size_t i = 0; i < a.hgm_gloss.size() && i < 6; ++i) {
                if (i) u += "; ";
                u += a.hgm_gloss[i];
            }
        }
        u += "\n";
    }
    u += "\n## SOURCE (ACIP)\n" + acip_source + "\n";
    u += "\n## ENGLISH DRAFT UNDER REVIEW\n" + english_draft + "\n";
    out.user = std::move(u);
    return out;
}

}  // namespace allcore
