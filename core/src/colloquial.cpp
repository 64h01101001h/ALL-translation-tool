#include "allcore/colloquial.h"

#include <fstream>
#include <sstream>

namespace allcore {

namespace {
// the pron_index fold: lowercase, letters only
std::string foldPron(const std::string& s) {
    std::string out;
    for (char c : s) {
        if (c >= 'a' && c <= 'z') out += c;
        else if (c >= 'A' && c <= 'Z') out += (char)(c - 'A' + 'a');
    }
    return out;
}
}  // namespace

bool ColloquialPron::load(const std::string& tsvPath) {
    std::ifstream f(tsvPath);
    if (!f) return false;
    std::string line;
    while (std::getline(f, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::vector<std::string> cols;
        std::stringstream ss(line);
        std::string col;
        while (std::getline(ss, col, '\t')) cols.push_back(col);
        if (cols.size() < 2) continue;
        ColloquialEntry e;
        e.colloquial = cols[0];
        e.wylie = cols[1];
        if (cols.size() > 2) e.gmrPron = cols[2];
        if (cols.size() > 3) e.cls = cols[3];
        const int ix = static_cast<int>(entries_.size());
        ix_[foldPron(e.colloquial)].push_back(ix);
        byWylie_[e.wylie].push_back(ix);
        entries_.push_back(std::move(e));
    }
    return !entries_.empty();
}

std::vector<const ColloquialEntry*> ColloquialPron::byWylie(
    const std::string& wylie) const {
    std::vector<const ColloquialEntry*> out;
    auto it = byWylie_.find(wylie);
    if (it != byWylie_.end())
        for (int ix : it->second) out.push_back(&entries_[ix]);
    return out;
}

std::vector<const ColloquialEntry*> ColloquialPron::byColloquial(
    const std::string& query) const {
    std::vector<const ColloquialEntry*> out;
    auto it = ix_.find(foldPron(query));
    if (it != ix_.end())
        for (int ix : it->second) out.push_back(&entries_[ix]);
    return out;
}

}  // namespace allcore
