// libindex_cancel_smoke — SQA PERF-2: "Update search index" ran for
// 190,492 ms on the GUI thread with no progress and no way out. The
// fix is a per-file progress callback that can say stop. This suite
// pins the three properties that make stopping SAFE, because a cancel
// that corrupts the index would be worse than the freeze:
//
//   1. the callback is actually called, with a real total and a
//      monotonically rising done;
//   2. a stop commits the files finished so far and reports itself
//      (UpdateStats::canceled) — no "up to date" that was not earned;
//   3. a stop lands BETWEEN files, never inside one, so no file is
//      banked half-indexed under its real mtime+size (which every
//      later update would skip as "unchanged" — a silent hole);
//   4. cancel + resume ends up byte-for-byte equivalent to one
//      uninterrupted update of the same tree.
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "allcore/libindex.h"
#include "allcore/searchnorm.h"

static int failures = 0;
#define CHECK(cond, msg)                                        \
    do {                                                        \
        if (cond) std::printf("  [PASS] %s\n", msg);            \
        else { std::printf("  [FAIL] %s\n", msg); ++failures; } \
    } while (0)

namespace fs = std::filesystem;

static const int kFiles = 300;    // > 256, so the walk callback fires
static const int kLines = 5;

static void makeTree(const fs::path& root) {
    fs::remove_all(root);
    fs::create_directories(root);
    for (int i = 0; i < kFiles; ++i) {
        char name[64];
        std::snprintf(name, sizeof name, "f%03d.txt", i);
        std::ofstream f(root / name);
        for (int l = 0; l < kLines; ++l)
            f << "BDEN PA BZHI NI file " << i << " line " << l << "\n";
    }
}

int main() {
    setvbuf(stdout, nullptr, _IONBF, 0);
    const fs::path root =
        fs::temp_directory_path() / "libindex_cancel_smoke_root";
    makeTree(root);

    // ---- 1. a full, uninterrupted run: the reference ----------------
    long long refFiles = 0, refLines = 0;
    size_t refHits = 0;
    {
        const std::string dbp = (root / ".full.db").string();
        fs::remove(dbp);
        allcore::LibraryIndex ix(dbp);
        auto st = ix.update(root.string());
        CHECK(!st.canceled && st.added == kFiles,
              "an uninterrupted update reports canceled=false");
        refFiles = ix.fileCount();
        refLines = ix.lineCount();
        refHits = ix.search("\"bden pa\"", 500).size();
        CHECK(refFiles == kFiles && refLines == (long long)kFiles * kLines,
              "the reference index holds every file and every line");
    }

    // ---- 2. stop after 12 files -------------------------------------
    const std::string dbp = (root / ".index.db").string();
    fs::remove(dbp);
    const int kStopAfter = 12;
    int calls = 0, walkCalls = 0, lastDone = -1, seenTotal = -1;
    bool monotonic = true, totalStable = true, doneNeverExceedsTotal = true;
    {
        allcore::LibraryIndex ix(dbp);
        auto st = ix.update(
            root.string(),
            [&](int done, int total, const std::string& path) {
                if (total == 0) {   // the directory-walk phase
                    ++walkCalls;
                    return true;
                }
                ++calls;
                if (done < lastDone) monotonic = false;
                lastDone = done;
                if (seenTotal < 0) seenTotal = total;
                else if (seenTotal != total) totalStable = false;
                if (done > total) doneNeverExceedsTotal = false;
                (void)path;
                return done < kStopAfter;
            });

        CHECK(walkCalls > 0,
              "the directory walk reports progress too (it is "
              "thousands of stat() calls on a real collection)");
        CHECK(calls == kStopAfter + 1 && monotonic && totalStable &&
                  doneNeverExceedsTotal,
              "the per-file callback runs once per file with a real "
              "total and a rising done");
        CHECK(seenTotal == kFiles,
              "the total handed to the callback is the real file "
              "count, not a cap or a guess");
        CHECK(st.canceled,
              "a stopped update SAYS it stopped (never a silent "
              "\"up to date\")");
        CHECK(st.added == kStopAfter,
              "a stopped update counts only the files it finished");
        CHECK(ix.fileCount() == kStopAfter,
              "the files finished before the stop are COMMITTED — a "
              "partial index is valid and resumable");
        CHECK(ix.lineCount() == (long long)kStopAfter * kLines,
              "no file was banked half-indexed: every committed file "
              "carries all of its lines");
    }

    // ---- 3. resume: the same tree, no callback ----------------------
    {
        allcore::LibraryIndex ix(dbp);
        auto st = ix.update(root.string());
        CHECK(!st.canceled && st.added == kFiles - kStopAfter &&
                  st.unchanged == kStopAfter,
              "the resume indexes exactly the remainder and leaves "
              "the committed files alone");
        CHECK(ix.fileCount() == refFiles && ix.lineCount() == refLines,
              "cancel + resume reaches the same index as one "
              "uninterrupted update");
        CHECK(ix.search("\"bden pa\"", 500).size() == refHits,
              "and answers the same query with the same hits");
    }

    // ---- 4. stopping during the WALK writes nothing -----------------
    {
        const std::string dbw = (root / ".walkstop.db").string();
        fs::remove(dbw);
        allcore::LibraryIndex ix(dbw);
        auto st = ix.update(root.string(),
                            [](int, int total, const std::string&) {
                                return total != 0;   // stop on walk
                            });
        CHECK(st.canceled && st.added == 0 && ix.fileCount() == 0,
              "a stop during the directory walk leaves an empty, "
              "consistent index (no transaction was open)");
    }

    // ---- 5. a stopped REFOLD must not stamp itself as finished ------
    // The fold generation stamp says "every row here was normalised
    // under this fold". If a cancelled refold stamped it anyway, the
    // files it never reached would keep their old norms for ever —
    // and (path, mtime, size) is unchanged, so no later update would
    // ever revisit them. Silent, permanent, and invisible in the UI.
    {
        const std::string dbf = (root / ".refold.db").string();
        fs::remove(dbf);
        allcore::LibraryIndex ix(dbf);
        auto full = ix.update(root.string());
        CHECK(full.added == kFiles, "refold drill: baseline index built");

        const fs::path verbs =
            fs::temp_directory_path() / "libindex_cancel_verbs.tsv";
        { std::ofstream v(verbs); v << "bkag\t'gog\n"; }
        allcore::configureLemmaFold(verbs.string());

        int seen = 0;
        auto part = ix.update(root.string(),
                              [&](int, int total, const std::string&) {
                                  if (total == 0) return true;
                                  return ++seen <= kStopAfter;
                              });
        CHECK(part.canceled && part.updated == kStopAfter,
              "refold drill: the stop lands mid-refold");
        auto after = ix.update(root.string());
        CHECK(after.updated == kFiles && after.unchanged == 0,
              "an interrupted refold does NOT stamp itself finished: "
              "the next update re-normalises every file, so no row is "
              "left with stale norms nothing will ever revisit");
        fs::remove(verbs);
    }

    fs::remove_all(root);
    std::printf("%s (%d failures)\n",
                failures ? "LIBINDEX CANCEL SMOKE FAILED"
                         : "LIBINDEX CANCEL SMOKE OK",
                failures);
    return failures ? 1 : 0;
}
