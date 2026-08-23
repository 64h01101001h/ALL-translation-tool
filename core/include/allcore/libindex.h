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
    };
    // Walk root recursively (.txt/.acip/.md/.act/.inc/.ace, ≤10 MB) and
    // bring the index up to date.
    UpdateStats update(const std::string& root);

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
