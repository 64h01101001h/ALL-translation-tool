// catalog_id.cpp — see allcore/catalog_id.h for the charter and the measured
// numbers. Built against the installed library, not against a guess about
// what ACIP title pages look like.
#include "allcore/catalog_id.h"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <set>
#include <sstream>
#include <unordered_map>

namespace allcore {
namespace {

std::string upperAscii(const std::string& s) {
    std::string o = s;
    for (char& c : o)
        c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    return o;
}

// Splits a normalized title into its syllables.
std::vector<std::string> syllables(const std::string& norm) {
    std::vector<std::string> out;
    std::istringstream is(norm);
    std::string w;
    while (is >> w) out.push_back(w);
    return out;
}

// The overlap score: shared distinct syllables over the LONGER of the two
// syllable sets. Dividing by the longer side is what keeps a three-syllable
// title from claiming a thirty-syllable work by accident.
double overlap(const std::vector<std::string>& a,
               const std::vector<std::string>& b, int* shared_out) {
    const std::set<std::string> A(a.begin(), a.end());
    const std::set<std::string> B(b.begin(), b.end());
    if (A.empty() || B.empty()) {
        if (shared_out) *shared_out = 0;
        return 0.0;
    }
    int shared = 0;
    for (const auto& x : A)
        if (B.count(x)) ++shared;
    if (shared_out) *shared_out = shared;
    return static_cast<double>(shared) /
           static_cast<double>(std::max(A.size(), B.size()));
}

// The head of the file is where a title page lives; beyond this the text is
// the work itself and any "BZHUGS SO" belongs to a chapter, not the title.
constexpr size_t kHeadBytes = 4000;

}  // namespace

std::string normalizeAcipTitle(const std::string& text) {
    const std::string up = upperAscii(text);
    std::string stripped;
    stripped.reserve(up.size());
    int bracket = 0, paren = 0;
    for (size_t i = 0; i < up.size(); ++i) {
        const char c = up[i];
        // @012A / @ 12B page markers
        if (c == '@') {
            size_t j = i + 1;
            while (j < up.size() && up[j] == ' ') ++j;
            size_t d = j;
            while (d < up.size() && std::isdigit(static_cast<unsigned char>(up[d])))
                ++d;
            if (d > j && d < up.size() && (up[d] == 'A' || up[d] == 'B')) {
                i = d;
                stripped.push_back(' ');
                continue;
            }
        }
        if (c == '[') { ++bracket; continue; }
        if (c == ']') { if (bracket) --bracket; continue; }
        if (c == '(') { ++paren; continue; }
        if (c == ')') { if (paren) --paren; continue; }
        if (bracket || paren) continue;
        if ((c >= 'A' && c <= 'Z') || c == '\'' || c == '-')
            stripped.push_back(c);
        else
            stripped.push_back(' ');
    }
    std::string out;
    bool space = true;  // leading space swallowed
    for (char c : stripped) {
        if (c == ' ') {
            if (!space) { out.push_back(' '); space = true; }
        } else {
            out.push_back(c);
            space = false;
        }
    }
    while (!out.empty() && out.back() == ' ') out.pop_back();
    return out;
}

TitleExtraction extractAcipTitle(const std::string& text) {
    TitleExtraction t;
    const std::string head = text.substr(0, std::min(text.size(), kHeadBytes));
    const std::string up = upperAscii(head);

    // Rule 1 (canonical bilingual head). The title is what follows "BOD SKAD
    // DU," up to the next comma — the homage clause begins after it.
    const std::string kBod = "BOD SKAD DU";
    const size_t b = up.find(kBod);
    if (b != std::string::npos) {
        size_t s = b + kBod.size();
        while (s < head.size() && (head[s] == ',' || head[s] == ' ' ||
                                   head[s] == '\n' || head[s] == '\r'))
            ++s;
        size_t e = head.find(',', s);
        if (e == std::string::npos) e = head.size();
        t.raw = head.substr(s, e - s);
        t.title = normalizeAcipTitle(t.raw);
        t.rule = "bod-skad-du";
        t.found = syllables(t.title).size() >= 3;
        // the Sanskrit (or Chinese) side of the same head, when present
        for (const char* cue : {"RGYA GAR SKAD DU", "RGYA'I SKAD DU"}) {
            const size_t g = up.find(cue);
            if (g != std::string::npos && g < b) {
                size_t gs = g + std::string(cue).size();
                while (gs < head.size() && (head[gs] == ',' || head[gs] == ' ' ||
                                            head[gs] == '\n' || head[gs] == '\r'))
                    ++gs;
                if (gs < b) t.sanskrit = head.substr(gs, b - gs);
                // trim the trailing "," that introduced BOD SKAD DU
                while (!t.sanskrit.empty() &&
                       (t.sanskrit.back() == ',' || t.sanskrit.back() == ' ' ||
                        t.sanskrit.back() == '\n' || t.sanskrit.back() == '\r'))
                    t.sanskrit.pop_back();
                break;
            }
        }
    }

    // Rule 2 (title block closing in BZHUGS SO) — the Sungbum shape.
    if (!t.found) {
        const size_t z = up.find("BZHUGS SO");
        if (z != std::string::npos) {
            std::string block = head.substr(0, z);
            // drop everything through the opening folio marker and its
            // punctuation preamble ("@001A *, ,")
            size_t at = block.rfind('@');
            if (at != std::string::npos) {
                size_t j = at + 1;
                while (j < block.size() &&
                       std::isdigit(static_cast<unsigned char>(block[j])))
                    ++j;
                if (j < block.size() && (block[j] == 'A' || block[j] == 'B')) {
                    ++j;
                    while (j < block.size() &&
                           (block[j] == ' ' || block[j] == ',' ||
                            block[j] == '*' || block[j] == '#' ||
                            block[j] == '`' || block[j] == '\n' ||
                            block[j] == '\r'))
                        ++j;
                    block = block.substr(j);
                }
            }
            t.raw = block;
            t.title = normalizeAcipTitle(block);
            t.rule = "bzhugs-so";
            t.found = syllables(t.title).size() >= 3;
        }
    }

    if (!t.found) {
        t.title.clear();
        t.raw.clear();
        t.rule.clear();
        return t;
    }

    // Alternative reading: Sungbum title pages routinely name the collection
    // first ("… BKA' 'BUM LAS <the work>"). The tail after LAS is a title in
    // its own right, so it is offered as a second reading of the same page.
    const auto sy = syllables(t.title);
    for (size_t i = 0; i + 3 < sy.size(); ++i) {
        if (sy[i] != "LAS") continue;
        std::string tail;
        for (size_t j = i + 1; j < sy.size(); ++j) {
            if (!tail.empty()) tail += ' ';
            tail += sy[j];
        }
        t.variants.push_back(tail);
        break;
    }
    return t;
}

bool TitleBank::add(const std::string& key, const std::string& raw_title,
                    const std::string& source, const std::string& eng) {
    TitleBankEntry e;
    e.key = key;
    e.raw = raw_title;
    e.title = normalizeAcipTitle(raw_title);
    e.eng = eng;
    e.source = source;
    if (key.empty() || syllables(e.title).size() < 3) return false;
    entries_.push_back(std::move(e));
    return true;
}

int TitleBank::addLibraryTree(const std::string& root) {
    namespace fs = std::filesystem;
    std::error_code ec;
    if (!fs::exists(root, ec)) return 0;
    int banked = 0;
    for (fs::recursive_directory_iterator it(
             root, fs::directory_options::skip_permission_denied, ec), end;
         it != end; it.increment(ec)) {
        if (ec) break;
        if (!it->is_regular_file(ec)) continue;
        const std::string name = it->path().filename().string();
        const std::string up = upperAscii(name);
        if (up.find("META") != std::string::npos) continue;
        const size_t dot = up.rfind('.');
        const std::string ext = dot == std::string::npos ? "" : up.substr(dot);
        if (ext != ".TXT" && ext != ".ACT" && ext != ".INC" && ext != ".ACE")
            continue;
        const std::string stem =
            dot == std::string::npos ? name : name.substr(0, dot);
        // "NUMBER_TIBETAN TITLE_English title_AUTHOR"
        const size_t u1 = stem.find('_');
        if (u1 == std::string::npos) continue;
        const size_t u2 = stem.find('_', u1 + 1);
        const std::string key = stem.substr(0, u1);
        const std::string tib = stem.substr(
            u1 + 1, u2 == std::string::npos ? std::string::npos : u2 - u1 - 1);
        std::string eng;
        if (u2 != std::string::npos) {
            const size_t u3 = stem.find('_', u2 + 1);
            eng = stem.substr(u2 + 1, u3 == std::string::npos
                                          ? std::string::npos
                                          : u3 - u2 - 1);
        }
        if (add(key, tib, "library filename", eng)) ++banked;
    }
    return banked;
}

std::vector<IdentityCandidate> suggestIdentity(const TitleExtraction& t,
                                               const TitleBank& bank,
                                               int limit, double floor_score) {
    std::vector<IdentityCandidate> out;
    if (!t.found || limit <= 0) return out;

    std::vector<std::pair<std::string, std::vector<std::string>>> readings;
    readings.emplace_back(t.title, syllables(t.title));
    for (const auto& v : t.variants) readings.emplace_back(v, syllables(v));

    // best candidate per catalog key — a work filed under several filenames
    // must not fill the list with copies of itself
    std::unordered_map<std::string, size_t> byKey;
    for (const auto& e : bank.entries()) {
        const auto bs = syllables(e.title);
        IdentityCandidate best;
        for (const auto& r : readings) {
            int shared = 0;
            const double s = overlap(r.second, bs, &shared);
            if (s <= best.score) continue;
            best.score = s;
            best.shared = shared;
            best.extracted_syllables = static_cast<int>(r.second.size());
            best.matched_variant = r.first;
        }
        if (best.score < floor_score) continue;
        best.key = e.key;
        best.raw = e.raw;
        best.eng = e.eng;
        best.source = e.source;
        best.bank_syllables = static_cast<int>(bs.size());
        best.basis = best.matched_variant == e.title ? "exact title"
                                                     : "syllable overlap";
        auto it = byKey.find(e.key);
        if (it == byKey.end()) {
            byKey[e.key] = out.size();
            out.push_back(std::move(best));
        } else if (out[it->second].score < best.score) {
            out[it->second] = std::move(best);
        }
    }
    std::sort(out.begin(), out.end(),
              [](const IdentityCandidate& a, const IdentityCandidate& b) {
                  if (a.score != b.score) return a.score > b.score;
                  return a.key < b.key;
              });
    if (static_cast<int>(out.size()) > limit) out.resize(limit);
    return out;
}

}  // namespace allcore

