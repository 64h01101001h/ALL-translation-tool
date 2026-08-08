#include "allcore/abbr.h"

#include <fstream>
#include <sstream>

namespace allcore {

namespace {

std::string foldWylie(const std::string& s) {
    std::string out;
    out.reserve(s.size());
    for (char c : s)
        out.push_back(c >= 'A' && c <= 'Z' ? static_cast<char>(c + 32) : c);
    while (!out.empty() && (out.back() == ' ' || out.back() == '\r'))
        out.pop_back();
    return out;
}

std::string stripUni(const std::string& s) {
    std::string out = s;
    while (!out.empty() && (out.back() == ' ' || out.back() == '\r'))
        out.pop_back();
    // trailing tsheg U+0F0B = e0 bc 8b
    if (out.size() >= 3 &&
        static_cast<unsigned char>(out[out.size() - 3]) == 0xE0 &&
        static_cast<unsigned char>(out[out.size() - 2]) == 0xBC &&
        static_cast<unsigned char>(out[out.size() - 1]) == 0x8B)
        out.resize(out.size() - 3);
    return out;
}

}  // namespace

bool AbbrTable::load(const std::string& csvPath) {
    std::ifstream f(csvPath);
    if (!f) return false;
    std::string line;
    bool first = true;
    while (std::getline(f, line)) {
        if (first) {  // header (and its UTF-8 BOM)
            first = false;
            continue;
        }
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty()) continue;
        std::vector<std::string> cols;
        std::stringstream ss(line);
        std::string col;
        while (std::getline(ss, col, ',')) cols.push_back(col);
        if (cols.size() < 4) continue;
        AbbrEntry e;
        e.abbrWylie = cols[0];
        e.abbrUnicode = cols[1];
        e.expWylie = cols[2];
        e.expUnicode = cols[3];
        int ix = static_cast<int>(entries_.size());
        entries_.push_back(std::move(e));
        wylieIx_[foldWylie(entries_.back().abbrWylie)].push_back(ix);
        uniIx_[stripUni(entries_.back().abbrUnicode)].push_back(ix);
    }
    return !entries_.empty();
}

std::vector<const AbbrEntry*> AbbrTable::byWylie(const std::string& ewts) const {
    std::vector<const AbbrEntry*> out;
    auto it = wylieIx_.find(foldWylie(ewts));
    if (it != wylieIx_.end())
        for (int ix : it->second) out.push_back(&entries_[ix]);
    return out;
}

std::vector<const AbbrEntry*> AbbrTable::byUnicode(const std::string& uni) const {
    std::vector<const AbbrEntry*> out;
    auto it = uniIx_.find(stripUni(uni));
    if (it != uniIx_.end())
        for (int ix : it->second) out.push_back(&entries_[ix]);
    return out;
}

}  // namespace allcore
