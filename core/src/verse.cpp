#include "allcore/verse.h"

#include <map>

#include "allcore/lattice.h"

namespace allcore {

VerseReport analyzeVerse(const std::string& acip) {
    VerseReport rep;
    // split into candidate lines at newlines and shad/comma
    std::vector<std::string> raw;
    std::string cur;
    for (char c : acip + "\n") {
        if (c == '\n' || c == '\r' || c == ',' || c == ';' || c == '/') {
            raw.push_back(cur);
            cur.clear();
        } else {
            cur += c;
        }
    }
    int lineNo = 0;
    for (const auto& r : raw) {
        std::vector<std::string> toks;
        std::vector<bool> bars;
        tokenizeDocument(r, toks, bars);
        if (toks.empty()) continue;
        VerseLine l;
        l.number = ++lineNo;
        l.syllables = (int)toks.size();
        for (size_t i = 0; i < toks.size(); ++i) {
            if (i) l.text += ' ';
            l.text += toks[i];
        }
        rep.lines.push_back(std::move(l));
    }
    if (rep.lines.size() < 4) return rep;   // too short to call it verse
    std::map<int, int> counts;
    for (const auto& l : rep.lines) ++counts[l.syllables];
    int best = 0, bestN = 0;
    for (auto& [len, n] : counts)
        if (n > bestN) { best = len; bestN = n; }
    // verse when the dominant count covers at least 70% of the lines and the
    // lines are long enough to be metrical
    if (best >= 5 && bestN * 10 >= (int)rep.lines.size() * 7) {
        rep.is_verse = true;
        rep.meter = best;
        for (auto& l : rep.lines) {
            l.irregular = (l.syllables != best);
            rep.irregular_count += l.irregular;
        }
    }
    return rep;
}

}  // namespace allcore
