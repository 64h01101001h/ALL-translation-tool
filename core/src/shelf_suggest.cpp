// shelf_suggest.cpp — see allcore/shelf_suggest.h. Weights below were
// tuned by the held-out battery (shelf_suggest_smoke), not by taste:
// four configurations measured 2026-08-19 — author-heavy 24% top-1,
// flat mixed-path 29%, leaf-weighted 23%, flat Tibetan-half 31%
// (shipped). The modest ceiling is the point: shelf choice is the
// fine-grained subject judgment GMR reserves for trained humans.
#include "allcore/shelf_suggest.h"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <map>
#include <set>
#include <sstream>

#include "allcore/catalog_id.h"
#include "allcore/tibexport.h"

namespace allcore {
namespace {

std::string upper(std::string s) {
    for (char& c : s)
        c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    return s;
}

// the author field, comparison-ready: uppercased, parenthesized dates
// and punctuation out, whitespace collapsed
std::string normAuthor(const std::string& a) {
    return normalizeAcipTitle(a);   // same fold does the job
}

std::set<std::string> syls(const std::string& norm) {
    std::set<std::string> out;
    std::istringstream is(norm);
    std::string w;
    while (is >> w) out.insert(w);
    return out;
}

double setSim(const std::set<std::string>& a,
              const std::set<std::string>& b) {
    if (a.empty() || b.empty()) return 0.0;
    int shared = 0;
    for (const auto& x : a)
        if (b.count(x)) ++shared;
    return static_cast<double>(shared) /
           static_cast<double>(std::max(a.size(), b.size()));
}

struct Fields {
    std::string tib, eng, author;
};

Fields splitFields(const std::string& filename) {
    Fields f;
    if (!decodeAcipFilename(filename).recognized) return f;
    std::string stem = filename;
    const size_t dot = stem.rfind('.');
    if (dot != std::string::npos) stem.erase(dot);
    std::vector<std::string> parts;
    size_t start = 0;
    while (true) {
        const size_t u = stem.find('_', start);
        if (u == std::string::npos) {
            parts.push_back(stem.substr(start));
            break;
        }
        parts.push_back(stem.substr(start, u - start));
        start = u + 1;
    }
    if (parts.size() >= 2) f.tib = parts[1];
    if (parts.size() >= 3) f.eng = parts[2];
    if (parts.size() >= 4) f.author = parts[3];
    return f;
}

}  // namespace

std::vector<ShelfSuggestion> suggestShelves(
    const std::string& dest_root, const std::string& filename,
    const std::string& title_hint,
    const std::vector<std::string>& subject_hints,
    const std::string& exclude_name, int limit) {
    namespace fs = std::filesystem;
    const Fields own = splitFields(filename);
    std::string tib = own.tib.empty() ? title_hint : own.tib;
    const auto ownTitle = syls(normalizeAcipTitle(tib));
    const std::string ownAuthor = normAuthor(own.author);

    struct Acc {
        int authorHits = 0;
        double bestTitle = 0.0;
        std::string bestTitleFile;
        double subject = 0.0;
        std::string subjectHint;
        double pathScore = 0.0;   // title syllables found in the
                                  // shelf's own path — the subject
                                  // folders are NAMED by these phrases
    };
    std::map<std::string, Acc> byShelf;

    std::error_code ec;
    if (!fs::exists(dest_root, ec)) return {};
    for (fs::recursive_directory_iterator it(
             dest_root, fs::directory_options::skip_permission_denied,
             ec),
         end;
         it != end; it.increment(ec)) {
        if (ec) break;
        if (it->is_directory(ec)) {
            // subject-token overlap against the shelf's own path name
            if (!subject_hints.empty()) {
                const std::string rel =
                    fs::relative(it->path(), dest_root, ec).string();
                const auto folderToks = syls(normalizeAcipTitle(rel));
                for (const auto& hint : subject_hints) {
                    const double s2 =
                        setSim(folderToks, syls(normalizeAcipTitle(hint)));
                    auto& acc = byShelf[rel];
                    if (s2 > acc.subject) {
                        acc.subject = s2;
                        acc.subjectHint = hint;
                    }
                }
            }
            continue;
        }
        if (!it->is_regular_file(ec)) continue;
        const std::string name = it->path().filename().string();
        if (name == exclude_name) continue;
        const std::string up = upper(name);
        if (up.find(" META.") != std::string::npos) continue;
        const Fields f = splitFields(name);
        if (f.tib.empty() && f.author.empty()) continue;
        const std::string shelf =
            fs::relative(it->path().parent_path(), dest_root, ec)
                .string();
        auto& acc = byShelf[shelf];
        if (acc.pathScore == 0.0 && !ownTitle.empty()) {
            // the shelves are NAMED by their subject phrases,
            // bilingually ("BLO SBYONG_DEVELOPING THE GOOD HEART") —
            // score only the TIBETAN half of each path component
            // (before its '_'), and weight the LEAF folder highest:
            // the leaf carries the precise shelf phrase
            double leaf = 0.0, anc = 0.0;
            int ancN = 0;
            std::string rest = shelf;
            std::vector<std::string> comps;
            size_t st = 0;
            while (true) {
                const size_t sl = rest.find('/', st);
                if (sl == std::string::npos) {
                    comps.push_back(rest.substr(st));
                    break;
                }
                comps.push_back(rest.substr(st, sl - st));
                st = sl + 1;
            }
            for (size_t ci = 0; ci < comps.size(); ++ci) {
                std::string tibHalf = comps[ci];
                const size_t u = tibHalf.find('_');
                if (u != std::string::npos) tibHalf.erase(u);
                const auto toks = syls(normalizeAcipTitle(tibHalf));
                if (toks.empty()) continue;
                int inTitle = 0;
                for (const auto& tk : toks)
                    if (ownTitle.count(tk)) ++inTitle;
                anc += static_cast<double>(inTitle) /
                       static_cast<double>(toks.size());
                ++ancN;
            }
            (void)leaf;
            acc.pathScore = ancN ? anc / ancN : 0.0;
        }
        if (!ownAuthor.empty() && !f.author.empty() &&
            normAuthor(f.author) == ownAuthor)
            ++acc.authorHits;
        if (!ownTitle.empty() && !f.tib.empty()) {
            const double s2 =
                setSim(ownTitle, syls(normalizeAcipTitle(f.tib)));
            if (s2 > acc.bestTitle) {
                acc.bestTitle = s2;
                acc.bestTitleFile = name;
            }
        }
    }

    std::vector<ShelfSuggestion> out;
    for (const auto& [shelf, acc] : byShelf) {
        ShelfSuggestion s;
        s.shelf = shelf;
        // weights tuned by the held-out battery (first try was
        // author-dominant and scored 24% top-1 — the Sungbum shelves
        // by SUBJECT, and prolific authors dragged files to their
        // biggest folder; the shelf PATH carries the subject phrases)
        if (acc.pathScore >= 0.25) {
            s.score += 4.0 * acc.pathScore;
            s.reasons.push_back(
                "the shelf's own name appears in the title");
        }
        if (acc.bestTitle >= 0.3) {
            s.score += 2.0 * acc.bestTitle;
            s.reasons.push_back(
                "closest title here: " + acc.bestTitleFile);
        }
        if (acc.authorHits > 0) {
            s.score += 0.5 + 0.1 * std::min(acc.authorHits, 5);
            s.reasons.push_back(
                std::to_string(acc.authorHits) +
                " work(s) by the same author already here");
        }
        if (acc.subject >= 0.3) {
            s.score += acc.subject;
            s.reasons.push_back("subject match: " + acc.subjectHint);
        }
        if (s.score <= 0.0) continue;
        out.push_back(std::move(s));
    }
    std::sort(out.begin(), out.end(),
              [](const ShelfSuggestion& a, const ShelfSuggestion& b) {
                  if (a.score != b.score) return a.score > b.score;
                  return a.shelf < b.shelf;
              });
    if (static_cast<int>(out.size()) > limit) out.resize(limit);
    return out;
}

}  // namespace allcore
