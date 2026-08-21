// terminology_smoke — the draft-workspace terminology checker.
#include <cstdio>

#include "allcore/terminology.h"

static int failures = 0;
#define CHECK(cond, msg)                                        \
    do {                                                        \
        if (cond) std::printf("  [PASS] %s\n", msg);            \
        else { std::printf("  [FAIL] %s\n", msg); ++failures; } \
    } while (0)

int main(int argc, char** argv) {
    setvbuf(stdout, nullptr, _IONBF, 0);
    if (argc < 2) {
        std::fprintf(stderr, "usage: terminology_smoke <spine.db>\n");
        return 2;
    }
    allcore::Spine spine(argv[1]);
    allcore::HeadwordIndex index(spine);

    // gloss matching mechanics
    CHECK(allcore::glossMatches("Dharma (the teaching)",
                                "the buddha taught the dharma"),
          "parenthetical stripped: 'Dharma (the teaching)' matches 'dharma'");
    CHECK(allcore::glossMatches("things / objects (phenomena)",
                                "all objects are empty"),
          "slash alternatives match independently");
    CHECK(!allcore::glossMatches("is", "this is a draft"),
          "alternatives under 3 letters never match");

    // a draft that uses HGM equivalents
    {
        auto rep = allcore::checkTerminology(
            spine, index, "SANGS RGYAS KYIS CHOS BSTAN",
            "The Buddha taught the Dharma.");
        const allcore::TermUse* sr = nullptr;
        const allcore::TermUse* chos = nullptr;
        for (const auto& t : rep.terms) {
            if (t.wylie == "sangs rgyas") sr = &t;
            if (t.wylie == "chos") chos = &t;
        }
        CHECK(sr != nullptr && chos != nullptr,
              "terms extracted from maximal spans");
        CHECK(sr && !sr->matched.empty(),
              "sangs rgyas matched via its HGM gloss (Buddha)");
        CHECK(chos && !chos->matched.empty(),
              "chos matched via its HGM gloss (Dharma)");
    }

    // a draft that uses none of them: unmatched terms sort first
    {
        auto rep = allcore::checkTerminology(
            spine, index, "SANGS RGYAS KYIS CHOS BSTAN",
            "Zebras enjoy xylophones.");
        bool anyMatched = false;
        for (const auto& t : rep.terms) anyMatched |= !t.matched.empty();
        CHECK(!rep.terms.empty() && !anyMatched,
              "nonsense draft matches nothing");
        CHECK(rep.terms.front().matched.empty(),
              "unmatched terms are sorted first");
    }

    // repeated term counts occurrences
    {
        auto rep = allcore::checkTerminology(
            spine, index, "BDEN PA DANG BDEN PA", "truth and truth");
        const allcore::TermUse* bp = nullptr;
        for (const auto& t : rep.terms)
            if (t.wylie == "bden pa") bp = &t;
        CHECK(bp && bp->occurrences == 2, "repeated term counted twice");
    }

    std::printf("%s (%d failures)\n",
                failures ? "TERMINOLOGY SMOKE FAILED" : "TERMINOLOGY SMOKE OK",
                failures);
    return failures ? 1 : 0;
}
