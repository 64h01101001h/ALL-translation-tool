// catalog_register.cpp — see allcore/catalog_register.h.
#include "allcore/catalog_register.h"

#include <cctype>
#include <sstream>

#include "allcore/catalog_audit.h"

namespace allcore {
namespace {

std::string lower(std::string s) {
    for (char& c : s)
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return s;
}

// split one CSV/TSV line; handles double-quoted CSV fields
std::vector<std::string> splitRow(const std::string& line, char delim) {
    std::vector<std::string> out;
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
            out.push_back(cur);
            cur.clear();
        } else if (c != '\r') {
            cur.push_back(c);
        }
    }
    out.push_back(cur);
    for (auto& f : out) {
        const size_t a = f.find_first_not_of(" \t");
        if (a == std::string::npos) { f.clear(); continue; }
        const size_t b = f.find_last_not_of(" \t");
        f = f.substr(a, b - a + 1);
    }
    return out;
}

}  // namespace

int CatalogRegister::loadText(const std::string& text) {
    entries_.clear();
    std::istringstream is(text);
    std::string header;
    if (!std::getline(is, header)) return 0;
    // delimiter: a tab in the header wins, else comma
    const char delim =
        header.find('\t') != std::string::npos ? '\t' : ',';
    const auto heads = splitRow(header, delim);
    int cNum = -1, cTitle = -1, cFolS = -1, cFolE = -1, cDate = -1,
        cWho = -1;
    for (size_t i = 0; i < heads.size(); ++i) {
        const std::string h = lower(heads[i]);
        const int ii = static_cast<int>(i);
        if (cNum < 0 && (h.find("number") != std::string::npos ||
                         h.find("catalog") != std::string::npos ||
                         h == "no" || h == "no." || h == "id"))
            cNum = ii;
        else if (cTitle < 0 && h.find("title") != std::string::npos)
            cTitle = ii;
        else if (cFolS < 0 && h.find("folio") != std::string::npos &&
                 h.find("end") == std::string::npos)
            cFolS = ii;
        else if (cFolE < 0 && h.find("folio") != std::string::npos)
            cFolE = ii;
        else if (cDate < 0 && h.find("date") != std::string::npos)
            cDate = ii;
        else if (cWho < 0 && (h.find("initial") != std::string::npos ||
                              h.find("who") != std::string::npos ||
                              h.find("by") == 0))
            cWho = ii;
    }
    if (cNum < 0) return 0;
    auto cell = [](const std::vector<std::string>& row, int c) {
        return c >= 0 && c < static_cast<int>(row.size()) ? row[c]
                                                          : std::string();
    };
    std::string line;
    while (std::getline(is, line)) {
        if (line.empty()) continue;
        const auto row = splitRow(line, delim);
        RegisterEntry e;
        e.number = cell(row, cNum);
        if (e.number.empty() ||
            normalizeCatalogNumber(e.number).second < 0)
            continue;   // a row without a real number is not a row
        e.title = cell(row, cTitle);
        e.folio_start = cell(row, cFolS);
        e.folio_end = cell(row, cFolE);
        e.date = cell(row, cDate);
        e.initials = cell(row, cWho);
        entries_.push_back(std::move(e));
    }
    return static_cast<int>(entries_.size());
}

const RegisterEntry* CatalogRegister::find(const std::string& number) const {
    const auto n = normalizeCatalogNumber(number);
    if (n.second < 0) return nullptr;
    for (const auto& e : entries_)
        if (normalizeCatalogNumber(e.number) == n) return &e;
    return nullptr;
}

}  // namespace allcore
