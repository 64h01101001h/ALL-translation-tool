// allcore/catalog_register.h — the registrar's spreadsheet, read-only
// (queue item 9). Sessions 4-8: Nick issues numbers from a spreadsheet;
// Apr 28 added folio start/end columns to it; the initials+date policy
// tracks who changed what. The app READS such a register — it never
// writes one, never mints a number.
//
// The three states the register view separates (session 1/2: works
// cited in bibliographies got numbers "but they're not in the catalog"):
//   1. number issued   — a register row (or an ACIP filename) says so
//   2. input exists    — a file carrying the number is present
//   3. cataloged       — that file sits inside the DESTINATION tree
// A work can hold any prefix of these; the view shows all three lights.
#pragma once

#include <string>
#include <vector>

namespace allcore {

struct RegisterEntry {
    std::string number;        // as filed, e.g. "S25239"
    std::string title;         // whatever title column the sheet has
    std::string folio_start, folio_end;
    std::string date;          // issue/change date column when present
    std::string initials;      // who (the initials policy)
};

class CatalogRegister {
public:
    // Parse CSV or TSV text (delimiter auto-detected from the header
    // row; quoted CSV fields handled). Column meanings are found by
    // header name (number/catalog, title, folio start/end, date,
    // initials/by/who) — case-insensitive substring match. Returns rows
    // banked; 0 with no usable number column.
    int loadText(const std::string& text);
    size_t size() const { return entries_.size(); }
    const std::vector<RegisterEntry>& entries() const { return entries_; }
    // padding-insensitive lookup (S464 == S00464); nullptr = absent
    const RegisterEntry* find(const std::string& number) const;

private:
    std::vector<RegisterEntry> entries_;
};

}  // namespace allcore
