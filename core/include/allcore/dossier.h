// allcore/dossier.h — Translation Dossiers (Lodestar L6 flagship).
//
// A dossier is a translator's DESK for one text: it points at the
// source, remembers where the reading stopped, and carries stamps.
// It never duplicates data — glossaries, comments, and drafts remain
// where they live; the dossier is the handle that reassembles the
// working world in one act (the excise it kills: rebuilding that
// world by hand every session).
//
// LOCAL store (library/dossiers.tsv) — a desk, not the shared record.
// Deterministic stamps are caller-supplied (drills pin behavior).
#pragma once

#include <string>
#include <vector>

namespace allcore {

struct Dossier {
    std::string slug;      // stable handle (from title + counter)
    std::string title;
    std::string textPath;  // the source text this desk serves
    int line = 1;          // 1-based reading position
    std::string created;
    std::string touched;   // most recent activity stamp
};

class DossierStore {
public:
    explicit DossierStore(const std::string& dir);
    bool load();           // missing file = empty store, true
    bool save();           // full rewrite (local, single-writer)
    // returns the new slug ("" on empty title/path)
    std::string create(const std::string& title,
                       const std::string& textPath, int line,
                       const std::string& stamp);
    bool touch(const std::string& slug, int line,
               const std::string& stamp);
    bool remove(const std::string& slug);
    // most-recently-touched first
    const std::vector<Dossier>& all() const { return items_; }

private:
    void sortByTouched();
    std::string dir_;
    std::vector<Dossier> items_;
};

}  // namespace allcore
