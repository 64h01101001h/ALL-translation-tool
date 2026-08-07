#include "allcore/verbstems.h"

#include <fstream>

namespace allcore {

bool VerbStems::load(const std::string& tsv_path) {
    std::ifstream f(tsv_path);
    if (!f) return false;
    std::string line;
    while (std::getline(f, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::vector<std::string> cols;
        size_t p = 0;
        while (p <= line.size()) {
            auto t = line.find('\t', p);
            if (t == std::string::npos) t = line.size();
            cols.push_back(line.substr(p, t - p));
            p = t + 1;
        }
        if (cols.size() < 5) continue;
        map_[cols[0]].push_back({cols[1], cols[3], cols[4]});
    }
    return !map_.empty();
}

std::vector<VerbStemHit> VerbStems::lookup(
    const std::string& unicode) const {
    auto it = map_.find(unicode);
    return it == map_.end() ? std::vector<VerbStemHit>{} : it->second;
}

}  // namespace allcore
