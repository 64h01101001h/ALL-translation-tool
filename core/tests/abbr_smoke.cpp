// abbr_smoke — the TibSchol bskungs-yig table (CC0). Fixtures from the
// table's own rows: khyadr = khyad par, ku+ob = kun rdzob (both lookup
// directions), unknown forms empty, and the red-wave companionship: a
// known abbreviation is (by design) not a legal ordinary syllable.
#include <cstdio>
#include <string>

#include "allcore/abbr.h"
#include "allcore/spellcheck.h"

static int failures = 0;
#define CHECK(cond, msg)                                        \
    do {                                                        \
        if (cond) std::printf("  [PASS] %s\n", msg);            \
        else { std::printf("  [FAIL] %s\n", msg); ++failures; } \
    } while (0)

int main(int argc, char** argv) {
    if (argc < 3) {
        std::fprintf(stderr,
                     "usage: abbr_smoke <tibschol_abbr.csv> <spellcheck-dir>\n");
        return 2;
    }
    allcore::AbbrTable abbr;
    CHECK(abbr.load(argv[1]), "table loads");
    std::printf("  %zu abbreviations\n", abbr.size());
    CHECK(abbr.size() >= 130, "130+ forms banked");

    auto k = abbr.byWylie("khyadr");
    CHECK(!k.empty() && k.front()->expWylie == "khyad par",
          "khyadr -> khyad par (EWTS lookup)");
    CHECK(!k.empty() && k.front()->expUnicode == "ཁྱད་པར",
          "expansion unicode carried");
    auto u = abbr.byUnicode("ཁྱདར");
    CHECK(!u.empty() && u.front()->expWylie == "khyad par",
          "ཁྱདར -> khyad par (unicode lookup)");
    CHECK(!abbr.byWylie("ku+ob").empty(), "ku+ob (stacked vowels) found");
    CHECK(abbr.byWylie("khyad par").empty() && abbr.byUnicode("ZZZ").empty(),
          "expansions and garbage are not abbreviations");

    // the rKTs layer (Dr. Laine's export, CC BY 4.0 — grant banked
    // in docs/licenses/RKTS_ABBREVIATIONS.md) stacks onto the same
    // table when its CSV is supplied as argv[3]; fixtures from the
    // file's own rows, with per-row source attribution carried
    if (argc >= 4) {
        const size_t before = abbr.size();
        CHECK(abbr.load(argv[3]), "rKTs table loads on top");
        std::printf("  %zu after rKTs\n", abbr.size());
        CHECK(abbr.size() >= before + 6000, "6,000+ rKTs forms join");
        auto r = abbr.byWylie("khaMsuM seMn");
        CHECK(!r.empty() &&
                  r.front()->expWylie == "khams gsum sems can",
              "khaMsuM seMn -> khams gsum sems can (rKTs row)");
        CHECK(!r.empty() &&
                  r.front()->src.find("rKTs") != std::string::npos,
              "rKTs row carries its attribution");
        // re-fetch: the second load() reallocated the entry
        // vector, so pointers taken before it are dangling (the
        // ASan battery caught exactly this)
        k = abbr.byWylie("khyadr");
        CHECK(!k.empty() && k.front()->src.empty(),
              "TibSchol rows keep the default attribution");
    }

    // the red wave and this layer are companions: the squeezed form is
    // not a legal ordinary syllable, which is exactly why the card must
    // explain it as an abbreviation rather than a typo
    try {
        allcore::SyllableChecker chk(argv[2]);
        CHECK(!chk.legalWylie("khyadr"),
              "khyadr fails ordinary syllable legality (red wave fires)");
    } catch (const std::exception&) {
        std::printf("  (spellcheck data unavailable — companion check skipped)\n");
    }

    std::printf("abbr_smoke: %s (%d failures)\n",
                failures ? "FAIL" : "ALL PASS", failures);
    return failures ? 1 : 0;
}
