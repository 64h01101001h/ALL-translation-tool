// spellcheck_smoke — syllable-legality layer: known cases + empirical
// calibration against every syllable of the 26,318 source-attested entries.
#include <sqlite3.h>

#include <cstdio>
#include <string>
#include <vector>

#include "allcore/spellcheck.h"

static int failures = 0;
#define CHECK(cond, msg)                                        \
    do {                                                        \
        if (cond) std::printf("  [PASS] %s\n", msg);            \
        else { std::printf("  [FAIL] %s\n", msg); ++failures; } \
    } while (0)

int main(int argc, char** argv) {
    if (argc < 3) {
        std::fprintf(stderr, "usage: spellcheck_smoke <spine.db> <spellcheck-dir>\n");
        return 2;
    }
    allcore::SyllableChecker chk(argv[2]);
    std::printf("  loaded %zu syllable bases\n", chk.stackCount());
    CHECK(chk.stackCount() > 300, "syllable data loaded");

    // legal syllables (wylie side, through the ported engines)
    for (const char* w : {"bsod", "nams", "dkyil", "bden", "pa", "'phags",
                          "bzhi", "sems", "dpa'", "sgrib"})
        CHECK(chk.legalWylie(w), (std::string("legal: ") + w).c_str());

    // the documented v0.27.1 wylie-typo class: dkhyil/skhyil must be flagged
    CHECK(!chk.legalWylie("dkhyil"), "typo flagged: dkhyil");
    CHECK(!chk.legalWylie("skhyil"), "typo flagged: skhyil");
    CHECK(!chk.legalWylie("qqq"), "garbage flagged");

    // empirical calibration: every syllable of the source-attested entries
    sqlite3* db = nullptr;
    sqlite3_open_v2(argv[1], &db, SQLITE_OPEN_READONLY, nullptr);
    sqlite3_stmt* s = nullptr;
    sqlite3_prepare_v2(db,
                       "SELECT tibetan FROM entries WHERE tibetan_source IS NULL "
                       "AND tibetan IS NOT NULL",
                       -1, &s, nullptr);
    long total = 0, legal = 0;
    std::vector<std::string> flagged;
    while (sqlite3_step(s) == SQLITE_ROW) {
        const unsigned char* t = sqlite3_column_text(s, 0);
        if (!t) continue;
        const std::string tib = reinterpret_cast<const char*>(t);
        std::string syl;
        auto flush = [&] {
            if (syl.empty()) return;
            ++total;
            if (chk.legalUnicode(syl)) ++legal;
            else if (flagged.size() < 6) flagged.push_back(syl);
            syl.clear();
        };
        for (size_t i = 0; i < tib.size();) {
            if (tib.compare(i, 3, "་") == 0 || tib.compare(i, 3, "།") == 0 ||
                tib.compare(i, 3, "༔") == 0 || tib[i] == ' ') {
                flush();
                i += tib[i] == ' ' ? 1 : 3;
            } else {
                syl += tib[i++];
            }
        }
        flush();
    }
    sqlite3_finalize(s);
    sqlite3_close(db);
    const double rate = total ? 100.0 * legal / total : 0;
    std::printf("  GT calibration: %ld/%ld syllables legal (%.2f%%)\n", legal,
                total, rate);
    for (auto& f : flagged) std::printf("     flagged (sample): %s\n", f.c_str());
    // real text contains Sanskrit transliteration + the documented typo classes,
    // so the legal rate is high but deliberately not 100%
    CHECK(total > 60000, "calibration covers the GT corpus of syllables");
    CHECK(rate >= 95.0, "NB-class reading holds up on real text (>=95% legal)");

    std::printf("%s (%d failures)\n",
                failures ? "SPELLCHECK SMOKE FAILED" : "SPELLCHECK SMOKE OK",
                failures);
    return failures ? 1 : 0;
}
