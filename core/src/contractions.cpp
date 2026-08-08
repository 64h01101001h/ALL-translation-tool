#include "allcore/contractions.h"

#include <fstream>
#include <sstream>

namespace allcore {

namespace {
std::string fold(const std::string& s) {
    std::string out;
    out.reserve(s.size());
    for (char c : s)
        out.push_back(c >= 'A' && c <= 'Z' ? static_cast<char>(c + 32) : c);
    while (!out.empty() && (out.back() == ' ' || out.back() == '\r'))
        out.pop_back();
    return out;
}
}  // namespace

bool Contractions::load(const std::string& tsvPath) {
    std::ifstream f(tsvPath);
    if (!f) return false;
    std::string line;
    while (std::getline(f, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty()) continue;
        std::vector<std::string> cols;
        std::stringstream ss(line);
        std::string col;
        while (std::getline(ss, col, '\t')) cols.push_back(col);
        if (cols.size() < 8) continue;
        Contraction c;
        c.shortWylie = cols[0];
        c.longWylie = cols[1];
        c.dropped = cols[2];
        c.cls = cols[3];
        c.glossKind = cols[4];
        c.gloss = cols[5];
        try {
            c.shortN = std::stol(cols[6]);
            c.longN = std::stol(cols[7]);
        } catch (...) {}
        int ix = static_cast<int>(entries_.size());
        entries_.push_back(std::move(c));
        ix_[fold(entries_.back().shortWylie)].push_back(ix);
    }
    return !entries_.empty();
}

std::vector<const Contraction*> Contractions::expansions(
    const std::string& wylie) const {
    std::vector<const Contraction*> out;
    auto it = ix_.find(fold(wylie));
    if (it != ix_.end())
        for (int ix : it->second) out.push_back(&entries_[ix]);
    return out;
}

}  // namespace allcore
