// allcore/libindex.h — the prebuilt full-text index over the user's library
// (installed ACIP collections + personal materials), so Gofer queries over an
// entire Kangyur answer instantly instead of re-reading every file.
//
// One SQLite file (FTS5) lives with the library; update() is incremental by
// (path, mtime, size). search() speaks the same Gofer grammar as the live
// scan and returns the same hit shape. NOTE the one documented semantic
// difference: the index matches whole FTS tokens/phrases (like corpus
// search), while the live scan matches raw substrings.
#pragma once

#include <functional>
#include <string>
#include <vector>

#include "allcore/gofer.h"

struct sqlite3;

namespace allcore {

class LibraryIndex {
public:
    // Opens (or creates) the index database.
    explicit LibraryIndex(const std::string& db_path);
    ~LibraryIndex();
    LibraryIndex(const LibraryIndex&) = delete;
    LibraryIndex& operator=(const LibraryIndex&) = delete;

    struct UpdateStats {
        int added = 0, updated = 0, removed = 0, unchanged = 0;
        long long lines = 0;   // total lines indexed after the update
        // SQA PERF-2: true when the caller's progress callback asked
        // to stop. Everything finished BEFORE the stop is committed —
        // the index is incremental by (path, mtime, size), so a
        // partial index is valid and the next update resumes from it.
        // The caller must say "stopped, N of M done", never "up to
        // date": a cancelled pass did not index the rest.
        bool canceled = false;
        // SQA PERF-6: true when a change of word-fold generation
        // forced a FULL reindex. It matters to the caller because the
        // fold stamp is deliberately withheld on a cancelled pass
        // (otherwise an interrupted refold would never heal), so a
        // cancelled refold banks NOTHING - the next run redoes every
        // file. A caller that says "the files already done are not
        // redone" is telling the truth about an incremental pass and
        // a lie about this one.
        bool refolded = false;
    };
    // SQA PERF-2: update() took 190,492 ms over the real 8,988-file
    // library on the GUI thread, with no progress and no way out.
    // This callback is invoked once per file (done, total, relative
    // path) and periodically during the directory walk (where total
    // is 0 — still counting); returning false stops the update.
    //
    // A stop is only ever honoured BETWEEN files. Stopping inside a
    // file would leave it recorded with its real mtime and size and
    // only some of its lines, and every later update would skip it as
    // unchanged — a silent hole in the index, which is worse than the
    // freeze this fixes.
    using UpdateProgress =
        std::function<bool(int done, int total, const std::string& path)>;
    // Walk root recursively (.txt/.acip/.md/.act/.inc/.ace, ≤10 MB) and
    // bring the index up to date.
    UpdateStats update(const std::string& root,
                       const UpdateProgress& progress = {});

    long long fileCount() const;
    long long lineCount() const;

    // Gofer grammar over the indexed lines; hits sorted by (file, line_lo).
    //
    // SQA PERF-1 (critical): every node used to materialise its FULL
    // match set before `limit` was consulted, so asking for 1 hit cost
    // the same as asking for 400, and the pane's "AND (same file)"
    // compiles to NEAR/1000000 — the entire same-file cross product.
    // Measured on the real 2.36 GB index: three ordinary Tibetan words
    // ANDed took 346,116 ms (5m46s) and 18.0 GB peak RSS, on the GUI
    // thread, with a Stop button that could not interrupt it.
    //
    // Each node is now bounded by kScanCap. That is a CAP, so it is
    // disclosed rather than silent (house rule 3): pass `truncated` and
    // tell the reader their result set was cut, or they will read a
    // partial answer as a complete one.
    static constexpr int kScanCap = 200000;
    std::vector<FileGoferHit> search(const std::string& query,
                                     int limit = 60,
                                     bool* truncated = nullptr) const;

private:
    sqlite3* db_ = nullptr;
};

}  // namespace allcore
