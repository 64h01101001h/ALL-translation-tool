#include "allcore/subjects.h"

#include <fstream>
#include <sstream>

namespace allcore {

bool Subjects::load(const std::string& tsvPath) {
    std::ifstream f(tsvPath);
    if (!f) return false;
    byWork_.clear();
    rows_ = 0;
    std::string line;
    while (std::getline(f, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::vector<std::string> c;
        std::string cell;
        std::istringstream ss(line);
        while (std::getline(ss, cell, '\t')) c.push_back(cell);
        if (c.size() < 6) continue;
        WorkSubject s;
        s.english = c[1];
        s.tibetan = c[2];
        s.raw = c[3];
        s.source = c[4];
        s.vinaya = (c[5] == "1");
        byWork_[c[0]].push_back(std::move(s));
        ++rows_;
    }
    return rows_ > 0;
}

const std::vector<WorkSubject>& Subjects::forWork(
    const std::string& workKey) const {
    const auto it = byWork_.find(workKey);
    return it == byWork_.end() ? empty_ : it->second;
}

bool Subjects::isVinaya(const std::string& workKey) const {
    for (const auto& s : forWork(workKey))
        if (s.vinaya) return true;
    return false;
}

}  // namespace allcore
