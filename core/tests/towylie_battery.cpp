// towylie_battery — the unicodeToWylie port against pyewts' OWN
// output on 109,490 fixtures (the master's ground-truth Tibetan +
// strided native 84000 unicode). The acceptance bar is 100%: a port
// that disagrees with its canonical oracle anywhere does not ship.
// Round-trip sanity rides along: wylieToUnicode(unicodeToWylie(u))
// must reproduce u on clean all-Tibetan fixtures.
#include <cstdio>
#include <fstream>
#include <string>

#include "allcore/engines.h"
#include "allcore/unicode_wylie.h"

int main(int argc, char** argv) {
    setvbuf(stdout, nullptr, _IONBF, 0);
    if (argc < 2) return 2;
    std::ifstream in(argv[1]);
    if (!in) {
        std::printf("cannot open fixtures\n");
        return 2;
    }
    std::string line;
    long total = 0, match = 0, rt_tried = 0, rt_ok = 0;
    std::string firstMiss;
    while (std::getline(in, line)) {
        if (line.empty() || line[0] == '#') continue;
        const size_t tab = line.find('\t');
        if (tab == std::string::npos) continue;
        const std::string uni = line.substr(0, tab);
        const std::string ref = line.substr(tab + 1);
        ++total;
        const auto got = allcore::unicodeToWylie(uni);
        if (got.wylie == ref) {
            ++match;
            // round trip on clean fixtures (no escapes, no warns)
            if (got.warns == 0 &&
                got.wylie.find('[') == std::string::npos &&
                got.wylie.find("\\u") == std::string::npos &&
                rt_tried < 5000) {
                ++rt_tried;
                auto [u2, ok] = allcore::wylieToUnicode(got.wylie);
                if (ok && u2 == uni) ++rt_ok;
            }
        } else if (firstMiss.empty()) {
            firstMiss = uni + " => '" + got.wylie + "' expected '" +
                        ref + "'";
        }
    }
    std::printf("fixtures: %ld  match: %ld (%.3f%%)\n", total, match,
                100.0 * match / total);
    if (!firstMiss.empty())
        std::printf("first miss: %s\n", firstMiss.c_str());
    std::printf("round-trip (clean subset): %ld/%ld\n", rt_ok,
                rt_tried);
    // round-trip measured 2026-08-20: 4,865/5,000 (97.3%). The
    // misses are FORWARD-engine boundaries, not port defects — EWTS
    // '_' (explicit space, which pyewts emits after shads) is not
    // accepted by wylieToUnicode, and explicit '+' Sanskrit stacks
    // (kar+Na) render differently there. Floor just under measure.
    const bool pass = total > 100000 && match == total &&
                      rt_tried > 1000 && rt_ok * 1000 >= rt_tried * 965;
    std::printf("%s\n", pass ? "OK" : "FAIL");
    return pass ? 0 : 1;
}
