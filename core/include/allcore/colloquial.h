// allcore/colloquial.h — the colloquial pronunciation register
// (data/pron_colloquial/): community spellings (gonpa, tulku,
// chenrezig…) mapped to their wylie dictionary entries. CURATED SEED
// tier; every row is spine-verified at generation. The register only
// WIDENS lookup — the GMR convention stays canonical everywhere.
#pragma once

#include <map>
#include <string>
#include <vector>

namespace allcore {

struct ColloquialEntry {
    std::string colloquial;
    std::string wylie;
    std::string gmrPron;
    std::string cls;   // "community" | "prenasal-derived"
};

class ColloquialPron {
public:
    bool load(const std::string& tsvPath);
    size_t size() const { return entries_.size(); }

    // case-folded, letters-only match (same fold as the pron index)
    std::vector<const ColloquialEntry*> byColloquial(
        const std::string& query) const;

private:
    std::vector<ColloquialEntry> entries_;
    std::map<std::string, std::vector<int>> ix_;
};

}  // namespace allcore
