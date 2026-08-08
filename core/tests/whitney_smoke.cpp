// whitney_smoke — the Whitney reference layer (Roots 1885 + Grammar
// 1879 citations; data/whitney/README.md for provenance and the
// form-level class caveat).
#include <cstdio>
#include <string>

#include "allcore/whitney.h"

static int failures = 0;
#define CHECK(cond, msg)                                        \
    do {                                                        \
        if (cond) std::printf("  [PASS] %s\n", msg);            \
        else { std::printf("  [FAIL] %s\n", msg); ++failures; } \
    } while (0)

int main(int argc, char** argv) {
    if (argc < 2) {
        std::fprintf(stderr, "usage: whitney_smoke <whitney_roots.tsv>\n");
        return 2;
    }
    allcore::WhitneyRoots w;
    CHECK(w.load(argv[1]), "layer loads");
    std::printf("  %zu roots\n", w.size());
    CHECK(w.size() >= 930, "930+ roots (Whitney lists 938)");

    CHECK(allcore::foldIast("bhū") == "bhu", "fold: bhū -> bhu");
    CHECK(allcore::foldIast("kṛṣ") == "krs", "fold: kṛṣ -> krs");
    CHECK(allcore::foldIast("aṃh") == "amh", "fold: aṃh -> amh");

    auto bhu = w.byRoot("bhū");
    CHECK(bhu.size() == 1 && bhu[0]->meaning == "be",
          "byRoot exact: bhū = be");
    CHECK(w.byRoot("bhu").size() == 1,
          "byRoot folded: plain 'bhu' finds bhū");

    auto kr = w.byRoot("kṛ");
    CHECK(kr.size() >= 3, "kṛ homonyms distinct (3 entries)");
    bool make = false, scatter = false;
    const allcore::WhitneyRoot* kr1 = nullptr;
    const allcore::WhitneyRoot* kr2 = nullptr;
    for (const auto* r : kr) {
        if (r->homonym == "1" && r->meaning == "make") {
            make = true;
            kr1 = r;
        }
        if (r->homonym == "2" && r->meaning == "scatter") {
            scatter = true;
            kr2 = r;
        }
    }
    CHECK(make && scatter, "1 kṛ = make, 2 kṛ = scatter (Whitney's "
                           "numbering preserved)");
    CHECK(!kr.empty() && !kr[0]->grammarSecs.empty(),
          "kṛ carries Grammar-1879 section citations");
    // per-homonym classes from the machine hub (the acid test that
    // rejected the form-level table)
    CHECK(kr1 && kr1->classes.find("VIII") != std::string::npos,
          "1 kṛ class list includes VIII (karoti)");
    CHECK(kr2 && kr2->classes == "VI" &&
              kr2->classes != kr1->classes,
          "2 kṛ class VI — homonyms carry DIFFERENT classes");
    CHECK(kr1 && kr1->ppp == "kṛtá",
          "accented PPP banked: 1 kṛ -> kṛtá");
    CHECK(kr1 && !kr1->mwId.empty(),
          "MW crosswalk id present for 1 kṛ");
    CHECK(kr1 && kr1->slp1 == "kf",
          "SLP1 deep-link key banked (kṛ -> kf)");
    CHECK(allcore::foldIast("gatá") == "gata",
          "accent fold: gatá -> gata");
    auto ppp = w.byPpp("gata");
    bool pppGam = false;
    for (const auto* r : ppp) pppGam |= r->root == "gam";
    CHECK(pppGam, "byPpp reverse: plain 'gata' finds gam");
    CHECK(!w.byPpp("krta").empty(),
          "byPpp folded: 'krta' finds kṛtá");
    auto bhu3 = w.byRoot("bhū");
    CHECK(bhu3.size() == 1 && !bhu3[0]->sectionRefs.empty() &&
              bhu3[0]->sectionRefs.find("perfect") != std::string::npos,
          "topical Grammar ranges banked (bhū has perfect:…)");

    auto gam = w.byRoot("gam");
    CHECK(gam.size() == 1 && gam[0]->meaning == "go", "gam = go");
    CHECK(gam.size() == 1 && gam[0]->ppp == "gatá", "gam ppp gatá");
    auto bhu2 = w.byRoot("bhū");
    CHECK(bhu2.size() == 1 && bhu2[0]->ppp == "bhūtá",
          "bhū ppp bhūtá");

    auto scat = w.byMeaning("scatter");
    bool hasKr = false;
    for (const auto* r : scat) hasKr |= r->root == "kṛ";
    CHECK(hasKr, "byMeaning('scatter') reaches 2 kṛ");
    CHECK(w.byMeaning("be").size() > 0 &&
              w.byMeaning("be")[0]->meaning.find("be") !=
                  std::string::npos,
          "byMeaning whole-word: 'be' matches 'be', not every 'bend'");
    bool bendLeak = false;
    for (const auto* r : w.byMeaning("be"))
        if (r->meaning == "bend") bendLeak = true;
    CHECK(!bendLeak, "whole-word match excludes 'bend' for 'be'");

    // Whitney's own annotations survive verbatim
    auto hur = w.byRoot("hur");
    CHECK(hur.size() == 1 &&
              hur[0]->meaning.find("see √hvṛ") != std::string::npos,
          "cross-reference roots kept (hur: see √hvṛ)");

    std::printf("whitney_smoke: %s (%d failures)\n",
                failures ? "FAIL" : "ALL PASS", failures);
    return failures ? 1 : 0;
}
