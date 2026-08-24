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

    fs::remove_all(root, ec);
    std::printf("%s (%d failure(s))\n", fails ? "FAILED" : "ok", fails);
    return fails ? 1 : 0;
}
