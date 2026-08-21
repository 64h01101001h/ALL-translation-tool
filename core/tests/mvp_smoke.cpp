// mvp_smoke — the Mahāvyutpatti reference table (DILA TEI, CC BY-SA 3.0).
// Fixtures are the glossary's own first entries (buddhaḥ / bhagavān /
// tathāgataḥ) plus honesty checks: exact-only lookup, DDBC editorial
// additions carrying their {ddbc} marker.
#include <cstdio>
#include <string>

#include "allcore/mvp.h"

static int failures = 0;
#define CHECK(cond, msg)                                        \
    do {                                                        \
        if (cond) std::printf("  [PASS] %s\n", msg);            \
        else { std::printf("  [FAIL] %s\n", msg); ++failures; } \
    } while (0)

int main(int argc, char** argv) {
    if (argc < 2) {
        std::fprintf(stderr, "usage: mvp_smoke <mahavyutpatti.tsv>\n");
        return 2;
    }
    allcore::Mvp mvp;
    CHECK(mvp.load(argv[1]), "table loads");
    std::printf("  %zu entries\n", mvp.size());
    CHECK(mvp.size() == 9379, "all 9,379 entries banked");

    auto sr = mvp.byWylie("sangs rgyas");
    CHECK(!sr.empty() && sr.front()->key == 1 && sr.front()->iast == "buddhaḥ",
          "sangs rgyas -> MVP 1 buddhaḥ");
    CHECK(!sr.empty() && !sr.front()->tibetan.empty() &&
              sr.front()->tibetan.front() == "སངས་རྒྱས་",
          "Tibetan script carried");
    CHECK(!mvp.byWylie("SANGS RGYAS").empty(), "wylie lookup case-folds");

    auto tg = mvp.byIast("tathāgataḥ");
    CHECK(!tg.empty() && !tg.front()->wylie.empty() &&
              tg.front()->wylie.front() == "de bzhin gshegs pa",
          "tathāgataḥ -> de bzhin gshegs pa");

    const auto* e2 = mvp.byKey(2);
    bool ddbcMarked = false;
    if (e2)
        for (const auto& c : e2->chinese)
            if (c.find("{ddbc}") != std::string::npos) ddbcMarked = true;
    CHECK(e2 && ddbcMarked,
          "MVP 2: DDBC editorial addition carries its {ddbc} marker");

    CHECK(mvp.byWylie("kag dag kag").empty() && mvp.byIast("xyz").empty(),
          "unknown terms return nothing (exact-only, never guessed)");

    std::printf("mvp_smoke: %s (%d failures)\n",
                failures ? "FAIL" : "ALL PASS", failures);
    return failures ? 1 : 0;
}