namespace allcore {
namespace {

// nearest preceding page marker "@NNN[AB]" before pos, as "NNNA"
std::string folioBefore(const std::string& up, size_t pos) {
    size_t at = up.rfind('@', pos);
    while (at != std::string::npos) {
        size_t j = at + 1;
        while (j < up.size() && up[j] == ' ') ++j;
        size_t d = j;
        while (d < up.size() && std::isdigit(static_cast<unsigned char>(up[d])))
            ++d;
        if (d > j && d < up.size() && (up[d] == 'A' || up[d] == 'B'))
            return up.substr(j, d - j + 1);
        if (at == 0) break;
        at = up.rfind('@', at - 1);
    }
    return "";
}

bool closingFormulaBefore(const std::string& up, size_t pos) {
    const size_t w = pos > 400 ? pos - 400 : 0;
    const std::string win = up.substr(w, pos - w);
    for (const char* f : {"RDZOGS S", "DGE'O", "DGE LEGS", "MANGGA LAM",
                          "BKRA SHIS", "DZOGS SO"})
        if (win.find(f) != std::string::npos) return true;
    return false;
}

}  // namespace

std::vector<SplitCandidate> suggestVolumeSplits(const std::string& doc) {
    std::string up = doc;
    for (char& c : up)
        c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));

    std::vector<SplitCandidate> out;

    // Rule 1: canonical bilingual heads. Strong signal — BOD SKAD DU only
    // appears where a translated work announces itself.
    const std::string kBod = "BOD SKAD DU";
    for (size_t pos = up.find(kBod); pos != std::string::npos;
         pos = up.find(kBod, pos + kBod.size())) {
        size_t start = pos;
        for (const char* cue : {"RGYA GAR SKAD DU", "RGYA'I SKAD DU"}) {
            const size_t g = up.rfind(cue, pos);
            if (g != std::string::npos && pos - g < 300) {
                start = g;
                break;
            }
        }
        const auto t = extractAcipTitle(doc.substr(start, 4000));
        if (!t.found || t.rule != "bod-skad-du") continue;
        SplitCandidate c;
        c.offset = start;
        c.rule = "bod-skad-du";
        c.title = t.title;
        c.raw = t.raw;
        c.folio = folioBefore(up, start);
        c.closing_before = closingFormulaBefore(up, start);
        out.push_back(std::move(c));
    }

    // Rule 2: title blocks closing in BZHUGS SO (the Sungbum shape). A
    // real title block is ANCHORED — it opens the document, follows a
    // folio marker, or follows a line-start '*' (the pecha title-page
    // mark) — and it reads like a title, not prose: almost no commas.
    // (Measured on the library: narrative prose uses "bzhugs so" freely —
    // one Avatamsaka text alone yielded 93 false blocks before the
    // comma-density gate; a real title block carries at most a few.)
    const std::string kZ = "BZHUGS SO";
    for (size_t z = up.find(kZ); z != std::string::npos;
         z = up.find(kZ, z + kZ.size())) {
        const size_t w = z > 800 ? z - 800 : 0;
        // candidate anchors, latest wins
        size_t at = std::string::npos;
        if (w == 0) at = 0;   // the document's own opening
        const size_t fol = up.rfind('@', z);
        if (fol != std::string::npos && fol >= w &&
            (at == std::string::npos || fol > at))
            at = fol;
        // a line that BEGINS with '*' opens a title page mid-volume
        for (size_t nl = up.rfind('\n', z);
             nl != std::string::npos && nl >= w && nl > 0;
             nl = up.rfind('\n', nl - 1)) {
            size_t k = nl + 1;
            while (k < z && (up[k] == ' ' || up[k] == '\r')) ++k;
            if (k < z && up[k] == '*') {
                if (at == std::string::npos || k > at) at = k;
                break;
            }
            if (nl == 0) break;
        }
        if (at == std::string::npos) continue;
        // TRIED AND REJECTED BY MEASUREMENT: requiring an end-of-work
        // mark (",," / blank line) before the anchor removed ZERO false
        // blocks (the pathological prose carries ",," too) and cost a
        // real junction. The residual false-positive source is narrative
        // "bzhugs so" in list-like scripture (one Avatamsaka text = 45
        // of 49 extras across 120 sampled texts); those candidates are
        // visibly prose fragments in the evidence panel, and a human
        // dismisses them — this is a suggest-only lane.
        const std::string rawBlock = doc.substr(at, z - at);
        // prose gate: shad-commas everywhere means running text
        int commas = 0;
        for (char ch : rawBlock)
            if (ch == ',') ++commas;
        if (commas > 4) continue;
        const auto t =
            extractAcipTitle(doc.substr(at, (z - at) + kZ.size() + 4));
        if (!t.found || t.rule != "bzhugs-so") continue;
        // plausibility: a real title block is not a whole page of prose
        int syl = 1;
        for (char ch : t.title)
            if (ch == ' ') ++syl;
        if (syl > 45) continue;
        SplitCandidate c;
        c.offset = at;
        c.rule = "bzhugs-so";
        c.title = t.title;
        c.raw = t.raw;
        c.folio = folioBefore(up, z);
        c.closing_before = closingFormulaBefore(up, at);
        out.push_back(std::move(c));
    }

    std::sort(out.begin(), out.end(),
              [](const SplitCandidate& a, const SplitCandidate& b) {
                  return a.offset < b.offset;
              });

    // One title page can be detected twice: a BZHUGS SO title block often
    // stands immediately before the bilingual head of the SAME text
    // (Kangyur convention). Cross-rule neighbors within 600 bytes are one
    // boundary — keep the earlier offset, prefer the bilingual head's
    // title (the stronger rule). Same-rule neighbors are NOT merged; the
    // proximity warning below flags them for the human instead.
    for (size_t i = 1; i < out.size();) {
        if (out[i].rule != out[i - 1].rule &&
            out[i].offset - out[i - 1].offset < 600) {
            SplitCandidate keep =
                out[i].rule == "bod-skad-du" ? out[i] : out[i - 1];
            keep.offset = out[i - 1].offset;
            keep.folio = out[i - 1].folio.empty() ? out[i].folio
                                                  : out[i - 1].folio;
            keep.closing_before =
                out[i - 1].closing_before || out[i].closing_before;
            out[i - 1] = std::move(keep);
            out.erase(out.begin() + i);
        } else {
            ++i;
        }
    }

    // "Don't split what the author didn't split": a boundary whose title
    // reads like a chapter or a part of the same work gets a warning.
    for (auto& c : out) {
        if (c.title.find(" LE'U") != std::string::npos ||
            c.title.rfind("LE'U", 0) == 0) {
            c.warn = true;
            c.warn_reason =
                "the title names a chapter (LE'U) - likely a part of one "
                "work, not an independent text";
        }
    }
    for (size_t i = 1; i < out.size(); ++i) {
        if (out[i].offset - out[i - 1].offset < 1500 && !out[i].warn) {
            out[i].warn = true;
            out[i].warn_reason =
                "very close to the previous boundary - possibly parts of "
                "one work (an empowerment's sections, a broken head)";
        }
    }
    return out;
}

}  // namespace allcore
