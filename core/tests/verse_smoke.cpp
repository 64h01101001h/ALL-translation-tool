// verse_smoke — the syllabic meter analyzer.
#include <cstdio>

#include "allcore/verse.h"

static int failures = 0;
#define CHECK(cond, msg)                                        \
    do {                                                        \
        if (cond) std::printf("  [PASS] %s\n", msg);            \
        else { std::printf("  [FAIL] %s\n", msg); ++failures; } \
    } while (0)

int main() {
    setvbuf(stdout, nullptr, _IONBF, 0);

    // a clean 7-syllable quatrain (the ubiquitous dedication meter)
    auto v = allcore::analyzeVerse(
        "BSOD NAMS 'DI YIS THAMS CAD KYIS,\n"
        "GZIGS PA NYID NI THOB GYUR NAS,\n"
        "SKYON GYIS PHAM PA'I DGRA RNAMS NI,\n"
        "'KHOR BA'I 'JIGS LAS SGROL BAR SHOG,");
    CHECK(v.lines.size() == 4, "four lines detected");
    CHECK(v.is_verse && v.meter == 7, "7-syllable meter recognized");
    CHECK(v.irregular_count == 0, "clean quatrain has no irregular lines");

    // one broken line is flagged
    auto b = allcore::analyzeVerse(
        "KA BA DANG NI BUM PA DANG,\n"
        "SEMS CAN THAMS CAD BDE BA DANG,\n"
        "KA BA DANG NI BUM PA YANG DAG DANG,\n"
        "SEMS CAN THAMS CAD BDE BA DANG,");
    CHECK(b.is_verse && b.meter == 7, "dominant meter survives one break");
    CHECK(b.irregular_count == 1 && b.lines[2].irregular,
          "the 9-syllable line is flagged irregular");

    // prose does not get called verse
    auto p = allcore::analyzeVerse(
        "SNGA DRO'I KA BA PHYI DRO MED PA DUS MA 'DRES PA'I DON YIN GYI, "
        "KA BA SNGA DRO YOD NA, PHYI DRO MED DGOS PA, DUS CHA MA 'DRES PA'I "
        "DON MA YIN LA,");
    CHECK(!p.is_verse, "uneven prose clauses are not verse");

    // too short to judge
    auto s = allcore::analyzeVerse("KA BA DANG NI BUM PA DANG,\nKA BA DANG NI BUM PA DANG,");
    CHECK(!s.is_verse, "two lines are too few to call verse");

    std::printf("%s (%d failures)\n",
                failures ? "VERSE SMOKE FAILED" : "VERSE SMOKE OK", failures);
    return failures ? 1 : 0;
}
