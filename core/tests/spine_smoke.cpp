// spine_smoke — proves allcore reads the spine the same way the Python CLI
// does: exact lookups in three scripts, English reverse, corpus NEAR/OR,
// provenance and tier labels intact. Exit 0 = pass.
#include <cstdio>
#include <string>

#include "allcore/spine.h"

static int failures = 0;

#define CHECK(cond, msg)                                        \
    do {                                                        \
        if (cond) {                                             \
            std::printf("  [PASS] %s\n", msg);                  \
        } else {                                                \
            std::printf("  [FAIL] %s\n", msg);                  \
            ++failures;                                         \
        }                                                       \
    } while (0)

int main(int argc, char** argv) {
    if (argc < 2) {
        std::fprintf(stderr, "usage: spine_smoke <hgm_spine.db>\n");
        return 2;
    }
    allcore::Spine spine(argv[1]);

    std::printf("release: %s v%s (built %s)\n",
                spine.metaValue("release_title").c_str(),
                spine.metaValue("release_version").c_str(),
                spine.metaValue("built_utc").c_str());
    CHECK(spine.metaValue("release_version") == "0.27.2", "release version 0.27.2");

    // wylie lookup — curated entry with register-split glosses
    auto e = spine.lookup("bsod nams");
    CHECK(e.size() == 1, "lookup 'bsod nams' -> 1 entry");
    if (!e.empty()) {
        CHECK(e[0].tibetan == "བསོད་ནམས", "tibetan is བསོད་ནམས");
        CHECK(e[0].tier == "curated", "tier curated");
        CHECK(e[0].pronunciation == "sunam" && e[0].pronunciation_card_attested,
              "pron 'sunam' card-attested");
        bool has_merit = false;
        for (auto& g : e[0].hgm_gloss) has_merit |= (g == "merit");
        CHECK(has_merit, "hgm_gloss contains 'merit'");
        CHECK(!e[0].provisional(), "curated entry is not provisional");
    }

    // Tibetan-unicode lookup (trailing tsheg tolerated)
    e = spine.lookup("བྱང་ཆུབ་");
    CHECK(!e.empty() && e[0].wylie == "byang chub",
          "lookup བྱང་ཆུབ་ -> 'byang chub'");

    // ACIP lookup
    e = spine.lookup("SDUG BSNGAL");
    CHECK(!e.empty() && e[0].wylie == "sdug bsngal",
          "lookup ACIP 'SDUG BSNGAL' -> 'sdug bsngal'");

    // provisional tier must be visible as provisional
    bool saw_provisional = false;
    for (auto& hit : spine.englishSearch("merit", /*binding_only=*/true, 50))
        saw_provisional |= hit.provisional();
    // (auto-aligned 'merit' entries exist in the release)
    CHECK(saw_provisional, "englishSearch surfaces provisional-tier entries as such");

    // generated-Tibetan provenance retained
    e = spine.lookup("kva yi");
    CHECK(!e.empty() && !e[0].tibetan_source.empty(),
          "'kva yi' carries tibetan_source (generated)");
    if (!e.empty()) {
        bool flagged = false;
        for (auto& f : e[0].flags) flagged |= (f == "tibetan-review");
        CHECK(flagged, "'kva yi' keeps tibetan-review flag");
        CHECK(e[0].tibetan.find("⟨") != std::string::npos,
              "failed syllable renders as ⟨wylie⟩ marker, never guessed");
    }

    // English reverse index, tier-ordered
    auto rev = spine.reverseIndex("merit");
    CHECK(!rev.empty() && rev[0].wylie == "bsod nams" && rev[0].tier == "curated",
          "reverseIndex('merit') top hit bsod nams [curated]");

    // corpus FTS: OR + NEAR + course filter
    CHECK(!spine.corpusSearch("stong OR bden", "C02", 5).empty(),
          "corpus 'stong OR bden' in C02");
    auto near = spine.corpusSearch("NEAR(byang sems, 5)", "", 5);
    CHECK(!near.empty(), "corpus NEAR(byang sems, 5)");

    std::printf("%s (%d failure%s)\n", failures ? "SMOKE FAILED" : "SMOKE OK",
                failures, failures == 1 ? "" : "s");
    return failures ? 1 : 0;
}
