// Build a self-contained drill pack for the iOS app.
//
// Path A (Adam, 2026-09-09): the phone carries no spine and no converter. Every
// drill is generated HERE, on the Mac, by the same allcore that the desktop
// uses — so the phone inherits today's fixes for free: the whole segment shown
// against the English rather than a quarter of it, the title catalogue and the
// English-in-ACIP segments refused at the draw, and editorial markup separated
// from script instead of leaking raw.
//
// Tibetan is converted here too, through allcore::acipDisplayPieces, so the app
// only ever renders Unicode it was handed. Anything that would not convert
// arrives already flagged in ⟨ ⟩ and can never be mistaken for script.
#include "allcore/chantline.h"
#include "allcore/engines.h"
#include "allcore/drills.h"
#include "allcore/lattice.h"
#include "allcore/spine.h"
#include "allcore/terminology.h"
#include "allcore/tibdisplay.h"
#include "allcore/tibdisplay.h"

#include <algorithm>
#include <cstdio>
#include <fstream>
#include <map>
#include <sstream>
#include <random>
#include <string>
#include <vector>

namespace {

void esc(std::string& o, const std::string& s) {
    for (char c : s) {
        switch (c) {
            case '"': o += "\\\""; break;
            case '\\': o += "\\\\"; break;
            case '\n': o += "\\n"; break;
            case '\r': break;
            case '\t': o += " "; break;
            default:
                if ((unsigned char)c < 0x20) o += ' ';
                else o += c;
        }
    }
}
void field(std::string& o, const char* k, const std::string& v, bool comma = true) {
    o += "\""; o += k; o += "\":\"";
    esc(o, v);
    o += comma ? "\"," : "\"";
}
// Markup is stripped before conversion, on both the segment and the options.
// Adam, 2026-09-09: an option beginning "(" against a segment ending ")" gives
// the answer away before a word of Tibetan is read. It is a tell, and markup is
// not part of the answer.
std::string tib(const std::string& acip) {
    return allcore::acipToTibetanPlain(allcore::acipStripMarkup(acip));
}

// WYLIE, not ACIP. They are not the same alphabet and the difference is
// silent: pushing wylie through tib() turns every d into a retroflex
// (bdag -> བཌག instead of བདག, dang -> ཌཎག, du -> ཌུ) and REFUSES every
// s-final word outright, so gyis/rdzogs/sangs rgyas came back ⟨flagged⟩ and
// were dropped -- which looked like scarcity in the bank rather than a bug.
// The refusals were caught by the ⟨ ⟩ guard. The fabrications were not caught
// by anything, because they convert "successfully" into a real-looking word
// that is a different word.
//
// This mistake has now been made in four places in this file and fixed in
// four. It has a name here so the fifth is harder: the headword keys of
// alignment_evidence_v1.json are wylie, and they go through THIS.
std::string tibFromWylie(const std::string& wylie) {
    return allcore::wylieToUnicode(wylie).first;
}
// ---------------------------------------------------------------------------
// A small but CORRECT JSON reader, used only by the reading-order section.
// The other sections scan for a literal "key":" and stop at the next quote.
// That is safe for ACIP, which has no quotes and no escapes. It is NOT safe
// for his English: the prose carries apostrophes, quotation marks and \u
// escapes, and this is the one drill whose key is labelled ATTESTED — his own
// rendering of that very span. A key that is silently truncated at an
// apostrophe would be a false claim of attestation, which is worse than no
// drill at all.
namespace bj {

inline void ws(const std::string& j, size_t& i) {
    while (i < j.size() &&
           (j[i] == ' ' || j[i] == '\t' || j[i] == '\n' || j[i] == '\r'))
        ++i;
}
inline void utf8(std::string& o, unsigned cp) {
    if (cp < 0x80) { o += (char)cp; }
    else if (cp < 0x800) {
        o += (char)(0xC0 | (cp >> 6)); o += (char)(0x80 | (cp & 0x3F));
    } else if (cp < 0x10000) {
        o += (char)(0xE0 | (cp >> 12));
        o += (char)(0x80 | ((cp >> 6) & 0x3F));
        o += (char)(0x80 | (cp & 0x3F));
    } else {
        o += (char)(0xF0 | (cp >> 18));
        o += (char)(0x80 | ((cp >> 12) & 0x3F));
        o += (char)(0x80 | ((cp >> 6) & 0x3F));
        o += (char)(0x80 | (cp & 0x3F));
    }
}
inline bool str(const std::string& j, size_t& i, std::string& out) {
    ws(j, i);
    if (i >= j.size() || j[i] != '"') return false;
    ++i; out.clear();
    while (i < j.size()) {
        const char c = j[i++];
        if (c == '"') return true;
        if (c != '\\') { out += c; continue; }
        if (i >= j.size()) return false;
        const char e = j[i++];
        switch (e) {
            case 'n': out += '\n'; break;
            case 't': out += '\t'; break;
            case 'r': out += '\r'; break;
            case 'b': out += '\b'; break;
            case 'f': out += '\f'; break;
            case 'u': {
                if (i + 4 > j.size()) return false;
                unsigned cp = (unsigned)std::strtoul(
                    j.substr(i, 4).c_str(), nullptr, 16);
                i += 4;
                // surrogate pair, or the astral character comes out mangled
                if (cp >= 0xD800 && cp <= 0xDBFF && i + 6 <= j.size() &&
                    j[i] == '\\' && j[i + 1] == 'u') {
                    const unsigned lo = (unsigned)std::strtoul(
                        j.substr(i + 2, 4).c_str(), nullptr, 16);
                    if (lo >= 0xDC00 && lo <= 0xDFFF) {
                        cp = 0x10000 + ((cp - 0xD800) << 10) + (lo - 0xDC00);
                        i += 6;
                    }
                }
                utf8(out, cp);
                break;
            }
            default: out += e;   // \" \\ \/ and anything else, verbatim
        }
    }
    return false;
}
inline void skipValue(const std::string& j, size_t& i) {
    ws(j, i);
    if (i >= j.size()) return;
    if (j[i] == '"') { std::string t; str(j, i, t); return; }
    if (j[i] == '{' || j[i] == '[') {
        const char open = j[i], close = open == '{' ? '}' : ']';
        ++i;
        int depth = 1;
        while (i < j.size() && depth) {
            if (j[i] == '"') { std::string t; str(j, i, t); continue; }
            if (j[i] == open) ++depth;
            else if (j[i] == close) --depth;
            ++i;
        }
        return;
    }
    while (i < j.size() && j[i] != ',' && j[i] != '}' && j[i] != ']') ++i;
}

}  // namespace bj

// A title line still gets badged on the phone, for the rare one that is a
// legitimate heading rather than catalogue.
bool isTitle(const std::string& course) {
    std::string u = course;
    for (auto& c : u) c = (char)std::toupper((unsigned char)c);
    return u.rfind("TITL", 0) == 0 || u == "AUTH" || u == "SUBJ";
}

}  // namespace

