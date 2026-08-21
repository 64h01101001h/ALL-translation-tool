// allcore/worksheet.h — the cataloging worksheet (9g step 3), built on
// THE REAL SCHEMA: the team's live 52-column spreadsheet (Adam's Google
// sheet, tab 1, read in full 2026-08-19; banked in
// docs/research/GMR_CATALOGING_SESSIONS.md). No guessed fields.
//
// The worksheet is WORKING DATA for a cataloger: it lives as a sidecar
// file beside the intake text ("<name>.worksheet.tsv"), never inside the
// destination tree, and it writes nothing official. "Export row" emits
// one CSV line in the sheet's own column order for pasting into the live
// spreadsheet — the spreadsheet stays the team's master.
#pragma once

#include <map>
#include <string>
#include <vector>

namespace allcore {

// The 52 column names, in the sheet's own order.
const std::vector<std::string>& worksheetSchema();

using Worksheet = std::map<std::string, std::string>;

// Sidecar format: one "Field\tValue" line per non-empty field (tabs and
// newlines in values escaped as \t and \n). Unknown field names survive
// a round trip — the schema may grow.
std::string serializeWorksheet(const Worksheet& w);
Worksheet parseWorksheet(const std::string& text);

// One CSV line in schema order (fields absent from w are empty cells);
// embedded commas/quotes/newlines quoted per RFC 4180. with_header
// prepends the column-name row.
std::string worksheetCsvRow(const Worksheet& w, bool with_header = true);

}  // namespace allcore
