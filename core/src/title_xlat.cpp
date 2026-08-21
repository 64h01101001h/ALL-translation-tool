// title_xlat.cpp — see allcore/title_xlat.h. Attestation, never
// composition.
#include "allcore/title_xlat.h"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <set>
#include <sstream>

#include "allcore/catalog_id.h"
#include "allcore/tibexport.h"

namespace allcore {
namespace {

std::vector<std::string> syls(const std::string& norm) {
    std::vector<std::string> out;
    std::istringstream is(norm);
    std::string w;
    while (is >> w) out.push_back(w);
    return out;
}

std::string joinSpan(const std::vector<std::string>& v, size_t from,
                     size_t n) {
    std::string s;
    for (size_t i = from; i < from + n; ++i) {
        if (!s.empty()) s += ' ';
        s += v[i];
    }
    return s;
}

// does needle appear in hay as a whole-syllable contiguous span?
bool spanIn(const std::string& hay, const std::string& needle) {
    const std::string h = " " + hay + " ";
    const std::string n = " " + needle + " ";
    return h.find(n) != std::string::npos;
}

}  // namespace

bool TitlePairBank::add(const std::string& tib_raw, const std::string& eng,
                        const std::string& source) {
    TitlePair p;
    p.tib_raw = tib_raw;
    p.tib_norm = normalizeAcipTitle(tib_raw);
    p.eng = eng;
    p.source = source;
    if (p.eng.empty() || syls(p.tib_norm).size() < 2) return false;
    entries_.push_back(std::move(p));
    return true;
}

int TitlePairBank::addLibraryTree(const std::string& root) {
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
        std::string up = name;
        for (char& c : up)
            c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
        if (up.find(" META.") != std::string::npos) continue;
        const size_t dot = up.rfind('.');
        const std::string ext = dot == std::string::npos ? "" : up.substr(dot);
        if (ext != ".TXT" && ext != ".ACT" && ext != ".INC" && ext != ".ACE")
            continue;
        if (!decodeAcipFilename(name).recognized) continue;
        std::string stem = name.substr(0, name.rfind('.'));
        const size_t u1 = stem.find('_');
        if (u1 == std::string::npos) continue;
        const size_t u2 = stem.find('_', u1 + 1);
        if (u2 == std::string::npos) continue;
        const size_t u3 = stem.find('_', u2 + 1);
        const std::string tib = stem.substr(u1 + 1, u2 - u1 - 1);
        const std::string eng =
            stem.substr(u2 + 1, u3 == std::string::npos ? std::string::npos
                                                        : u3 - u2 - 1);
        if (add(tib, eng, "library filename")) ++banked;
    }
    return banked;
}

TitleWorkbench buildTitleWorkbench(const std::string& tibetan_title,
                                   const TitlePairBank& bank,
                                   int max_whole, double whole_floor,
                                   int max_span, int max_examples) {
    TitleWorkbench w;
    const std::string qnorm = normalizeAcipTitle(tibetan_title);
    const auto q = syls(qnorm);
    if (q.empty()) return w;

    // ---- whole-title matches (shared syllables / longer title) ----------
    const std::set<std::string> qset(q.begin(), q.end());
    std::vector<WholeTitleMatch> all;
    for (const auto& e : bank.entries()) {
        const auto es = syls(e.tib_norm);
        const std::set<std::string> eset(es.begin(), es.end());
        int shared = 0;
        for (const auto& s : qset)
            if (eset.count(s)) ++shared;
        const double score =
            static_cast<double>(shared) /
            static_cast<double>(std::max(qset.size(), eset.size()));
        if (score < whole_floor) continue;
        all.push_back({e.tib_raw, e.eng, e.source, score});
    }
    std::sort(all.begin(), all.end(),
              [](const WholeTitleMatch& a, const WholeTitleMatch& b) {
                  return a.score > b.score;
              });
    if (static_cast<int>(all.size()) > max_whole) all.resize(max_whole);
    w.whole = std::move(all);

    // ---- fragment cover: greedy, longest first ---------------------------
    size_t covered = 0;
    for (size_t i = 0; i < q.size();) {
        FragmentCard best;
        for (int n = std::min<int>(max_span, static_cast<int>(q.size() - i));
             n >= 2; --n) {
            const std::string span = joinSpan(q, i, n);
            FragmentCard card;
            card.fragment = span;
            card.from = i;
            card.count = n;
            for (const auto& e : bank.entries()) {
                if (!spanIn(e.tib_norm, span)) continue;
                if (static_cast<int>(card.examples.size()) < max_examples)
                    card.examples.push_back({e.tib_raw, e.eng, e.source});
                else
                    break;
            }
            if (!card.examples.empty()) {
                best = std::move(card);
                break;   // longest span wins
            }
        }
        if (!best.examples.empty()) {
            covered += best.count;
            const size_t adv = best.count;
            w.fragments.push_back(std::move(best));
            i += adv;
        } else {
            w.uncovered.push_back(q[i]);
            ++i;
        }
    }
    w.coverage = static_cast<double>(covered) / static_cast<double>(q.size());
    return w;
}

}  // namespace allcore
