// gofer_bound_smoke — SQA PERF-1 (2026-08-23, critical, ship-blocker).
//
// goferSearchFiles is the UNINDEXED library search: the default path on
// a fresh install, because the index is built only by a button and the
// press ships an empty library/. Two defects, both measured:
//
//   1. Node::NEAR was a bare nested cross product with no cap of any
//      kind, and `limit` was first consulted after eval() returned.
//      Measured on the real library: 115,589.7 ms and a 23.5 GB peak
//      RSS for three ordinary words. On the 16 GB laptops this ships
//      to, that is a swap storm or a jetsam kill, not a stall.
//
//   2. The file walk broke at 4,000 files while the library holds
//      8,988. 4,988 files were silently excluded from every unindexed
//      search, so a rare phrase in them returned "no matches" - a
//      performance cap converted into a claim about the world, which
//      is house rule 3 in its exact terms.
//
// Why this drill is cheap: the NEAR product is QUADRATIC, so a single
// file of 500 lines carrying both terms yields 500 x 500 = 250,000
// windows - past the 200,000 cap - without a large fixture. The twin
// LibraryIndex::search was capped the same way (libindex.h:76).
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <string>

#include "allcore/gofer.h"

static int fails = 0;

static void check(bool ok, const char* what) {
    std::printf("  [%s] %s\n", ok ? "PASS" : "FAIL", what);
    if (!ok) ++fails;
}

int main() {
    namespace fs = std::filesystem;
    std::error_code ec;
    const fs::path root = fs::temp_directory_path() / "all_gofer_bound";
    fs::remove_all(root, ec);
    fs::create_directories(root, ec);

    std::printf("gofer_bound_smoke (PERF-1)\n");

    // ---- 1. the NEAR cross product is bounded ---------------------
    {
        std::ofstream f(root / "quadratic.txt");
        for (int i = 0; i < 500; ++i) f << "AAA BBB\n";
    }
    {
        allcore::GoferScan scan;
        const auto hits = allcore::goferSearchFiles(
            root.string(), "\"AAA\" NEAR/1000000 \"BBB\"", 60, &scan);
        check(scan.cut,
              "NEAR: a 250,000-window cross product reports that it "
              "was cut, rather than materialising it (PERF-1)");
        check((int)hits.size() <= 60,
              "NEAR: the caller's limit is honoured");
    }

    // ---- 2. the file cap is disclosed, not silent -----------------
    for (int i = 0; i < 5; ++i) {
        std::ofstream f(root / ("pad" + std::to_string(i) + ".txt"));
        f << "CCC\n";
    }
    {
        allcore::GoferScan scan;
        scan.file_cap = 3;          // stand-in for the shipped 4,000
        const auto hits =
            allcore::goferSearchFiles(root.string(), "\"CCC\"", 60,
                                      &scan);
        (void)hits;
        check(scan.files_skipped > 0,
              "file cap: files left unread are COUNTED, so the pane "
              "can say so instead of implying 'no matches' (PERF-1, "
              "house rule 3)");
        check(scan.files_scanned == 3,
              "file cap: the number actually read is reported");
    }

    // ---- 2b. PERF-R2: TERM and OR honor the same ceiling ----------
    // The re-measurement caught the two evaluators re-diverged: the
    // file walk capped only NEAR, so a common syllable materialised
    // every matching line. Both nodes now cap and DISCLOSE.
    // ISOLATED dir with ONE file: with a second file present the
    // outer per-file cap re-arms and masks the inner strand - the
    // first mutation run proved it (survived), so the fixture
    // isolates what the pin claims to police.
    const fs::path r2 = fs::temp_directory_path() / "all_gofer_r2";
    fs::remove_all(r2, ec);
    fs::create_directories(r2, ec);
    {
        std::ofstream f(r2 / "common.txt");
        for (int i = 0; i < 210000; ++i) f << "CCC DDD\n";
    }
    {
        allcore::GoferScan scan;
        auto hits = allcore::goferSearchFiles(r2.string(),
                                              "\"CCC\"", 60, &scan);
        check(scan.cut,
              "a TERM matching more lines than the ceiling is CUT "
              "and says so (PERF-R2)");
    }
    // The OR-node resize is DEFENSE-IN-DEPTH: with TERM capped, each
    // side arrives at <= cap already flagged, so the resize's own
    // flag is unobservable through scan.cut (shared state) - the
    // sweep records that honestly as an expected survivor rather
    // than pretending this pin reaches it.
    {
        allcore::GoferScan scan;
        auto hits = allcore::goferSearchFiles(
            r2.string(), "\"CCC\" OR \"DDD\"", 60, &scan);
        check(scan.cut,
              "an OR of over-ceiling terms is CUT and says so "
              "(PERF-R2)");
    }
    fs::remove_all(r2, ec);

    // ---- 3. an ordinary search still reports nothing cut ----------
    // A drill whose flags are always set proves nothing, so pin the
    // negative case too.
    {
        allcore::GoferScan scan;
        const auto hits =
            allcore::goferSearchFiles(root.string(), "\"CCC\"", 60,
                                      &scan);
        check(!hits.empty(), "control: an ordinary search still finds "
                             "its matches");
        check(!scan.cut,
              "control: an ordinary search reports nothing cut - the "
              "flag means something");
        check(scan.files_skipped == 0,
              "control: nothing skipped when under the cap");
        // Asserted HERE, where the true count (6) differs from the
        // capped section's (3). Pinning it only there let a mutant
        // that hardcoded 3 survive - caught by mutation testing, and
        // the reason this second assertion exists.
        check(scan.files_scanned == 6,
              "control: the scanned count is the real number, not a "
              "constant that happens to match the cap");
    }

    {   // PERF-R1 (SQA re-measurement 2026-08-26): the unindexed
        // walk is the DEFAULT search on a fresh install and ran with
        // no pulse - the pane's Stop button was dead for the whole
        // scan. The pulse fires as the walk runs; returning false
        // stops it, the remainder is COUNTED as skipped (rule 3),
        // and stats say stopped out loud.
        allcore::GoferScan scan;
        int pulses = 0;
        scan.pulse = [&pulses](int) { return ++pulses < 2; };
        auto hits = allcore::goferSearchFiles(
            root.string(), "\"bden\"", 60, &scan);
        check(pulses >= 2, "the pulse fires during the walk (PERF-R1)");
        check(scan.stopped,
              "a false pulse stops the walk and says stopped (PERF-R1)");
        check(scan.files_scanned < 6 && scan.files_skipped > 0 &&
                  scan.files_scanned + scan.files_skipped == 6,
              "the stopped walk counts every unread file as skipped - "
              "scanned+skipped covers the library (PERF-R1)");
    }

    fs::remove_all(root, ec);
    std::printf("%s (%d failure(s))\n", fails ? "FAILED" : "ok", fails);
    return fails ? 1 : 0;
}
