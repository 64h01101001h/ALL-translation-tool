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

// ---- the change-log stamp (session 4/Jun 16 policy) ------------------------
// "You have to rename the copy with your initials so that we know you
// changed it on this day, and to blame you if something went wrong.
// That is our policy." A stamped folder name looks like
//   "<base> - updated 2026-08-19 ADA"
// The parser is tolerant of the older hand-made variants (a date-ish
// token followed by trailing initials).
struct ChangeStamp {
    bool found = false;
    std::string base;       // the name without the stamp
    std::string date;       // as written
    std::string initials;   // 2-4 capitals
};

ChangeStamp parseChangeStamp(const std::string& folder_name);
std::string composeChangeStamp(const std::string& base,
                               const std::string& iso_date,
                               const std::string& initials);

}  // namespace allcore
