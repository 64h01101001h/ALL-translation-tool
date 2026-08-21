// forward_battery — F2 leg A at dictionary scale: the C++ forward
// engine (wylie → unicode) against fixtures emitted by the CANONICAL
// Python (ewts_unicode.py, rule 2's oracle) over a stride-3 sample of
// the whole dictionary (35,211 headwords).
//
// The pin is a RATCHET: agreement may never fall below the floor.
// (Measured at enactment; divergences are the port-vs-python delta on
// real headwords — each future fix may only raise the floor.)
#include <cstdio>
#include <fstream>
#include <string>

#include "allcore/engines.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        std::printf("usage: forward_battery <forward_reference.tsv>\n");
        return 2;
    }
    std::ifstream f(argv[1]);
    if (!f) {
        std::printf("forward_battery: cannot open %s\n", argv[1]);
        return 2;
    }
    long total = 0, agree = 0, shown = 0;
    std::string line;
    while (std::getline(f, line)) {
        const auto tab = line.find('\t');
        if (tab == std::string::npos) continue;
        const std::string wylie = line.substr(0, tab);
        const std::string want = line.substr(tab + 1);
        ++total;
        // ok=false only signals flagged (⟨⟩) tokens — the output
        // is still the engine's honest answer; parity is CONTENT
        auto [got, ok] = allcore::wylieToUnicode(wylie);
        (void)ok;
        if (got == want) {
            ++agree;
        } else if (shown < 8) {
            std::printf("  diverges: %s\n    py:  %s\n    c++: %s\n",
                        wylie.c_str(), want.c_str(), got.c_str());
            ++shown;
        }
    }
    const double pct = total ? 100.0 * agree / total : 0.0;
    // the ratchet floor — raise only, never lower
    const double kFloor = 99.95;   // measured 100.000% at enactment
    std::printf("forward_battery: %ld/%ld agree (%.3f%%), floor %.2f%%\n",
                agree, total, pct, kFloor);
    return (total > 30000 && pct >= kFloor) ? 0 : 1;
}
