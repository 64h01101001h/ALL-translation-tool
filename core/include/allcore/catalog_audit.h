// allcore/catalog_audit.h — the bibliography audit (TODO 9g step 2a).
//
// GMR's first job to the cataloging class (session 1, Dec 22 2025; session
// 2 ran it by AI): every ACIP catalog number cited in the published Mixed
// Nuts bibliographies must exist in the database — "if a book made it into
// a bibliography it was important enough that it sure as hell better be in
// the database." This module is that check, run against whatever library
// tree the app is pointed at.
//
// HONESTY CLAUSE carried by every report: the installed library is the
// app's subset, not ACIP's master tree — "missing here" is a lower bound
// on what the master audit would find, and presence here is not proof of
// presence in the master.
#pragma once

#include <cstddef>
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace allcore {

// One "ACIP <number>" citation found in a text (bibliography JSON, plain
// bibliography text — the scanner does not care which).
struct AcipCitation {
    std::string number;   // as cited, e.g. "TD03971", "S464", "KL00824"
    size_t offset = 0;    // byte offset in the scanned text
};

// Scan any text for "ACIP <letters><digits>" citations. Case-sensitive on
// "ACIP" (the published apparatus always sets it in caps); the number is
// 1-2 letters + 3 or more digits (no upper cap — Adam 2026-08-19) with an
// optional dashed sub-number.
std::vector<AcipCitation> extractAcipCitations(const std::string& text);

// "S464" and "S00464" are the same number filed with different padding;
// "S05002-1" and "S05002-2" are DIFFERENT works (the dashed sub-number,
// live in the registrar's own worksheet). Normalized key: prefix +
// unpadded digits + optional "-sub" ("S464", "S5002-1"). Empty = not a
// catalog number. NOTE (Adam, 2026-08-19): ACIP numbers are NOT capped
// at 5 digits going forward — no upper digit limit here.
std::string normalizeCatalogKey(const std::string& num);
// the key with any dashed sub-number stripped ("S5002-1" -> "S5002").
// Dashes mean different things in different sources (the registrar's
// sub-works, a citation's volume, a filename's range) — presence
// AUDITS compare at the base-work level, the honest common ground;
// the register keeps full keys.
std::string baseCatalogKey(const std::string& num);
// compatibility shim: (prefix, value) with -1 for invalid; sub-numbers
// LOSE their suffix here — prefer normalizeCatalogKey
std::pair<std::string, int> normalizeCatalogNumber(const std::string& num);

// Walk a library tree and collect the normalized catalog keys its
// FILENAMES assert (leading 1-2 letters + 3+ digits + optional dashed
// sub-number; META files count — they assert their partner's identity).
std::set<std::string> collectLibraryNumbers(const std::string& root);

struct AuditEntry {
    std::string number;   // as cited (first spelling seen)
    bool present = false;
    int citations = 0;    // how many times it was cited across the texts
};

struct AuditResult {
    int cited_distinct = 0;
    int present = 0;
    int missing = 0;
    std::vector<AuditEntry> entries;   // missing first, then present;
                                       // each group sorted by number
};

// The audit: which cited numbers exist in the library, which do not.
AuditResult auditPresence(const std::vector<AcipCitation>& cited,
                          const std::set<std::string>& have);

}  // namespace allcore