int main(int argc, char** argv) {
    if (argc < 3) {
        std::fprintf(stderr,
                     "usage: build_drill_pack <spine.db> <out.json> "
                     "[count] [stamp]\n");
        return 2;
    }
    const int want = argc > 3 ? std::atoi(argv[3]) : 4000;
    // A stamp naming the commit and moment this pack was built. Without
    // one a STALE pack is indistinguishable from a fresh one, and the
    // phone will happily show yesterday's drills after a fix landed.
    const char* stamp = argc > 4 ? argv[4] : "unstamped";
    allcore::Spine spine(argv[1]);
    allcore::HeadwordIndex index(spine);
    allcore::DrillFactory f(spine, index);
    std::mt19937 rng(20260909);

    std::string out = "{\"meta\":{";
    field(out, "built_by", "allcore, on the Mac; the phone generates nothing");
    field(out, "source", spine.metaValue("source_corpus"));
    // This said "Every English line is Geshe Michael Roach's own text from
    // his courses." Measured against the pack it describes, on 2026-09-15:
    // 6,003 of 7,084 entries are his glossary or curated English, 709 are
    // AUTO-ALIGNED -- matched from the courses by machine, never composed,
    // and carried as provisional -- and 372 have no tier at all. 400 entries
    // carry aligned_eng, which is a machine-cut SPAN of one of his sentences;
    // some of those spans are not words he ever wrote, e.g. 'da' bar ->
    // "gressing" (erratum E-183), which is half of "transgressing".
    //
    // So the sentence was false for about a tenth of the pack, and it was
    // false in the pack's own provenance record -- a falsehood banked, which
    // is the one thing Rule 10 forbids outright. It also said "his courses"
    // where the project names him.
    field(out, "tier",
          "Most English here is Geshe Michael Roach's own, from Geshe "
          "Michael's courses. Some is machine-matched to that English and "
          "never composed; every such entry carries tier=auto-aligned and "
          "provisional=true, and must be shown as provisional. Entries "
          "carrying aligned_eng hold a machine-cut span of one of Geshe "
          "Michael's sentences, which may not be a whole word. Engine "
          "guidance is "
          "labeled guidance.");
    field(out, "built_from", stamp);
    out += "\"schema\":1},\"cloze\":[";

    int n = 0, titles = 0;
    // Dropped for want of a place to put the blank. The builder used to drop
    // these silently; a refusal that leaves no trace looks exactly like
    // "there were none".
    int unplaceable = 0;
    for (int i = 0; i < want * 8 && n < want; ++i) {
        auto c = f.makeCloze(rng);
        if (!c) continue;
        if (c->correct < 0 || c->correct >= (int)c->options.size()) continue;
        const std::string& seg = c->segment.acip;
        const std::string ans = c->options[c->correct];
        // the whole segment, with the answer blanked where it stands —
        // allcore::placeBlank, the same call the desktop makes
        const auto sp = allcore::placeBlank(seg, c->chunks, ans);
        if (!sp.ok) { ++unplaceable; continue; }   // refuse, and COUNT it

        // The segment converts as a whole — isDrillable already refuses one
        // that does not — but SPLITTING it at the blank can leave a fragment
        // that will not convert on its own, e.g. a bare "s". The two halves
        // are what the reader actually sees, so they are what must be
        // checked. Cheap, and it closes the last 0.1%.
        const std::string beforeT = tib(sp.before);
        const std::string afterT = tib(sp.after);
        if (beforeT.find("\u27e8") != std::string::npos ||
            afterT.find("\u27e8") != std::string::npos)
            continue;

        if (n) out += ",";
        out += "{";
        field(out, "before", beforeT);
        field(out, "after", afterT);
        field(out, "english", c->segment.english);
        field(out, "role", c->role);
        // The skill this drill exercises, in the SAME vocabulary the desktop
        // files misses under (allcore::clozeSkill). The phone cannot generate
        // a targeted drill — the pack is built here — but with every drill
        // tagged it can FILTER, which is the same feature reached the other
        // way round. One vocabulary, so a weak spot named on the desktop
        // means the same thing on the phone.
        field(out, "skill", allcore::clozeSkill(*c));
        field(out, "course", c->segment.course);
        out += "\"seq\":" + std::to_string(c->segment.seq) + ",";
        out += std::string("\"title\":") + (isTitle(c->segment.course) ? "true" : "false") + ",";
        if (isTitle(c->segment.course)) ++titles;
        out += "\"answer\":" + std::to_string(c->correct) + ",";
        out += "\"options\":[";
        for (size_t k = 0; k < c->options.size(); ++k) {
            if (k) out += ",";
            out += "\"";
            esc(out, tib(c->options[k]));
            out += "\"";
        }
        out += "],";
        // What the blanked word means — Adam, 2026-09-10: "I have answered
        // correctly a couple of times and STILL wondered what the English
        // equivalent was." Rule 1 forbids composing English, so this is
        // strictly a MATCH: allcore::checkTerminology takes the blanked chunk
        // and his own English for this segment, and reports which of his
        // recorded equivalents actually occur in it. Evidence that the match
        // is real rather than coincidence: against his English for THIS
        // segment one or more equivalents occur 50.3% of the time; against a
        // random other segment, 14.6%. A lift of 35.7 points.
        //
        // Never "the answer means X" — the median glossed headword has four
        // attested equivalents and the worst has 137, so naming one would be
        // picking a sense, which is a guess. The card shows his recorded range
        // and marks which of them he used here.
        {
            const allcore::TermReport tr = allcore::checkTerminology(
                spine, index, ans, c->segment.english);
            // checkTerminology orders unmatched first, because it was built
            // to flag gaps in a translator's draft. A learner's card wants the
            // opposite: the word he actually used here is the answer to the
            // question being asked, and a content word teaches more than a
            // nominaliser. So: matched first, then the longer headword.
            std::vector<allcore::TermUse> terms = tr.terms;
            std::stable_sort(terms.begin(), terms.end(),
                             [](const allcore::TermUse& a,
                                const allcore::TermUse& b) {
                                 if (a.matched.empty() != b.matched.empty())
                                     return !a.matched.empty();
                                 return a.wylie.size() > b.wylie.size();
                             });
            out += "\"means\":[";
            int emitted = 0;
            for (const auto& t : terms) {
                if (emitted >= 2) break;       // two headwords is a card, not a wall
                if (t.glosses.empty()) continue;
                if (emitted) out += ",";
                out += "{";
                field(out, "wylie", t.wylie);
                field(out, "tier", t.tier);
                out += std::string("\"provisional\":") +
                       (t.provisional ? "true" : "false") + ",";
                out += "\"glosses\":[";
                // twenty carried, eight shown until Look up is pressed — the
                // button must have something real to reveal
                for (size_t k = 0; k < t.glosses.size() && k < 20; ++k) {
                    if (k) out += ",";
                    out += "\""; esc(out, t.glosses[k]); out += "\"";
                }
                out += "],\"more\":" +
                       std::to_string(t.glosses.size() > 20
                                          ? t.glosses.size() - 20 : 0) + ",";
                out += "\"used\":[";
                for (size_t k = 0; k < t.matched.size() && k < 4; ++k) {
                    if (k) out += ",";
                    out += "\""; esc(out, t.matched[k]); out += "\"";
                }
                out += "]}";
                ++emitted;
            }
            out += "]";
        }
        out += "}";
        ++n;
    }
    out += "],";

    // ---- The other drill kinds (Adam, 2026-09-11: "the iOS version just has
    // the cloze option"). He was right, and it was a parity failure against
    // his own standing order. The phone generates nothing — the pack is built
    // here — so every kind it can offer has to be baked, and that is what the
    // rest of this file does now.
    //
    // Three desktop modes are NOT baked, each for a reason:
    //   Parallel reading    — it is corpus segments to read, which the trainer
    //                         array already carries.
    //   Vocabulary (SRS)    — the deck is the LEARNER's, built from what they
    //                         clicked on the desktop, and the two have no way
    //                         to sync. A second unconnected deck on the phone
    //                         would quietly compete with the real one.
    //   Translate & compare — needs checkTerminology run against the whole
    //                         105,634-entry spine, over a draft typed at the
    //                         moment. Not bakeable; genuinely desktop-only.
    {
        std::mt19937 r2(20260911);
        out += "\"order\":[";
        int no = 0;
        for (int i = 0; i < want * 4 && no < want / 4; ++i) {
            auto o = f.makeOrder(r2);
            if (!o) continue;
            bool bad = false;
            for (const auto& ch : o->chunks)
                if (tib(ch).find("\u27e8") != std::string::npos) bad = true;
            if (bad) continue;               // same refusal as the cloze
            if (no) out += ",";
            out += "{";
            field(out, "english", o->segment.english);
            field(out, "course", o->segment.course);
            out += "\"seq\":" + std::to_string(o->segment.seq) + ",";
            out += "\"chunks\":[";
            for (size_t k = 0; k < o->chunks.size(); ++k) {
                if (k) out += ",";
                out += "\""; esc(out, tib(o->chunks[k])); out += "\"";
            }
            out += "],\"presented\":[";
            for (size_t k = 0; k < o->presented.size(); ++k) {
                if (k) out += ",";
                out += std::to_string(o->presented[k]);
            }
            out += "],\"verb\":";
            out += std::string("{") ;
            {
                std::string vb;
                field(vb, "wylie", o->verb.confident ? o->verb.wylie : "");
                field(vb, "evidence", o->verb.evidence);
                vb += std::string("\"confident\":") +
                      (o->verb.confident ? "true" : "false");
                out += vb;
            }
            out += "}}";
            ++no;
        }
        out += "],\"particle\":[";
        int np = 0;
        for (int i = 0; i < want * 4 && np < want / 4; ++i) {
            auto pd = f.makeParticle(r2);
            if (!pd) continue;
            if (np) out += ",";
            out += "{";
            field(out, "english", pd->segment.english);
            field(out, "course", pd->segment.course);
            out += "\"seq\":" + std::to_string(pd->segment.seq) + ",";
            field(out, "explanation", pd->explanation);
            out += "\"correct\":" + std::to_string(pd->correct) + ",";
            out += "\"options\":[";
            for (size_t k = 0; k < pd->options.size(); ++k) {
                if (k) out += ",";
                out += "\""; esc(out, pd->options[k]); out += "\"";
            }
            out += "],\"tokens\":[";
            for (size_t k = 0; k < pd->tokens.size(); ++k) {
                if (k) out += ",";
                out += "\""; esc(out, tib(pd->tokens[k])); out += "\"";
            }
            out += "]}";
            ++np;
        }
        out += "],";

        // Script cards: his own Language Study Guide cards. Everything on one
        // is his — source-attested Tibetan, his pronunciation, his gloss — so
        // it is the beginner's rung and the phone should have it.
        out += "\"script\":[";
        int ns = 0;
        for (const auto& e : spine.scriptCards("", 1200)) {
            if (e.tibetan.empty() || e.pronunciation.empty()) continue;
            if (ns) out += ",";
            out += "{";
            field(out, "tibetan", e.tibetan);
            field(out, "wylie", e.wylie);
            field(out, "pron", e.pronunciation);
            field(out, "tier", e.tier);
            out += std::string("\"provisional\":") +
                   (e.provisional() ? "true" : "false") + ",";
            out += "\"gloss\":[";
            for (size_t k = 0; k < e.hgm_gloss.size() && k < 4; ++k) {
                if (k) out += ",";
                out += "\""; esc(out, e.hgm_gloss[k]); out += "\"";
            }
            out += "]}";
            ++ns;
        }
        out += "],";

        // The Debate Dojo: statements carrying the full template, split into
        // their three elements here so the phone does no parsing.
        out += "\"debate\":[";
        int nd = 0;
        for (const auto& sg : spine.corpusSearch("\"CHOS CAN\" AND \"THAL\"",
                                                 "", 400)) {
            if (nd >= 200) break;
            // the documented C13 window is refused, as on the desktop
            if (sg.course == "C13" && sg.seq >= 63 && sg.seq <= 82) continue;
            const auto db = allcore::parseDebate(sg.acip);
            if (!db.ok) continue;
            const std::string sub = tib(db.subject), con = tib(db.consequence),
                              rea = tib(db.reason);
            if (sub.find("\u27e8") != std::string::npos ||
                con.find("\u27e8") != std::string::npos ||
                rea.find("\u27e8") != std::string::npos) continue;
            if (nd) out += ",";
            out += "{";
            field(out, "preamble", tib(db.preamble));
            field(out, "subject", sub);
            field(out, "consequence", con);
            field(out, "reason", rea);
            field(out, "english", sg.english);
            field(out, "course", sg.course);
            out += "\"seq\":" + std::to_string(sg.seq);
            out += "}";
            ++nd;
        }
        out += "],";

        // The Silent Particle and His second thought both read the alignment
        // layer, which the packer does not otherwise open. Read it here so
        // the phone gets these two as well — they were left out of the first
        // parity pass, which is the same omission twice.
        out += "\"silent\":[";
        int nsp = 0;
        bool sawGisFamily = false;
        {
            std::ifstream af("data/alignment/alignment_full_v1.json");
            std::string j((std::istreambuf_iterator<char>(af)),
                          std::istreambuf_iterator<char>());
            size_t at = 0;
            while (nsp < 300 && (at = j.find("\"case\":true", at)) !=
                                    std::string::npos) {
                // walk back to this record's start and pull its fields
                const size_t rec = j.rfind('{', at);
                if (rec == std::string::npos) { at += 11; continue; }
                auto sfield = [&](const char* k) {
                    const std::string key = std::string("\"") + k + "\":\"";
                    const size_t p2 = j.find(key, rec);
                    if (p2 == std::string::npos || p2 > at + 400) return std::string();
                    const size_t b = p2 + key.size();
                    const size_t e = j.find('"', b);
                    return e == std::string::npos ? std::string()
                                                  : j.substr(b, e - b);
                };
                // `tib` is WYLIE. tib() converts ACIP, and the two are not
                // the same alphabet: pushing wylie through it turns every d
                // into a retroflex (dang -> ཌཎག, bdag -> བཌག, du -> ཌུ)
                // and REFUSES every s-final word outright (gyis, rdzogs,
                // sangs rgyas all came back ⟨flagged⟩). The refusals were
                // caught by the ⟨ ⟩ guard below and dropped, which is why the
                // whole gis-family was quietly missing from this drill and
                // looked like scarcity in the bank. The fabrications were not
                // caught by anything, because they convert "successfully".
                //
                // The peel section twenty lines down carries a comment warning
                // about this exact mistake. It was fixed there and not here.
                // Prefer the bank's ACIP field; fall back to the wylie one
                // through the WYLIE converter, never through this one.
                const std::string tibAcip = sfield("tib_acip");
                const std::string tibw = sfield("tib");
                const std::string course = sfield("course");
                // the record runs from rec to the next '}'; "eng":null
                // inside it means he rendered this span with no English word
                const size_t recEnd = j.find('}', at);
                const size_t nullAt = j.find("\"eng\":null", rec);
                const bool rendered =
                    !(nullAt != std::string::npos && recEnd != std::string::npos &&
                      nullAt < recEnd);
                const std::string tibGlyph =
                    !tibAcip.empty() ? tib(tibAcip)
                                     : allcore::wylieToUnicode(tibw).first;
                if (!tibw.empty() && !course.empty() &&
                    tibGlyph.find("\u27e8") == std::string::npos) {
                    if (nsp) out += ",";
                    out += "{";
                    field(out, "tib", tibGlyph);
                    field(out, "wylie", tibw);
                    field(out, "course", course);
                    out += std::string("\"rendered\":") +
                           (rendered ? "true" : "false");
                    out += "}";
                    ++nsp;
                    if (tibw == "gyis" || tibw == "kyis" || tibw == "gis")
                        sawGisFamily = true;
                }
                at += 11;
            }
        }
        // The gis-family is the commonest agentive marking in the language. If
        // none of gyis/kyis/gis survives into this drill, the wylie is being
        // pushed through the ACIP converter again -- which refuses every
        // s-final word, so the whole family vanishes and looks like scarcity
        // in the bank rather than a converter picked wrong. That is exactly
        // what had happened, and nothing noticed because the refusals were
        // dropped by the ⟨ ⟩ guard on their way out.
        if (nsp > 0 && !sawGisFamily) {
            std::fprintf(stderr,
                "REFUSED: the silent-particle drill contains no gyis/kyis/gis "
                "at all.\n  That family cannot really be absent; check which "
                "converter the wylie field is going through.\n");
            return 1;
        }
        out += "],\"second\":[";
        int nse = 0;
        {
            // alignment_evidence_v1.json: {"pairs": {wylie: [{eng, refs, n}]}}
            // Scanned rather than fully parsed — the packer has no JSON
            // library, and every field taken is a plain string or integer.
            std::ifstream ef("data/alignment/alignment_evidence_v1.json");
            std::string j((std::istreambuf_iterator<char>(ef)),
                          std::istreambuf_iterator<char>());
            const size_t pairsAt = j.find("\"pairs\"");
            size_t at = pairsAt == std::string::npos ? j.size() : pairsAt;
            while (nse < 300) {
                // a headword key: "\n  "wylie": ["
                const size_t k = j.find("\": [", at);
                if (k == std::string::npos) break;
                {   // headword keys are indented two spaces;
                    // "refs" sits deeper and must be skipped
                    const size_t ls = j.rfind('\n', k);
                    if (ls == std::string::npos ||
                        j.compare(ls, 4, "\n  \"") != 0) {
                        at = k + 4;
                        continue;
                    }
                }
                const size_t qs = j.rfind('"', k);
                if (qs == std::string::npos) break;
                const size_t qb = j.rfind('"', qs - 1);
                if (qb == std::string::npos) break;
                const std::string w = j.substr(qb + 1, qs - qb - 1);
                // Find the MATCHING bracket. The first ']' after the key
                // closes the nested "refs" array, not the headword's, and
                // stopping there made every word look like it had a single
                // rendering — so none ever qualified.
                size_t arrEnd = std::string::npos;
                {
                    int depth = 0;
                    // start AFTER the opening bracket: starting on it
                    // counted it as a nesting level, so the matching
                    // close never registered and nothing ever parsed
                    for (size_t z = k + 4; z < j.size(); ++z) {
                        if (j[z] == '[') ++depth;
                        else if (j[z] == ']') {
                            if (depth == 0) { arrEnd = z; break; }
                            --depth;
                        }
                    }
                }
                if (arrEnd == std::string::npos) break;
                const std::string arr = j.substr(k, arrEnd - k);
                // collect this headword's renderings
                std::vector<std::pair<std::string, int>> rs;
                size_t e = 0;
                // bj::str, not a raw search for the next quote. The bank holds
                // 194 renderings that CONTAIN a quotation mark -- '"They will
                // entrap me!"', 'of "true" existence', '"self,"' -- and JSON
                // escapes them. Reading to the next raw '"' stopped ON THE
                // ESCAPE, so what got banked was whatever preceded it: a bare
                // backslash, eight of them in the shipped pack. Those were
                // then printed under "Everything Geshe Michael has been
                // recorded writing for it" -- a false claim about his English
                // manufactured by a parser bug. A correct reader was already
                // in this file, five hundred lines up; this loop never called
                // it.
                while ((e = arr.find("\"eng\":", e)) != std::string::npos) {
                    size_t rd = e + 6;
                    std::string eng;
                    if (!bj::str(arr, rd, eng)) { e += 6; continue; }
                    const size_t q = rd;
                    int n = 1;
                    const size_t np2 = arr.find("\"n\": ", q);
                    if (np2 != std::string::npos && np2 < q + 300)
                        n = std::atoi(arr.c_str() + np2 + 5);
                    if (!eng.empty()) rs.emplace_back(std::move(eng), n);
                    e = q;
                }
                at = arrEnd + 1;
                if (rs.size() < 2 || w.empty() || w.size() > 40) continue;
                std::sort(rs.begin(), rs.end(),
                          [](const auto& a, const auto& b) {
                              return a.second > b.second;
                          });
                if (nse) out += ",";
                out += "{";
                field(out, "tib", tibFromWylie(w));   // w is WYLIE
                field(out, "wylie", w);
                out += "\"renderings\":[";
                for (size_t z = 0; z < rs.size() && z < 12; ++z) {
                    if (z) out += ",";
                    out += "{";
                    field(out, "eng", rs[z].first);
                    out += "\"n\":" + std::to_string(rs[z].second);
                    out += "}";
                }
                // The list is CAPPED at 12 and the phone printed "Everything
                // Geshe Michael has been recorded writing for it" over it.
                // 'gro ba has 23. A learner who named one of the missing 11
                // was told by a line claiming completeness that it was not his.
                // Carry the real count so the card can say which it is showing.
                out += "],\"total\":" + std::to_string(rs.size());
                out += "}";
                ++nse;
            }
        }
        out += "],";

        // Peel: a span and the pieces it contains. The nesting is derived
        // from the layer's own span ids and text containment — 39,945
        // parent-child pairs — so the phone consumes a table rather than
        // parsing anything, which is the plan's prerequisite.
        out += "\"peel\":[";
        int npl = 0;
        {
            std::ifstream pf("data/alignment/alignment_full_v1.json");
            std::string j((std::istreambuf_iterator<char>(pf)),
                          std::istreambuf_iterator<char>());
            struct Sp { std::string tib, course; int d = 0; int seg = 0; };
            std::vector<Sp> all;
            size_t at = 0;
            while ((at = j.find("{\"course\":\"", at)) != std::string::npos) {
                auto sf = [&](const char* k) {
                    const std::string key = std::string("\"") + k + "\":\"";
                    const size_t p2 = j.find(key, at);
                    const size_t stop = j.find('}', at);
                    if (p2 == std::string::npos || (stop != std::string::npos && p2 > stop))
                        return std::string();
                    const size_t b = p2 + key.size();
                    const size_t e = j.find('"', b);
                    return e == std::string::npos ? std::string() : j.substr(b, e - b);
                };
                Sp sp;
                sp.course = sf("course");
                // tib_acip, not tib: tib is WYLIE and tib() converts ACIP,
                // so the wylie field renders every d as a retroflex.
                sp.tib = sf("tib_acip");
                const size_t dp = j.find("\"d\":", at);
                const size_t sg = j.find("\"seg\":", at);
                const size_t stop = j.find('}', at);
                if (dp != std::string::npos && dp < stop) sp.d = std::atoi(j.c_str() + dp + 4);
                if (sg != std::string::npos && sg < stop) sp.seg = std::atoi(j.c_str() + sg + 6);
                if (!sp.tib.empty() && sp.d) all.push_back(sp);
                at = stop == std::string::npos ? j.size() : stop + 1;
            }
            // group by segment, then find a parent that splits
            std::map<std::pair<std::string,int>, std::vector<Sp>> bySeg;
            for (const auto& sp : all) bySeg[{sp.course, sp.seg}].push_back(sp);
            for (const auto& [key, v] : bySeg) {
                if (npl >= 250) break;
                for (const auto& par : v) {
                    std::vector<std::string> kids;
                    int kd = 99;
                    for (const auto& ch : v) {
                        if (ch.d <= par.d || ch.tib == par.tib) continue;
                        if (par.tib.find(ch.tib) == std::string::npos) continue;
                        if (ch.d < kd) { kd = ch.d; kids.clear(); }
                        if (ch.d == kd) kids.push_back(ch.tib);
                    }
                    if (kids.size() < 2) continue;
                    const std::string pg = tib(par.tib);
                    if (pg.find("\u27e8") != std::string::npos) continue;
                    bool ok = true;
                    for (const auto& k2 : kids)
                        if (tib(k2).find("\u27e8") != std::string::npos) ok = false;
                    if (!ok) continue;
                    if (npl) out += ",";
                    out += "{";
                    field(out, "parent", pg);
                    field(out, "course", key.first);
                    out += "\"seq\":" + std::to_string(key.second) + ",";
                    out += "\"pieces\":[";
                    for (size_t z = 0; z < kids.size(); ++z) {
                        if (z) out += ",";
                        out += "\""; esc(out, tib(kids[z])); out += "\"";
                    }
                    out += "]}";
                    ++npl;
                    break;
                }
            }
        }
        out += "],";
        std::printf("  peel %d\n", npl);

        // Boundary hunt: the segment with its punctuation stripped. Both
        // pools are packed and each item says which it came from, because
        // they are different skills and a phone that merged them would let
        // progress in the warm-up pool stand in for the real one.
        out += "\"boundary\":[";
        int nbd = 0;
        int nbd_pool[2] = {0, 0};
        {
            std::mt19937 brng(9182);
            for (int pool = 0; pool < 2; ++pool) {
                const int want = pool ? 120 : 80;
                // makeBoundary rejection-samples: it draws random segments and
                // gives up after 60 misses. The unpunctuated pool draws from
                // 2,412 qualifying segments out of 42,199 — about one in
                // eighteen — so a run of sixty misses comes up roughly 3% of
                // the times it is called, on a pool nowhere near exhausted.
                // Breaking on the first of those ended the pool on a coin
                // flip: the 11 September pack shipped 64 of these and this one
                // shipped 1, from engines measured at 22 and 21. Only a
                // sustained run of misses is evidence a pool is really dry.
                constexpr int kDryRuns = 12;   // ~1e-18 of a false exhaustion
                int dry = 0;
                for (int made = 0; made < want; ) {
                    auto d = f.makeBoundary(brng, pool == 1);
                    if (!d) { if (++dry >= kDryRuns) break; continue; }
                    dry = 0;
                    // A piece that will not render is refused outright rather
                    // than shipped with ⟨ ⟩ sitting in the middle of a word
                    // the learner is being asked to count across.
                    bool ok = true;
                    for (const auto& t : d->tokens)
                        if (tib(t).find("\u27e8") != std::string::npos) ok = false;
                    if (!ok) continue;
                    if (nbd) out += ",";
                    out += "{";
                    out += "\"tokens\":[";
                    for (size_t z = 0; z < d->tokens.size(); ++z) {
                        if (z) out += ",";
                        out += "\""; esc(out, tib(d->tokens[z])); out += "\"";
                    }
                    out += "],\"ends\":[";
                    for (size_t z = 0; z < d->ends.size(); ++z)
                        out += (z ? "," : "") + std::to_string(d->ends[z]);
                    out += "],\"attested\":[";
                    for (size_t z = 0; z < d->attested.size(); ++z)
                        out += std::string(z ? "," : "") +
                               (d->attested[z] ? "true" : "false");
                    out += "],\"unscored\":[";
                    for (size_t z = 0; z < d->unscored.size(); ++z)
                        out += (z ? "," : "") + std::to_string(d->unscored[z]);
                    out += "],\"functions\":[";
                    for (size_t z = 0; z < d->functions.size(); ++z) {
                        if (z) out += ",";
                        out += "\""; esc(out, d->functions[z]); out += "\"";
                    }
                    out += "],";
                    field(out, "course", d->segment.course);
                    out += "\"seq\":" + std::to_string(d->segment.seq) + ",";
                    out += std::string("\"hard\":") +
                           (d->punctuated ? "false" : "true") + ",";
                    field(out, "english", d->segment.english, false);
                    out += "}";
                    ++nbd; ++made; ++nbd_pool[pool];
                }
            }
        }
        out += "],";
        std::printf("  boundary %d  (warm-up %d/80, unpunctuated %d/120)\n",
                    nbd, nbd_pool[0], nbd_pool[1]);
        // A short pool is not a smaller pack, it is a different exercise: the
        // unpunctuated pool is the one that teaches finding a clause end with
        // no scribal help, and it has come up short in every pack ever built
        // without saying so. Refuse rather than ship a quiet degradation.
        if (nbd_pool[0] < 80 || nbd_pool[1] < 120) {
            std::fprintf(stderr,
                "REFUSED: boundary pools came up short — warm-up %d/80, "
                "unpunctuated %d/120.\n"
                "  The generator rejection-samples; a short pool means either "
                "the corpus really is exhausted\n"
                "  or the give-up rule regressed. Measure the population "
                "before raising a target.\n",
                nbd_pool[0], nbd_pool[1]);
            return 1;
        }

        // Reading order — which chunk does his English take first? The
        // fourteenth drill, and the one that answers the difficulty Adam
        // actually named: not what the words mean, but what order to take
        // them in. It landed on the desktop on 13 September against a pack
        // that had no such kind at all, so the phone was a drill behind from
        // the moment the desktop gained it.
        //
        // The key here is ATTESTED and must stay that way: it is where each
        // child's English actually sits inside the parent's English in the
        // bank — his rendering of that very span — and never the engine's
        // ruling about what order the chunks ought to be read in.
        out += "\"readorder\":[";
        int nro = 0, nro_reorder = 0, nro_identity = 0;
        {
            std::ifstream rf("data/alignment/alignment_full_v1.json");
            std::string j((std::istreambuf_iterator<char>(rf)),
                          std::istreambuf_iterator<char>());
            struct BL {
                std::string course, eng, tib, acip;
                int d = 0, seg = 0;
            };
            std::map<std::string, std::vector<BL>> byRef;
            size_t i = j.find("\"links\"");
            if (i != std::string::npos) i = j.find('[', i);
            if (i != std::string::npos) {
                ++i;
                while (i < j.size()) {
                    bj::ws(j, i);
                    if (i >= j.size() || j[i] == ']') break;
                    if (j[i] == ',') { ++i; continue; }
                    if (j[i] != '{') break;
                    ++i;
                    BL b;
                    while (i < j.size()) {
                        bj::ws(j, i);
                        if (j[i] == '}') { ++i; break; }
                        if (j[i] == ',') { ++i; continue; }
                        std::string key;
                        if (!bj::str(j, i, key)) { i = j.size(); break; }
                        bj::ws(j, i);
                        if (i < j.size() && j[i] == ':') ++i;
                        bj::ws(j, i);
                        const bool isStr = i < j.size() && j[i] == '"';
                        if (key == "course" && isStr) bj::str(j, i, b.course);
                        else if (key == "eng" && isStr) bj::str(j, i, b.eng);
                        // tib is WYLIE and is what the bank nests by; tib_acip
                        // is what renders. Both are needed and they are not
                        // interchangeable — see the peel section's note.
                        else if (key == "tib" && isStr) bj::str(j, i, b.tib);
                        else if (key == "tib_acip" && isStr) bj::str(j, i, b.acip);
                        else if (key == "d" || key == "seg") {
                            const size_t st = i;
                            bj::skipValue(j, i);
                            const int val =
                                std::atoi(j.substr(st, i - st).c_str());
                            (key == "d" ? b.d : b.seg) = val;
                        } else {
                            bj::skipValue(j, i);
                        }
                    }
                    if (!b.course.empty())
                        byRef[b.course + ":" + std::to_string(b.seg)]
                            .push_back(b);
                }
            }
            std::mt19937 rrng(4413);
            // Draw across the whole bank, not the front of it. std::map walks
            // its keys in order, so taking the first 300 took C01 230 times,
            // C02 46 and C03 24 — the phone's newest drill would have been
            // course one wearing the whole bank's name, and course one is the
            // simplest material, which also skewed how often the answer
            // reorders at all. Shuffled with its own fixed seed so the pack
            // stays reproducible.
            std::vector<const std::pair<const std::string,
                                        std::vector<BL>>*> refs;
            refs.reserve(byRef.size());
            for (const auto& kv : byRef) refs.push_back(&kv);
            {
                std::mt19937 srng(7717);
                std::shuffle(refs.begin(), refs.end(), srng);
            }
            for (const auto* kvp : refs) {
                if (nro >= 300) break;
                const auto& kv = *kvp;
                const auto& v = kv.second;
                if (v.size() < 3) continue;
                for (const BL& p : v) {
                    if (p.tib.empty() || p.eng.empty() || p.acip.empty())
                        continue;
                    // The bank does not pass through isDrillable, so the
                    // chanted prayers reach this drill by their own road: 23
                    // cards presented "dakki jinsok gyipay sunam kyi," as his
                    // English, under a badge reading ATTESTED. It is the sound
                    // of the Tibetan, not a rendering of it.
                    if (allcore::englishIsNotEnglish(p.eng, p.acip)) continue;
                    // the SHALLOWEST layer strictly under this span
                    std::vector<const BL*> kids;
                    int kidDepth = 99;
                    for (const BL& c : v) {
                        if (c.d <= p.d || c.tib == p.tib) continue;
                        if (c.eng.empty() || c.tib.empty()) continue;
                        if (p.tib.find(c.tib) == std::string::npos) continue;
                        if (c.d < kidDepth) { kidDepth = c.d; kids.clear(); }
                        if (c.d == kidDepth) kids.push_back(&c);
                    }
                    if (kids.size() < 2 || kids.size() > 5) continue;
                    // shown in the order they are WRITTEN
                    std::sort(kids.begin(), kids.end(),
                              [&](const BL* a, const BL* b2) {
                                  return p.tib.find(a->tib) <
                                         p.tib.find(b2->tib);
                              });
                    // and where his English puts each of them. `find` is an
                    // unanchored substring probe, so when one piece's English
                    // occurs inside another's it is located in the WRONG place
                    // and the order derived from it is not his: "The definition
                    // of an apparent refuge is" put `refuge` at 30, which is
                    // inside `apparent refuge` at 21. Three cards in 300
                    // shipped such a key under a badge reading ATTESTED.
                    //
                    // Distinct positions were not enough — the two positions
                    // there differ. The pieces must occupy DISJOINT stretches
                    // of his sentence, and a card that cannot show that is
                    // refused rather than guessed at (rule 3).
                    std::vector<std::pair<size_t, size_t>> at;
                    bool ok = true;
                    for (const BL* k : kids) {
                        const size_t b = p.eng.find(k->eng);
                        if (b == std::string::npos) { ok = false; break; }
                        at.emplace_back(b, b + k->eng.size());
                    }
                    if (ok)
                        for (size_t a = 0; a < at.size() && ok; ++a)
                            for (size_t b2 = a + 1; b2 < at.size(); ++b2)
                                if (at[a].first < at[b2].second &&
                                    at[b2].first < at[a].second) { ok = false; break; }
                    if (!ok) continue;
                    std::vector<size_t> pos;
                    pos.reserve(at.size());
                    for (const auto& r : at) pos.push_back(r.first);
                    // Every piece must have Tibetan to tap. The bank holds
                    // one-sided records — his English for a span with no
                    // Tibetan exponent of its own — and 44 of the first 300
                    // cards shipped with a numbered row and nothing beside it.
                    // A chunk with no Tibetan cannot be put in reading order.
                    for (const BL* k : kids) {
                        const std::string t = tib(k->acip);
                        if (t.empty() || t.find("⟨") != std::string::npos)
                            ok = false;
                    }
                    const std::string pt = tib(p.acip);
                    if (!ok || pt.empty() || pt.find("⟨") != std::string::npos)
                        continue;
                    std::vector<int> rank(pos.size());
                    for (size_t z = 0; z < pos.size(); ++z) rank[z] = (int)z;
                    std::sort(rank.begin(), rank.end(),
                              [&](int a, int b2) { return pos[a] < pos[b2]; });
                    bool identity = true;
                    for (size_t z = 0; z < rank.size(); ++z)
                        if (rank[z] != (int)z) identity = false;
                    // A card whose answer is 1 2 3 teaches typing 1 2 3. Some
                    // are kept so the learner cannot assume every card
                    // reorders, but the pool is the spans that actually do.
                    // THE GUARD ABOVE TESTS THE PARENT ONLY, and the parent is
                    // not what the learner is shown as the key. C02:21's parent
                    // reads "kyabne konchok sumla chupa bul. A Buddhist Grace I
                    // offer this To the Teacher..." -- transliteration followed
                    // by its translation -- so it carries English function
                    // words and clears the guard comfortably. Its five CHILDREN
                    // are "kyabne", "konchok", "sumla", "chupa", "bul": pure
                    // sound, and they are what appears under each Tibetan chunk
                    // on Check, beneath a badge reading "ATTESTED - the key is
                    // Geshe Michael's own English for this very span".
                    //
                    // Testing each child alone is a no-op: both arms of
                    // englishIsNotEnglish need four words and these are one
                    // each. So the children are judged AS A SET, which is how
                    // the learner reads them -- five words, not one English
                    // function word among them.
                    {
                        std::string kidLine, kidAcip;
                        for (const BL* k : kids) {
                            if (!kidLine.empty()) { kidLine += " "; kidAcip += " "; }
                            kidLine += k->eng;
                            kidAcip += k->acip;
                        }
                        if (allcore::englishIsNotEnglish(kidLine, kidAcip))
                            continue;
                    }
                    if (identity && (rrng() % 4)) continue;
                    if (nro) out += ",";
                    out += "{";
                    field(out, "ref", kv.first);
                    field(out, "parent", tib(p.acip));
                    field(out, "parentEng", p.eng);
                    out += "\"kids\":[";
                    for (size_t z = 0; z < kids.size(); ++z) {
                        if (z) out += ",";
                        out += "\""; esc(out, tib(kids[z]->acip)); out += "\"";
                    }
                    out += "],\"kidsEng\":[";
                    for (size_t z = 0; z < kids.size(); ++z) {
                        if (z) out += ",";
                        out += "\""; esc(out, kids[z]->eng); out += "\"";
                    }
                    out += "],\"answer\":[";
                    for (size_t z = 0; z < rank.size(); ++z) {
                        if (z) out += ",";
                        out += std::to_string(rank[z]);
                    }
                    out += "]}";
                    ++nro;
                    if (identity) ++nro_identity; else ++nro_reorder;
                    break;
                }
            }
        }
        out += "],";
        std::printf("  readorder %d  (his English reorders in %d, runs "
                    "straight through in %d)\n",
                    nro, nro_reorder, nro_identity);
        if (nro < 200) {
            std::fprintf(stderr,
                "REFUSED: reading-order pool short — %d of a wanted 300.\n"
                "  The pool is the alignment bank, not the corpus: it grows "
                "as courses are aligned.\n"
                "  If the bank shrank, find out why before lowering this.\n", nro);
            return 1;
        }

        // Known here / known anywhere. A word is packed only when it has TWO
        // distinct attestations, because the whole exercise is the difference
        // between the two: "you know it here" is the segment you met it in,
        // "you know it anywhere" is a different one. A word with a single
        // attestation cannot answer the second question and is left out
        // rather than shipped with the second stage quietly unreachable.
        out += "\"vocab\":[";
        int nvc = 0;
        {
            std::ifstream ef("data/alignment/alignment_evidence_v1.json");
            std::string j((std::istreambuf_iterator<char>(ef)),
                          std::istreambuf_iterator<char>());
            const size_t pairsAt = j.find("\"pairs\"");
            size_t at = pairsAt == std::string::npos ? j.size() : pairsAt;
            while (nvc < 400) {
                const size_t k = j.find("\": [", at);
                if (k == std::string::npos) break;
                {
                    const size_t ls = j.rfind('\n', k);
                    if (ls == std::string::npos ||
                        j.compare(ls, 4, "\n  \"") != 0) { at = k + 4; continue; }
                }
                const size_t qs = j.rfind('"', k);
                if (qs == std::string::npos) break;
                const size_t qb = j.rfind('"', qs - 1);
                if (qb == std::string::npos) break;
                const std::string w = j.substr(qb + 1, qs - qb - 1);
                size_t arrEnd = std::string::npos;
                {
                    int depth = 0;
                    for (size_t z = k + 4; z < j.size(); ++z) {
                        if (j[z] == '[') ++depth;
                        else if (j[z] == ']') {
                            if (depth == 0) { arrEnd = z; break; }
                            --depth;
                        }
                    }
                }
                if (arrEnd == std::string::npos) break;
                const std::string arr = j.substr(k, arrEnd - k);
                at = arrEnd + 1;
                if (w.empty() || w.size() > 40) continue;
                // Multi-syllable only. The single-syllable entries here are
                // overwhelmingly grammatical particles ('am, ni, kyi), which
                // the Particle mode already drills properly and which nobody
                // learns as vocabulary — "do you know 'am?" is not a question
                // about knowing a word. Content vocabulary in Tibetan is
                // overwhelmingly multi-syllable, so requiring a space is a
                // cheap filter that costs almost no real words.
                if (w.find(' ') == std::string::npos) continue;
                // his most-attested English for it
                std::string gloss; int best = -1;
                size_t e = 0;
                // Same fault as the 'second' scanner: reading to the next raw
                // quote truncates any rendering that contains one, and the
                // vocab card's gloss is the ANSWER. bj::str handles escapes.
                while ((e = arr.find("\"eng\":", e)) != std::string::npos) {
                    size_t rd = e + 6;
                    std::string one;
                    if (!bj::str(arr, rd, one)) { e += 6; continue; }
                    const size_t q = rd;
                    int n = 1;
                    const size_t np2 = arr.find("\"n\": ", q);
                    if (np2 != std::string::npos && np2 < q + 300)
                        n = std::atoi(arr.c_str() + np2 + 5);
                    if (n > best && !one.empty()) { best = n; gloss = one; }
                    e = q;
                }
                if (gloss.empty()) continue;
                const std::string wd = tibFromWylie(w);   // w is WYLIE
                if (wd.find("\u27e8") != std::string::npos) continue;
                auto hits = spine.corpusSearch("\"" + w + "\"", "", 8);
                // two DISTINCT segments, each carrying his English
                const allcore::CorpusSegment* a = nullptr;
                const allcore::CorpusSegment* b2 = nullptr;
                // The card says "The segment you met it in." It has to be a
                // segment the word is actually IN. corpusSearch is FTS, and
                // FTS matches through a normalised column where 'chi and shi
                // fall together -- so the shipped card for 'chi rgyu ("cause
                // of death") cited C13:214 and C18:438, whose Tibetan reads
                // SHI RGYU YIN TE. The word was not there, and the segment
                // quoted underneath was the one the learner was told they had
                // met it in. A search hit is a candidate, not an attestation.
                auto containsHeadword = [&](const allcore::CorpusSegment& h) {
                    if (h.wylie.find(w) != std::string::npos) return true;
                    // the segment's ACIP, read back as wylie, must hold it
                    const std::string he = allcore::acipToEwts(h.acip);
                    return he.find(w) != std::string::npos;
                };
                for (const auto& h : hits) {
                    if (h.english.empty()) continue;
                    if (!allcore::DrillFactory::isDrillable(h)) continue;
                    if (!containsHeadword(h)) continue;
                    const std::string t = tib(h.acip);
                    if (t.empty() || t.find("\u27e8") != std::string::npos) continue;
                    if (!a) a = &h;
                    else if (h.id != a->id) { b2 = &h; break; }
                }
                if (!a || !b2) continue;
                if (nvc) out += ",";
                out += "{";
                field(out, "tib", wd);
                field(out, "wylie", w);
                // NOT "eng". This is the alignment layer's most-attested
                // English for the word — machine-matched, TENTATIVE — and a
                // field called "eng" next to a segment's attested english
                // invites a UI author to render the two the same way. The
                // name says what it is.
                field(out, "aligned_eng", gloss);
                auto seg = [&](const char* nm, const allcore::CorpusSegment* g) {
                    out += "\""; out += nm; out += "\":{";
                    field(out, "tib", tib(g->acip));
                    field(out, "course", g->course);
                    out += "\"seq\":" + std::to_string(g->seq) + ",";
                    field(out, "english", g->english, false);
                    out += "},";
                };
                seg("here", a);
                seg("anywhere", b2);
                out.pop_back();
                out += "}";
                ++nvc;
            }
        }
        out += "],";
        std::printf("  vocab %d\n", nvc);
        std::printf("  order %d · particle %d · script %d · debate %d · "
                    "silent %d · second %d\n", no, np, ns, nd, nsp, nse);
    }

    out += "\"trainer\":[";

    // The Trainer: a passage to read yourself, then six layers revealed one at
    // a time — chunks, particle roles, reading order, vocabulary, the answer
    // key, and the full parse. makeOrder already computes all of it, so the
    // phone reveals what the Mac worked out. Adam, 2026-09-09: the iOS app is
    // "a Tibetan Translation Trainer that has both the drills functionality
    // and the Trainer tool".
    int t = 0;
    for (int i = 0; i < want * 8 && t < want / 4; ++i) {
        auto o = f.makeOrder(rng);
        if (!o) continue;
        if (o->chunks.size() < 2) continue;
        if (t) out += ",";
        out += "{";
        field(out, "tibetan", tib(o->segment.acip));
        field(out, "english", o->segment.english);
        field(out, "course", o->segment.course);
        out += "\"seq\":" + std::to_string(o->segment.seq) + ",";
        // layer 1 — the chunks, in their true order
        out += "\"chunks\":[";
        for (size_t k = 0; k < o->chunks.size(); ++k) {
            if (k) out += ",";
            out += "\""; esc(out, tib(o->chunks[k])); out += "\"";
        }
        out += "],";
        // layer 2 — what each chunk is doing
        out += "\"roles\":[";
        for (size_t k = 0; k < o->chunks.size(); ++k) {
            if (k) out += ",";
            out += "\"";
            esc(out, k < o->markers.size() ? o->markers[k] : std::string());
            out += "\"";
        }
        out += "],";
        // layer 3 — the reading order, with the guidance for each step
        out += "\"plan\":[";
        for (size_t k = 0; k < o->plan.size(); ++k) {
            if (k) out += ",";
            const auto& st = o->plan[k];
            out += "{\"chunk\":" + std::to_string(st.chunk) +
                   ",\"order\":" + std::to_string(st.order) + ",";
            field(out, "how", st.how ? st.how : "", false);
            out += "}";
        }
        out += "],";
        // the verb, which frames the clause — layer 6's anchor
        out += "{";
        out.pop_back();
        // How much of the passage on screen the layers below actually
        // describe. The passage is the WHOLE segment; the chunks, roles and
        // plan come from ONE clause of it (drills.cpp picks 3-7 chunks), so
        // the median is about a third. The surface must say so rather than
        // let the layers read as an account of everything above them.
        {
            auto sylCount = [](const std::string& t) {
                int n = 0; bool in = false;
                for (size_t i = 0; i < t.size(); ++i) {
                    const unsigned char ch = (unsigned char)t[i];
                    const bool sep = (ch == ' ') ||
                        (i + 2 < t.size() && ch == 0xE0 &&
                         (unsigned char)t[i+1] == 0xBC &&
                         (unsigned char)t[i+2] == 0x8B);   // U+0F0B tsheg
                    if (sep) { in = false; if (ch != ' ') i += 2; }
                    else if (!in) { in = true; ++n; }
                }
                return n;
            };
            int whole = sylCount(tib(o->segment.acip)), part = 0;
            for (const auto& c : o->chunks) part += sylCount(tib(c));
            const int share = whole ? (part * 100 / whole) : 100;
            out += "\"clause_share\":" + std::to_string(share) + ",";
        }
        field(out, "verb", o->verb.wylie);
        field(out, "verb_evidence", o->verb.evidence);
        out += std::string("\"verb_confident\":") +
               (o->verb.confident ? "true" : "false");
        out += "}";
        ++t;
    }
    out += "]}";

    FILE* fp = std::fopen(argv[2], "wb");
    if (!fp) { std::fprintf(stderr, "cannot write %s\n", argv[2]); return 1; }
    std::fwrite(out.data(), 1, out.size(), fp);
    std::fclose(fp);
    std::printf("wrote %s: %d cloze drills, %.2f MB (%zu bytes each)\n",
                argv[2], n, out.size() / 1048576.0,
                n ? out.size() / (size_t)n : 0);
    std::printf("  trainer passages: %d\n", t);
    std::printf("  cloze drills dropped for want of a blank position: %d "
                "(the answer did not appear verbatim in its segment)\n",
                unplaceable);
    std::printf("  title-catalogue drills in the pack: %d (the draw refuses "
                "them; this should read 0)\n", titles);
    return n > 0 ? 0 : 1;
}
