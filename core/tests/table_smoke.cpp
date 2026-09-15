// table_smoke — the delimited-table reader battery (F7 Table Compare,
// acceptance tests 1-6). Fixture-free: every input is spelled out here,
// so the battery runs anywhere.
#include <cstdio>
#include <string>
#include <vector>

#include "allcore/table.h"

using namespace allcore::table;
static int failures = 0;
#define CHECK(cond, msg) do { if (cond) std::printf("  [PASS] %s\n", msg); else { std::printf("  [FAIL] %s\n", msg); ++failures; } } while (0)

static bool has(const std::string& hay, const std::string& needle) {
    return hay.find(needle) != std::string::npos;
}

int main() {
    setvbuf(stdout, nullptr, _IONBF, 0);

    // 1 — RFC-4180 quoting: a quoted comma and a doubled quote
    {
        const Table t = readDelimited(
            "a,b,c\n1,\"b,with,commas\",\"Geshe Michael said \"\"hi\"\"\"\n");
        CHECK(t.delimiter == ',' && t.headerFromFile &&
                  t.columns == std::vector<std::string>({"a", "b", "c"}),
              "T1 header read from the file; comma delimiter");
        CHECK(t.rows.size() == 1 &&
                  t.rows[0].cells ==
                      std::vector<std::string>({"1", "b,with,commas",
                                                "Geshe Michael said \"hi\""}),
              "T1 a quoted comma stays in the cell and \"\" becomes one quote");
        CHECK(t.rows.size() == 1 && t.rows[0].sourceLine == 2 &&
                  t.refusedRows == 0,
              "T1 sourceLine is 1-based (the data row is line 2); nothing refused");
    }

    // 2 — a header carrying both a tab and a comma: the tab wins, and
    // the choice is on the record
    {
        const Table t = readDelimited("wylie\tgloss, with a comma\tnote\n"
                                      "chos\tdharma\tthe first\n");
        CHECK(t.delimiter == '\t' &&
                  t.columns == std::vector<std::string>(
                                   {"wylie", "gloss, with a comma", "note"}),
              "T2 a tab in the header beats the comma; the comma stays inside "
              "the column name");
        CHECK(has(t.detectionNote, "tab") && !t.detectionNote.empty(),
              "T2 the delimiter choice is recorded in detectionNote");
        CHECK(t.rows.size() == 1 && t.rows[0].cells.size() == 3 &&
                  t.rows[0].cells[1] == "dharma",
              "T2 the data row splits on the same delimiter");
        const Table forced = readDelimited("a\tb,c\n1\t2,3\n", Format::Csv);
        CHECK(forced.delimiter == ',' && forced.columns.size() == 2 &&
                  has(forced.detectionNote, "comma"),
              "T2 an explicit Format overrides detection and says so");
    }

    // 3 — CRLF input: no '\r' rides along in the last cell
    {
        const Table t = readDelimited("a,b\r\n1,x\r\n2,\"y\"\r\n");
        CHECK(t.columns.size() == 2 && t.columns[1] == "b" &&
                  t.rows.size() == 2 && t.refusedRows == 0,
              "T3 CRLF: the cell count matches the header on every row");
        CHECK(t.rows[0].cells[1] == "x" && t.rows[1].cells[1] == "y",
              "T3 CRLF: the last cell has no trailing carriage return, quoted "
              "or not");
    }

    // 4 — a '#'-commented headerless house TSV read under implied columns
    {
        const Table t = readDelimited(
            "# glossary for course 3\n"
            "# generated 2026-09-09\n"
            "chos\tdharma\n"
            "sems can\tliving being\n",
            Format::Auto, {"wylie", "gloss"});
        CHECK(!t.headerFromFile &&
                  t.columns == std::vector<std::string>({"wylie", "gloss"}),
              "T4 implied columns: headerFromFile is false and the names are "
              "the caller's");
        CHECK(t.rows.size() == 2 && t.rows[0].cells[0] == "chos" &&
                  t.rows[1].cells[1] == "living being",
              "T4 no data row is eaten as a header");
        CHECK(t.comments.size() == 2 && has(t.comments[0], "course 3") &&
                  has(t.comments[1], "2026-09-09"),
              "T4 the '#' lines are preserved as comments");
        CHECK(t.delimiter == '\t' && t.rows[0].sourceLine == 3 &&
                  has(t.detectionNote, "not read from the file"),
              "T4 a comment is neither data nor header: line numbers still "
              "count it, and the note says the names are not the file's");
    }

    // 5 — a short row is refused, not padded
    {
        const Table t = readDelimited("a,b,c\n"
                                      "1,2\n"
                                      "# a comment between rows\n"
                                      "4,5,6\n"
                                      "7,8,9,10\n");
        CHECK(t.refusedRows == 2 && t.rows.size() == 1 &&
                  t.rows[0].cells[0] == "4",
              "T5 the short row and the long row are refused; only the "
              "matching row is kept");
        CHECK(t.refusals.size() == 2 && has(t.refusals[0], "line 2") &&
                  has(t.refusals[0], "1,2"),
              "T5 the refusal carries the line number AND the raw text");
        CHECK(has(t.refusals[1], "line 5") && t.comments.size() == 1 &&
                  t.rows[0].sourceLine == 4,
              "T5 a comment between rows does not shift the line numbers and "
              "is not itself refused");
    }

    // 6 — writeCsv round-trips the awkward cell
    {
        Table t;
        t.columns = {"key", "value"};
        Row r;
        r.cells = {"chos", "a comma, a \"quote\" and a\nnewline"};
        r.sourceLine = 2;
        t.rows.push_back(r);
        const std::string csv = writeCsv(t);
        CHECK(has(csv, "\"a comma, a \"\"quote\"\" and a\nnewline\""),
              "T6 the cell is quoted and its own quotes are doubled");
        const Table back = readDelimited(csv, Format::Csv);
        CHECK(back.columns == t.columns && back.rows.size() == 1 &&
                  back.rows[0].cells == r.cells && back.refusedRows == 0,
              "T6 writeCsv → readDelimited round-trips comma, quote and "
              "newline");
        Table plain;
        plain.columns = {"a", "b"};
        Row p;
        p.cells = {"1", "2"};
        plain.rows.push_back(p);
        CHECK(writeCsv(plain) == "a,b\n1,2\n",
              "T6 an ordinary table is written unquoted, one row per line");
    }

    std::printf("table_smoke: %d failure(s)\n", failures);
    return failures ? 1 : 0;
}
