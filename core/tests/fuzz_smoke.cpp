// fuzz_smoke — T3: the engine seams under hostile input.
//
// The batteries prove the KNOWN cases; this hunts the unknown:
// mutated wylie/ACIP/unicode — splices, stack pileups, orphan '+',
// control bytes, broken UTF-8, pathological lengths — through every
// public converter. The engines' contract under garbage is simple:
// return SOMETHING, bounded, without crashing or hanging; honest
// flagging is the converters' own business (rule 3 covers content).
//
// Deterministic (fixed-seed mt19937) so a failure reproduces exactly;
// any interesting survivor gets promoted to a named fixture below.
#include <cstdio>
#include <random>
#include <string>
#include <vector>

#include "allcore/engines.h"
#include "allcore/unicode_wylie.h"

static int failures = 0;
#define CHECK(cond, what)                                   \
    do {                                                    \
        const bool ok = (cond);                             \
        if (!ok) {                                          \
            std::printf("  [FAIL] %s\n", what);             \
            ++failures;                                     \
        }                                                   \
    } while (0)

int main() {
    // seed corpus: real shapes the mutators splice from
    const std::vector<std::string> seeds = {
        "bsod nams", "byang chub sems dpa'", "sems can",
        "rnam par shes pa", "sat+t+wa", "badz+ra", "oM",
        "dz+nyA na", "khaMs", "'jam dpal dbyangs", "brgyad",
        "bskyabs", "mngon sum", "kSha", "shes rab kyi pha rol",
        "BSOD NAMS", "SEMS CAN", "@01A ,BDEN PA,",
        "བསོད་ནམས",
        "སྟྭ་ཨོཾ",
    };
    const std::string specials =
        "+*~^'\"`.-_[]{}()<>#@!%&|\\/0123456789 \t";

    std::mt19937 rng(0xA11CE);
    auto pick = [&](size_t n) { return rng() % n; };

    long iters = 0;
    for (int round = 0; round < 20000; ++round) {
        // build a mutant
        std::string m = seeds[pick(seeds.size())];
        switch (pick(6)) {
            case 0:   // splice two seeds
                m += seeds[pick(seeds.size())];
                break;
            case 1:   // inject specials
                for (int k = 0; k < 8; ++k)
                    m.insert(pick(m.size() + 1),
                             1, specials[pick(specials.size())]);
                break;
            case 2:   // stack pileup
                for (int k = 0; k < 12; ++k)
                    m.insert(pick(m.size() + 1), "+w");
                break;
            case 3:   // truncate mid-UTF8
                if (m.size() > 3) m.resize(m.size() - 1 - pick(2));
                break;
            case 4:   // raw control/high bytes
                for (int k = 0; k < 6; ++k)
                    m.insert(pick(m.size() + 1), 1,
                             (char)(0x80 + pick(0x7f)));
                break;
            case 5:   // repetition blowup (bounded)
                while (m.size() < 2000) m += m;
                m.resize(1500 + pick(500));
                break;
        }
        ++iters;
        // every converter must return, bounded, on every mutant
        auto [uni, ok1] = allcore::wylieToUnicode(m);
        CHECK(uni.size() < m.size() * 24 + 64,
              "wylieToUnicode output bounded");
        const auto rev = allcore::unicodeToWylie(m);
        CHECK(rev.wylie.size() < m.size() * 24 + 64,
              "unicodeToWylie output bounded");
        const std::string ew = allcore::acipToEwts(m);
        CHECK(ew.size() < m.size() * 24 + 64,
              "acipToEwts output bounded");
        const std::string pr = allcore::pronounce(m);
        CHECK(pr.size() < m.size() * 24 + 64,
              "pronounce output bounded");
        if (failures > 20) break;   // enough to diagnose
    }

    // promoted fixtures: shapes that once looked risky, pinned forever
    {
        auto [u1, o1] = allcore::wylieToUnicode(std::string(1500, '+'));
        CHECK(true, "orphan-plus wall returns");
        auto [u2, o2] = allcore::wylieToUnicode("");
        CHECK(u2.empty(), "empty input, empty output");
        const auto r = allcore::unicodeToWylie(std::string("\xe0\xbd", 2));
        CHECK(true, "truncated UTF-8 returns");
        (void)o1; (void)o2; (void)u1; (void)r;
    }

    std::printf("fuzz_smoke: %ld mutants, %s (%d failures)\n", iters,
                failures ? "FAIL" : "ALL SURVIVED", failures);
    return failures ? 1 : 0;
}
