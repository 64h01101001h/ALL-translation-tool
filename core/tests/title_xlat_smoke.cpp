// title_xlat_smoke — the title translator's workbench (queue item 5).
// Pins fix the attestation mechanics; the battery measures HELD-OUT
// usefulness on the real library: for each sampled bilingual filename,
// its own pair is excluded and the workbench is built from everything
// else — how much of the title do attested fragments cover, and how
// often does a near-identical whole title surface?
//
// Usage: title_xlat_smoke [library_root]
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <string>
#include <vector>

#include "allcore/title_xlat.h"

static int failures = 0;
#define CHECK(cond, msg)                                        \
    do {                                                        \
        if (cond) std::printf("  [PASS] %s\n", msg);            \
        else { std::printf("  [FAIL] %s\n", msg); ++failures; } \
    } while (0)

int main(int argc, char** argv) {
    setvbuf(stdout, nullptr, _IONBF, 0);

    // ---- pins ------------------------------------------------------------
    {
        allcore::TitlePairBank b;
        b.add("SHES RAB KYI PHA ROL TU PHYIN PA'I SNYING PO",
              "The Heart of the Perfection of Wisdom", "test");
        b.add("SHES RAB KYI PHA ROL TU PHYIN PA'I MAN NGAG",
              "Advices on the Perfection of Wisdom", "test");
        b.add("BYANG CHUB SEMS DPA'I SPYOD PA LA 'JUG PA",
              "A Guide to the Bodhisattva's Way of Life", "test");
        const auto w = allcore::buildTitleWorkbench(
            "SHES RAB KYI PHA ROL TU PHYIN PA'I SGRON MA", b);
        CHECK(!w.whole.empty() &&
                  w.whole[0].eng.find("Perfection of Wisdom") !=
                      std::string::npos,
              "a near-identical title surfaces with its published "
              "English");
        CHECK(!w.fragments.empty() &&
                  w.fragments[0].fragment.rfind("SHES RAB KYI", 0) == 0 &&
                  w.fragments[0].count >= 6,
              "the longest attested span wins (not word-by-word "
              "scraps)");
        bool sgron = false;
        for (const auto& u : w.uncovered)
            if (u == "SGRON") sgron = true;
        CHECK(sgron, "the unattested syllable (SGRON) is listed "
                     "uncovered - a real answer, not a guess");
        CHECK(w.coverage > 0.7 && w.coverage < 1.0,
              "coverage is measured, not claimed");
        {   // SQA TEST-1 survivor TITLEXLAT-SPAN (2026-08-24). The
            // greedy cover walks n from max_span DOWN TO 2, and the
            // header states the contract in words: "contiguous
            // syllable spans of the query (2..max_span syllables)".
            // Nothing asserted the floor, so `n >= 2` -> `n >= 1`
            // survived the whole battery.
            //
            // A DEDICATED bank is needed to see it. Against the large
            // query above the mutation is invisible, because the
            // leftover syllables are not attested singly either - the
            // first version of this pin passed on the mutant and had
            // to be thrown away. Here KA is attested (inside KA KHA)
            // and GA is not, so a one-syllable cover is available to
            // any implementation willing to emit one.
            allcore::TitlePairBank b2;
            b2.add("KA KHA", "alpha beta", "test");
            const auto w2 = allcore::buildTitleWorkbench("KA GA", b2);
            size_t shortest2 = 999;
            for (const auto& f : w2.fragments)
                shortest2 = std::min(shortest2, f.count);
            CHECK(w2.fragments.empty() || shortest2 >= 2,
                  "no fragment card is a single syllable - the cover "
                  "attests spans, not particles (TEST-1 "
                  "TITLEXLAT-SPAN)");
        }
        // rule 1: every example is a full pair, never a composed gloss
        CHECK(!w.fragments[0].examples.empty() &&
                  !w.fragments[0].examples[0].tib_raw.empty() &&
                  !w.fragments[0].examples[0].eng.empty(),
              "fragment evidence is full published pairs (match, "
              "never compose)");
    }
    CHECK(allcore::buildTitleWorkbench("ZZZQ XYZW", allcore::TitlePairBank())
                  .whole.empty(),
          "an empty bank yields no matches, not an invention");

    if (argc < 2) {
        std::printf("  [SKIP] library battery (no root)\n");
        std::printf("%s\n", failures ? "FAILURES" : "title_xlat_smoke OK");
        return failures ? 1 : 0;
    }

    // ---- battery: held-out measurement over the real library -------------
    allcore::TitlePairBank bank;
    const int banked = bank.addLibraryTree(argv[1]);
    std::printf("  bank: %d bilingual title pairs from the library\n",
                banked);
    CHECK(banked > 3000, "the library yields a real pair bank "
                     "(3,613 as shipped; the app adds the "
                     "catalog_works.json pairs on top)");

    // deterministic stride sample of held-out queries
    const auto& entries = bank.entries();
    const size_t stride = std::max<size_t>(1, entries.size() / 150);
    int queries = 0, wholeHits = 0;
    double covSum = 0.0;
    for (size_t i = 0; i < entries.size() && queries < 150; i += stride) {
        const auto& held = entries[i];
        allcore::TitlePairBank rest;
        for (size_t j = 0; j < entries.size(); ++j) {
            if (j == i) continue;
            // exclude ALL entries with the identical normalized title
            // (duplicate filings of the same work would leak the answer)
            if (entries[j].tib_norm == held.tib_norm) continue;
            rest.add(entries[j].tib_raw, entries[j].eng,
                     entries[j].source);
        }
        const auto w = allcore::buildTitleWorkbench(held.tib_raw, rest);
        ++queries;
        if (!w.whole.empty()) ++wholeHits;
        covSum += w.coverage;
    }
    const double meanCov = queries ? covSum / queries : 0.0;
    std::printf(
        "  battery: %d held-out titles · %.1f%% mean fragment coverage "
        "· %d (%.1f%%) with a whole-title near match\n",
        queries, 100.0 * meanCov, wholeHits,
        queries ? 100.0 * wholeHits / queries : 0.0);
    CHECK(meanCov >= 0.60,
          "attested fragments cover at least 60% of a held-out title "
          "on average");
    CHECK(queries > 0 && wholeHits * 100 >= queries * 25,
          "a near whole-title match surfaces for at least a quarter "
          "of held-out titles");

    std::printf("%s\n", failures ? "FAILURES" : "title_xlat_smoke OK");
    return failures ? 1 : 0;
}
