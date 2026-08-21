// allcore/catalog_list.h — the ASCII master catalog generator (9g 2b/4).
//
// GMR, session 3 (Jan 19 2026): "We have the database, we have the
// folders, but we don't have a separate LIST of what's in the folders…
// I want this team to go back to having an ASCII catalog." The model is
// the St. Petersburg catalog: field-coded ASCII lines, one record per
// book, readable by any computer forever — the future-proofing doctrine
// (the Ask Sam story) applied to the catalog itself.
//
// This generates that LIST from a library tree. It is a list of what the
// folders hold — NOT the official catalog, which changes only through
// data releases. Field lines (a line is omitted when its value is
// unknown; nothing is guessed):
//
//   S:  catalog number as the filename asserts it (or "uncataloged")
//   F:  the filename
//   D:  the folder, relative to the root (the shelf it sits on)
//   T:  Tibetan title  — from the filename; "T*" when it had to be read
//       from the text's own title page instead (marked, per rule 3)
//   E:  English title  — from the filename
//   A:  author (dates) — from the filename
//   V:  verification status + language, decoded from the ACIP name
//   P:  size · lines · folio range seen in the text (e.g. "85A-92B")
//
// Records are separated by one blank line; the file opens with a
// commented header documenting the format and its lineage.
#pragma once

#include <string>

namespace allcore {

struct AsciiCatalogStats {
    int files = 0;        // eligible text files walked
    int cataloged = 0;    // filename decodes to a catalog number
    int uncataloged = 0;
    int titled_from_text = 0;   // T* records (title read from the text)
};

struct AsciiCatalogResult {
    std::string text;     // the whole catalog, plain ASCII
    AsciiCatalogStats stats;
};

// Walk root (recursively; .txt/.act/.inc/.ace, META companions skipped)
// and emit the field-coded list. Deterministic order (sorted paths).
AsciiCatalogResult generateAsciiCatalog(const std::string& root);

// First and last @NNN[AB] folio marks in a document body ("","" when
// none) — the worksheet's folio-range prefill uses this too.
std::pair<std::string, std::string> acipFolioRange(
    const std::string& body);

}  // namespace allcore
