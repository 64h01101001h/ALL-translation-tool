// libindex_smoke — the prebuilt library full-text index: build, search,
// incremental update, removal.
#include <cstdio>
#include <filesystem>
#include <fstream>

#include "allcore/libindex.h"
#include <sqlite3.h>
#include "allcore/searchnorm.h"

static int failures = 0;
#define CHECK(cond, msg)                                        \
    do {                                                        \
        if (cond) std::printf("  [PASS] %s\n", msg);            \
        else { std::printf("  [FAIL] %s\n", msg); ++failures; } \
    } while (0)

namespace fs = std::filesystem;

static void write(const fs::path& p, const char* text) {
    std::ofstream f(p);
    f << text;
}

int main() {
    setvbuf(stdout, nullptr, _IONBF, 0);
    const fs::path root = fs::temp_directory_path() / "libindex_smoke_root";
    fs::remove_all(root);
    fs::create_directories(root / "kangyur");
    write(root / "kangyur" / "KL0001MA.ACT",
          "BDEN PA BZHI NI\nSDUG BSNGAL DANG\nKUN 'BYUNG DANG\n'GOG PA DANG "
          "LAM MO\nBDEN PA'I DON LA\n");
    write(root / "notes.txt",
          "line one\nBDEN PA appears here\nfar away\nfar away\nSDUG BSNGAL "
          "here\n");
    write(root / "skip.pdf", "not indexed");

    const std::string dbp = (root / ".index.db").string();
    allcore::LibraryIndex ix(dbp);
    auto st = ix.update(root.string());
    CHECK(st.added == 2 && st.removed == 0,
          "initial update indexes the two eligible files (.pdf skipped)");
    CHECK(ix.fileCount() == 2 && ix.lineCount() == 10,
          "file and line counts recorded");

    auto hits = ix.search("\"bden pa\"");
    CHECK(hits.size() == 3, "phrase found in both files (3 windows — "
                            "incl. the affixed line's own)");
    // affix tolerance: the BDEN PA'I line matches a bden pa search via
    // the normalized column (same authority as the corpus spine)
    bool affixLine = false;
    for (auto& h : hits)
        for (auto& l : h.lines)
            affixLine |= (l.find("PA'I DON") != std::string::npos);
    CHECK(affixLine, "BDEN PA'I line found by the bden pa search");
    bool actHit = false;
    for (auto& h : hits) actHit |= h.file.find("KL0001MA.ACT") != std::string::npos;
    CHECK(actHit, ".ACT collection files are searchable");

    auto near1 = ix.search("\"BDEN PA\" NEAR/3 \"SDUG BSNGAL\"");
    auto near2 = ix.search("\"BDEN PA\" NEAR/1 \"SDUG BSNGAL\"");
    CHECK(near1.size() == 3 && near2.size() == 1,
          "NEAR windows honor the line bound per file (affixed line "
          "joins the /3 window)");
    CHECK(!near1[0].lines.empty(), "hit windows are hydrated with line text");

    auto orHits = ix.search("\"KUN 'BYUNG\" OR \"far away\"");
    CHECK(orHits.size() == 3, "OR unions across files");

    // incremental: unchanged files skipped; edits re-index; deletions purge
    auto st2 = ix.update(root.string());
    CHECK(st2.unchanged == 2 && st2.added == 0 && st2.updated == 0,
          "second update touches nothing");
    // fold-generation stamp: configuring the lemma fold after an
    // index was built forces one full reindex, so norms built
    // without the fold heal — and a past-form line becomes
    // reachable from its present stem
    const fs::path verbs = fs::temp_directory_path() / "libindex_verbs.tsv";
    write(verbs, "bkag\t'gog\n");
    allcore::configureLemmaFold(verbs.string());
    write(root / "past.txt", "BKAG PA YIN\n");
    auto stf = ix.update(root.string());
    CHECK(stf.added == 1 && stf.updated == 2 && stf.unchanged == 0,
          "lemma-fold generation change reindexes every file once");
    bool pastHit = false;
    for (auto& h : ix.search("\"'gog\""))
        for (auto& l : h.lines)
            pastHit |= l.find("BKAG") != std::string::npos;
    CHECK(pastHit, "past form BKAG found by its present stem 'gog");
    auto stf2 = ix.update(root.string());
    CHECK(stf2.unchanged == 3 && stf2.updated == 0,
          "stamp settles: next update touches nothing");
    fs::remove(verbs);

    fs::last_write_time(root / "notes.txt",
                        fs::file_time_type::clock::now());
    write(root / "notes.txt", "completely new content\n");
    auto st3 = ix.update(root.string());
    CHECK(st3.updated == 1, "edited file re-indexes");
    CHECK(ix.search("\"far away\"").empty(),
          "stale lines are purged from the FTS index");
    fs::remove(root / "kangyur" / "KL0001MA.ACT");
    auto st4 = ix.update(root.string());
    CHECK(st4.removed == 1 && ix.fileCount() == 2,
          "deleted file leaves the index");
    CHECK(ix.search("\"bden pa\"").empty(), "its lines are gone too");

    // ---- SQA PERF-1: the scan is BOUNDED, and says when it cut ----
    // Every node used to materialise its full match set before `limit`
    // was consulted, and the pane's "AND (same file)" compiles to
    // NEAR/1000000 — the entire same-file cross product. Measured on
    // the real 2.36 GB index: three ordinary Tibetan words ANDed took
    // 5m46s and 18.0 GB of RSS on the GUI thread. After bounding:
    // 3.3 s and 40.7 MB.
    //
    // This test builds a corpus far larger than a deliberately tiny
    // cap so the cross product WOULD explode if the bound were
    // removed, and asserts two things: the bound holds, and it is
    // reported. A cap that is not disclosed is house-rule-3 breakage,
    // and silently returning part of a library reads as all of it.
    {
        const fs::path big = fs::temp_directory_path() / "sqa_perf1";
        std::error_code ec;
        fs::remove_all(big, ec);
        fs::create_directories(big / "texts", ec);
        // 60 files x 400 lines, every line carrying both terms: a
        // same-file NEAR pair count in the millions if unbounded.
        for (int f = 0; f < 60; ++f) {
            std::ofstream o2(big / "texts" /
                             ("T" + std::to_string(f) + ".ACT"));
            for (int l = 0; l < 400; ++l)
                o2 << "CHOS SANGS RGYAS line " << l << "\n";
        }
        allcore::LibraryIndex bx((big / ".index.db").string());
        bx.update((big / "texts").string());

        bool cut = false;
        const auto hits =
            bx.search("\"CHOS\" NEAR/1000000 \"SANGS RGYAS\"", 60, &cut);
        CHECK(hits.size() <= 60,
              "PERF-1: the hit list honours its limit");
        CHECK(cut,
              "PERF-1: a scan that reached the window ceiling REPORTS "
              "it, so partial results are never read as a whole "
              "library");
        bool small = false;
        const auto few = bx.search("\"line 7\"", 60, &small);
        CHECK(!small,
              "PERF-1: a query that fits under the ceiling reports NO "
              "truncation (the flag means something)");
        (void)few;
        fs::remove_all(big, ec);
    }

    fs::remove_all(root);
    {
        // PERF-3: limit must reach the scan. Own root and own index,
        // so the main fixture's incremental-update tests stay honest.
        const fs::path proot =
            fs::temp_directory_path() / "libindex_perf3_root";
        fs::remove_all(proot);
        fs::create_directories(proot / "kangyur");
        std::string many;
        for (int i = 0; i < 3000; ++i)
            many += "ZAB MO LINE " + std::to_string(i) + "\n";
        write(proot / "kangyur" / "KL9999ZZ.ACT", many.c_str());
        allcore::LibraryIndex pix((proot / ".index.db").string());
        pix.update(proot.string());
        allcore::LibraryIndex::SearchStats st5;
        auto few = pix.search("ZAB", 5, nullptr, &st5);
        CHECK(few.size() == 5, "perf: limit=5 returns 5 hits");
        // >= guards the vacuous pass: an unpopulated counter reads 0,
        // and 0 <= 6 would have blessed the very defect under test.
        CHECK(st5.term_rows_visited >= 5 && st5.term_rows_visited <= 6,
              "perf: a bare-term search visits O(limit) rows, not every match");
        bool cut = false;
        auto all = pix.search("ZAB", 60, &cut);
        CHECK(all.size() == 60 && cut,
              "perf: truncation reported when the limit cuts real matches");
        // PERF-4: the pump fires during a scan, and returning false
        // aborts it mid-flight instead of after the fact.
        int pumped = 0;
        allcore::LibraryIndex::SearchStats stp;
        auto pumpedHits = pix.search("ZAB", 2000, nullptr, &stp,
                                     [&] { ++pumped; return true; });
        CHECK(pumped > 0, "perf4: the pump is actually called during a scan");
        CHECK(!stp.aborted, "perf4: a true-returning pump never aborts");
        allcore::LibraryIndex::SearchStats sta;
        auto few2 = pix.search("ZAB", 2000, nullptr, &sta,
                               [] { return false; });
        CHECK(sta.aborted, "perf4: a false-returning pump aborts the scan");
        CHECK(few2.size() < 2000,
              "perf4: the aborted scan returns partial, not full, results");
        // PERF-6: the index runs in WAL mode - readers do not block
        // the writer, and a crash mid-update rolls back cleanly.
        {
            sqlite3* raw = nullptr;
            CHECK(sqlite3_open((proot / ".index.db").string().c_str(),
                               &raw) == SQLITE_OK, "perf6: db reopens");
            sqlite3_stmt* st = nullptr;
            sqlite3_prepare_v2(raw, "PRAGMA journal_mode", -1, &st, nullptr);
            std::string mode;
            if (sqlite3_step(st) == SQLITE_ROW)
                mode = (const char*)sqlite3_column_text(st, 0);
            sqlite3_finalize(st);
            sqlite3_close(raw);
            CHECK(mode == "wal", "perf6: the index journal is WAL");
        }
        fs::remove_all(proot);
    }

    std::printf("%s (%d failures)\n",
                failures ? "LIBINDEX SMOKE FAILED" : "LIBINDEX SMOKE OK",
                failures);
    return failures ? 1 : 0;
}
