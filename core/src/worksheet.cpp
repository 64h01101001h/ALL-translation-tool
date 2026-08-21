// worksheet.cpp — see allcore/worksheet.h.
#include "allcore/worksheet.h"

#include <sstream>

namespace allcore {

const std::vector<std::string>& worksheetSchema() {
    // verbatim from the team's live spreadsheet (tab 1 header row)
    static const std::vector<std::string> kSchema = {
        "ACIP Number",
        "In Database",
        "Tohoku",
        "Title Page Title",
        "Tibetan Title",
        "English Title",
        "Sanskrit Title",
        "Author / Authorship Statement",
        "Alternate Name",
        "Author Dates",
        "Tohoku Volume ID",
        "ALL/BDRC Volume ID & Folio Count",
        "ACIP/ALL Database Link",
        "ACIP/ALL Scan Link",
        "ACIP/ALL Subject Category",
        "Byte Count (MD5 Checksum)",
        "BDRC Permalink",
        "BDRC Work RID",
        "BDRC Image Group RID",
        "BDRC Volume RID",
        "Volume Number",
        "Sequence in Volume",
        "Image Number Range in BDRC Volume",
        "Tohoku Notes",
        "General Notes",
        "BDRC notes (Summary; Authorship Statement)",
        "Publisher:",
        "Location of printing:",
        "Colophon:",
        "Place of Production: ",
        "Drawings or illustrations:",
        "Input File Name",
        "Language(s) of the main text:",
        "Other languages in the book:",
        "Brief Title (native language):",
        "Brief Title (translated):",
        "Translator:",
        "Dates of the translator:",
        "Editor:",
        "Dates of the editor:",
        "Year of this edition:",
        "Format of the book:",
        "Cover:",
        "Condition of book:",
        "Readability of book:",
        "Volume:",
        "Page numbers:",
        "Size of pages:",
        "Size of printed area:",
        "Location of this copy:",
        "Copyright owner:",
        "ISBN number:",
    };
    return kSchema;
}

namespace {

std::string escapeTsv(const std::string& v) {
    std::string o;
    o.reserve(v.size());
    for (char c : v) {
        if (c == '\t') o += "\\t";
        else if (c == '\n') o += "\\n";
        else if (c == '\r') continue;
        else if (c == '\\') o += "\\\\";
        else o.push_back(c);
    }
    return o;
}

std::string unescapeTsv(const std::string& v) {
    std::string o;
    o.reserve(v.size());
    for (size_t i = 0; i < v.size(); ++i) {
        if (v[i] == '\\' && i + 1 < v.size()) {
            const char n = v[i + 1];
            if (n == 't') { o.push_back('\t'); ++i; continue; }
            if (n == 'n') { o.push_back('\n'); ++i; continue; }
            if (n == '\\') { o.push_back('\\'); ++i; continue; }
        }
        o.push_back(v[i]);
    }
    return o;
}

std::string csvCell(const std::string& v) {
    if (v.find_first_of(",\"\n\r") == std::string::npos) return v;
    std::string o = "\"";
    for (char c : v) {
        if (c == '"') o += "\"\"";
        else o.push_back(c);
    }
    o += "\"";
    return o;
}

}  // namespace

std::string serializeWorksheet(const Worksheet& w) {
    std::string out =
        "; cataloging worksheet sidecar - working data, not the "
        "official catalog\n";
    // schema order first, then any unknown fields (the schema may grow)
    for (const auto& f : worksheetSchema()) {
        const auto it = w.find(f);
        if (it != w.end() && !it->second.empty())
            out += escapeTsv(f) + "\t" + escapeTsv(it->second) + "\n";
    }
    for (const auto& [k, v] : w) {
        bool known = false;
        for (const auto& f : worksheetSchema())
            if (f == k) { known = true; break; }
        if (!known && !v.empty())
            out += escapeTsv(k) + "\t" + escapeTsv(v) + "\n";
    }
    return out;
}

Worksheet parseWorksheet(const std::string& text) {
    Worksheet w;
    std::istringstream is(text);
    std::string line;
    while (std::getline(is, line)) {
        if (line.empty() || line[0] == ';') continue;
        // the field/value tab is the first UNESCAPED tab
        size_t tab = std::string::npos;
        for (size_t i = 0; i < line.size(); ++i) {
            if (line[i] == '\\') { ++i; continue; }
            if (line[i] == '\t') { tab = i; break; }
        }
        if (tab == std::string::npos) continue;
        w[unescapeTsv(line.substr(0, tab))] =
            unescapeTsv(line.substr(tab + 1));
    }
    return w;
}

std::string worksheetCsvRow(const Worksheet& w, bool with_header) {
    std::string out;
    if (with_header) {
        bool first = true;
        for (const auto& f : worksheetSchema()) {
            if (!first) out += ",";
            out += csvCell(f);
            first = false;
        }
        out += "\n";
    }
    bool first = true;
    for (const auto& f : worksheetSchema()) {
        if (!first) out += ",";
        const auto it = w.find(f);
        if (it != w.end()) out += csvCell(it->second);
        first = false;
    }
    out += "\n";
    return out;
}

}  // namespace allcore
