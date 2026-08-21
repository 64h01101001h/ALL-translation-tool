// weird_battery — G2: the dictionary's weirdest 1,000 headwords
// (ranked by tools/weirdness_rank.py) through every converter.
// Stride sampling is structurally blind to this tail; these rows are
// where real-world breakage concentrates. Contract under weirdness:
// return, bounded, honestly flagged — never crash, never hang.
#include <cstdio>
#include <fstream>
#include <string>

#include "allcore/engines.h"
#include "allcore/unicode_wylie.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        std::printf("usage: weird_battery <weird_top.tsv>\n");
        return 2;
    }
    std::ifstream f(argv[1]);
    if (!f) {
        std::printf("weird_battery: cannot open %s\n", argv[1]);
        return 2;
    }
    long total = 0, fails = 0, flagged = 0;
    std::string line;
    while (std::getline(f, line)) {
        const auto t1 = line.find('\t');
        const auto t2 = line.find('\t', t1 + 1);
        if (t1 == std::string::npos || t2 == std::string::npos)
            continue;
        const std::string wylie = line.substr(t1 + 1, t2 - t1 - 1);
        ++total;
        auto [uni, ok] = allcore::wylieToUnicode(wylie);
        if (!ok) ++flagged;
        if (uni.size() > wylie.size() * 24 + 64) {
            std::printf("  UNBOUNDED forward: %s\n", wylie.c_str());
            ++fails;
        }
        const auto rev = allcore::unicodeToWylie(uni);
        if (rev.wylie.size() > uni.size() * 24 + 64) {
            std::printf("  UNBOUNDED reverse: %s\n", wylie.c_str());
            ++fails;
        }
        const std::string pr = allcore::pronounce(wylie);
        if (pr.size() > wylie.size() * 24 + 64) {
            std::printf("  UNBOUNDED pron: %s\n", wylie.c_str());
            ++fails;
        }
        if (fails > 10) break;
    }
    std::printf("weird_battery: %ld weirdest rows, %ld flagged "
                "(honest), %ld failures\n",
                total, flagged, fails);
    return (total > 900 && fails == 0) ? 0 : 1;
}
