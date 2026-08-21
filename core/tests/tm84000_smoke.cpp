// tm84000_smoke — the 84000 TM comparanda layer on the REAL shipped
// TSV (data/84000/tm_84000.tsv.gz, 400,745 segments / 388 texts at
// ingest, 2026-08-20). Pins: the build from gz, the source-size cache
// (second ensureBuilt must not rebuild), phrase search on both the
// Tibetan and English columns, and the provenance row.
//
// argv[1] = the shipped tm_84000.tsv.gz; argv[2] = a scratch dir for
// the generated db.
#include <sys/stat.h>

#include <chrono>
#include <cstdio>
#include <string>

#include "allcore/tm84000.h"

static int failures = 0;
#define CHECK(cond, msg)                                        \
    do {                                                        \
        if (cond) std::printf("  [PASS] %s\n", msg);            \
        else { std::printf("  [FAIL] %s\n", msg); ++failures; } \
    } while (0)

int main(int argc, char** argv) {
    setvbuf(stdout, nullptr, _IONBF, 0);
    if (argc < 3) {
        std::printf("usage: tm84000_smoke <tm_84000.tsv.gz> <scratch>\n");
        return 2;
    }
    const std::string tsv = argv[1];
    const std::string db = std::string(argv[2]) + "/tm_84000_smoke.db";
    std::remove(db.c_str());

    const long long rows = allcore::Tm84000::ensureBuilt(tsv, db);
    std::printf("  built: %lld rows\n", rows);
    // measured at ingest 2026-08-20: 400,745 rows; floor just under
    CHECK(rows >= 390000, "the shipped TSV builds >= 390k segments");

    // the cache: a second call must be a no-op (source size matches)
    const auto t0 = std::chrono::steady_clock::now();
    const long long again = allcore::Tm84000::ensureBuilt(tsv, db);
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::steady_clock::now() - t0)
                        .count();
    CHECK(again == rows, "second ensureBuilt returns the same count");
    CHECK(ms < 3000, "…and is a cache hit, not a rebuild");

    allcore::Tm84000 tm(db);
    CHECK(tm.rowCount() == rows, "rowCount matches the build");

    // Tibetan phrase search: sems can (all beings) — everywhere in the
    // canon; the FTS5 unicode61 tokenizer splits on tsheg
    auto hits = tm.search("\"སེམས ཅན\"", 50);
    std::printf("  sems-can hits: %zu\n", hits.size());
    CHECK(hits.size() == 50, "Tibetan phrase 'sems can' fills the limit");
    bool toh_ok = !hits.empty() && !hits[0].toh.empty() &&
                  hits[0].toh.rfind("toh", 0) == 0;
    CHECK(toh_ok, "hits carry their Tohoku attribution");
    bool bo_en = true;
    for (const auto& h : hits)
        if (h.tibetan.empty() || h.english.empty()) bo_en = false;
    CHECK(bo_en, "every hit is a real aligned pair (both sides present)");

    // English side: the TM's own published rendering is searchable
    auto eng = tm.search("english: \"perfection of wisdom\"", 20);
    CHECK(!eng.empty(), "English column search reaches the TM");

    // no hallucinated matches: a nonsense phrase returns nothing
    auto none = tm.search("\"zzqx yyqk\"", 5);
    CHECK(none.empty(), "a nonsense phrase matches nothing");

    std::printf("%s: %d failure(s)\n", failures ? "FAIL" : "OK", failures);
    return failures ? 1 : 0;
}
