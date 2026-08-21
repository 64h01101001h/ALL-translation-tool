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
    std::vector<FileGoferHit> search(const std::string& query,
                                     int limit = 60) const;

private:
    sqlite3* db_ = nullptr;
};

}  // namespace allcore
