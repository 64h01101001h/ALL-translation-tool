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
