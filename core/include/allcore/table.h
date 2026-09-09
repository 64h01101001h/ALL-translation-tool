// allcore/table.h — the delimited-table reader (F7 Table Compare, the
// reader half; also the import lane's reader).
//
// One job: turn CSV/TSV text into columns and rows WITHOUT guessing.
// The splitter is the one that used to sit trapped in
// catalog_register.cpp's anonymous namespace — it is promoted here
// unchanged (register_smoke is the proof the lift is faithful) and
// CatalogRegister now calls it.
//
// The house rules this file keeps:
//   * a row whose cell count does not match the header is REFUSED —
//     recorded with its line number and its raw text, never padded and
//     never truncated. Refusing beats guessing which column went
//     missing.
//   * '#' lines are comments. They are never data and never a header.
//   * when the caller supplies impliedColumns (the headerless
//     '#'-commented house TSVs) no data row is eaten as a header, and
//     headerFromFile says so, so the pane can state that the column
//     names did not come from the file.
//   * the delimiter choice is recorded in detectionNote rather than
//     silently assumed.
//
// Pure C++: no Qt, no product code.
#pragma once

#include <string>
#include <vector>

namespace allcore {
namespace table {

enum class Format { Auto, Csv, Tsv };

struct Row {
    std::vector<std::string> cells;
    int sourceLine = 0;   // 1-based line in the source text
};

struct Table {
    std::vector<std::string> columns;
    std::vector<Row> rows;
    std::vector<std::string> comments;   // '#' lines, verbatim, in order
    char delimiter = '\t';
    bool headerFromFile = false;         // false = columns came from the caller
    int refusedRows = 0;
    std::vector<std::string> refusals;   // "line N: <raw text>"
    std::string detectionNote;           // how the delimiter (and the header) were decided
};

// Split one CSV/TSV line into fields. RFC-4180 double quotes ("" is a
// literal quote inside a quoted field), unquoted '\r' dropped, and each
// field trimmed of leading/trailing spaces and tabs. Lifted verbatim
// from catalog_register.cpp so the registrar's parse is byte-identical.
// Consequence of the trim, stated once here: a cell's own leading or
// trailing spaces do not survive a read, quoted or not.
std::vector<std::string> splitRow(const std::string& line, char delimiter);

// Read delimited text.
//   Format::Auto — the delimiter is taken from the first line that is
//   neither blank nor a comment: a tab in it wins, else comma.
//   impliedColumns non-empty — the text has NO header row: the columns
//   are the caller's and every non-comment line is data.
// A quoted field may span lines (RFC-4180); the record's sourceLine is
// the line it starts on. A quote left open at end of text ends the
// record there, which normally lands it in refusals — an unterminated
// quote is not repaired.
Table readDelimited(const std::string& text, Format format = Format::Auto,
                    const std::vector<std::string>& impliedColumns = {});

// Write the table as RFC-4180 CSV: comments first (verbatim), then the
// header row when the table has columns, then the rows. A cell holding
// a comma, a double quote, a CR or an LF is quoted and its own quotes
// doubled. Lines end with '\n'.
std::string writeCsv(const Table& t);

}  // namespace table
}  // namespace allcore
