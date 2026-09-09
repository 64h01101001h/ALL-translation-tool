// table.cpp — see allcore/table.h.
#include "allcore/table.h"

#include <cstddef>

namespace allcore {
namespace table {
namespace {

// The one state machine. splitRow drives it for the fields; the record
// assembler drives it (with fields thrown away) to learn whether a line
// ended inside a quoted field. One implementation, so the two can never
// drift apart. Returns true when the line ends still inside quotes.
bool scanRow(const std::string& line, char delim,
             std::vector<std::string>* out) {
    std::string cur;
    bool quoted = false;
    for (size_t i = 0; i < line.size(); ++i) {
        const char c = line[i];
        if (quoted) {
            if (c == '"') {
                if (i + 1 < line.size() && line[i + 1] == '"') {
                    cur.push_back('"');
                    ++i;
                } else {
                    quoted = false;
                }
            } else {
                cur.push_back(c);
            }
        } else if (c == '"' && cur.empty()) {
            quoted = true;
        } else if (c == delim) {
            if (out) out->push_back(cur);
            cur.clear();
        } else if (c != '\r') {
            cur.push_back(c);
        }
    }
    if (out) out->push_back(cur);
    return quoted;
}

bool isComment(const std::string& line) { return !line.empty() && line[0] == '#'; }

bool isBlank(const std::string& line) {
    return line.find_first_not_of(" \t\r") == std::string::npos;
}

// one physical line, its trailing '\r' gone
std::string stripCr(const std::string& s) {
    if (!s.empty() && s.back() == '\r') return s.substr(0, s.size() - 1);
    return s;
}

std::vector<std::string> physicalLines(const std::string& text) {
    std::vector<std::string> out;
    std::string cur;
    for (const char c : text) {
        if (c == '\n') {
            out.push_back(stripCr(cur));
            cur.clear();
        } else {
            cur.push_back(c);
        }
    }
    if (!cur.empty()) out.push_back(stripCr(cur));
    return out;
}

std::string quoteCell(const std::string& s) {
    bool needs = false;
    for (const char c : s)
        if (c == ',' || c == '"' || c == '\n' || c == '\r') { needs = true; break; }
    if (!needs) return s;
    std::string out = "\"";
    for (const char c : s) {
        if (c == '"') out.push_back('"');
        out.push_back(c);
    }
    out.push_back('"');
    return out;
}

std::string joinCsv(const std::vector<std::string>& cells) {
    std::string out;
    for (size_t i = 0; i < cells.size(); ++i) {
        if (i) out.push_back(',');
        out += quoteCell(cells[i]);
    }
    return out;
}

}  // namespace

std::vector<std::string> splitRow(const std::string& line, char delim) {
    std::vector<std::string> out;
    scanRow(line, delim, &out);
    for (auto& f : out) {
        const size_t a = f.find_first_not_of(" \t");
        if (a == std::string::npos) { f.clear(); continue; }
        const size_t b = f.find_last_not_of(" \t");
        f = f.substr(a, b - a + 1);
    }
    return out;
}

Table readDelimited(const std::string& text, Format format,
                    const std::vector<std::string>& impliedColumns) {
    Table t;
    const auto lines = physicalLines(text);

    // A record is one or more physical lines: a quoted field may hold a
    // newline. Comments and blanks are only recognised OUTSIDE a
    // quoted field, so a '#' inside a quoted cell stays data.
    struct Record { std::string text; int line = 0; };
    std::vector<Record> records;

    // The delimiter has to be known before records can be assembled
    // (quote state depends on it), so decide it from the first line
    // that is neither blank nor a comment — the header when there is
    // one, the first data line when the caller supplied the columns.
    std::string shapeLine;
    for (const auto& l : lines)
        if (!isBlank(l) && !isComment(l)) { shapeLine = l; break; }
    const bool sawTab = shapeLine.find('\t') != std::string::npos;
    const char delim = format == Format::Csv   ? ','
                       : format == Format::Tsv ? '\t'
                                               : (sawTab ? '\t' : ',');
    t.delimiter = delim;

    const char* what = delim == '\t' ? "tab-separated" : "comma-separated";
    if (format != Format::Auto) {
        t.detectionNote = std::string(what) + " (the caller said so)";
    } else if (sawTab) {
        t.detectionNote = std::string(what) +
                          " (a tab in the first line chose it)";
    } else {
        t.detectionNote = std::string(what) +
                          " (no tab in the first line, so comma)";
    }

    for (size_t i = 0; i < lines.size();) {
        if (isComment(lines[i])) {
            t.comments.push_back(lines[i]);
            ++i;
            continue;
        }
        if (isBlank(lines[i])) { ++i; continue; }
        Record r;
        r.line = static_cast<int>(i) + 1;   // 1-based
        r.text = lines[i];
        ++i;
        // an open quote swallows the following lines, newlines and all
        while (scanRow(r.text, delim, nullptr) && i < lines.size()) {
            r.text.push_back('\n');
            r.text += lines[i];
            ++i;
        }
        records.push_back(std::move(r));
    }

    size_t first = 0;
    if (!impliedColumns.empty()) {
        t.columns = impliedColumns;
        t.headerFromFile = false;
        t.detectionNote += "; column names were not read from the file";
    } else if (!records.empty()) {
        t.columns = splitRow(records[0].text, delim);
        t.headerFromFile = true;
        first = 1;
    }

    for (size_t i = first; i < records.size(); ++i) {
        auto cells = splitRow(records[i].text, delim);
        if (cells.size() != t.columns.size()) {
            // never padded, never truncated — refused, with the line
            // number and the raw text so it can be found and fixed
            ++t.refusedRows;
            t.refusals.push_back("line " + std::to_string(records[i].line) +
                                 ": " + records[i].text);
            continue;
        }
        Row row;
        row.cells = std::move(cells);
        row.sourceLine = records[i].line;
        t.rows.push_back(std::move(row));
    }
    return t;
}

std::string writeCsv(const Table& t) {
    std::string out;
    for (const auto& c : t.comments) { out += c; out.push_back('\n'); }
    if (!t.columns.empty()) { out += joinCsv(t.columns); out.push_back('\n'); }
    for (const auto& r : t.rows) { out += joinCsv(r.cells); out.push_back('\n'); }
    return out;
}

}  // namespace table
}  // namespace allcore
