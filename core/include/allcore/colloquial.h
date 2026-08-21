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
    // all registered variants for a dictionary headword
    std::vector<const ColloquialEntry*> byWylie(
        const std::string& wylie) const;

private:
    std::vector<ColloquialEntry> entries_;
    std::map<std::string, std::vector<int>> ix_;
    std::map<std::string, std::vector<int>> byWylie_;
};

// ---- the authority's ruling applied to the register file ----------
// Approving a pronunciation proposal whose form exists as a
// machine-derived row upgrades that row's class to "approved" in
// place (stamped "# ruled by <ruler> <date>"); declining removes the
// derived row. ONLY the prenasal-derived class is ever touched —
// community and hgm-attested rows are structurally untouchable by
// rulings. Returns true if a matching derived row was found and the
// file rewritten; false leaves the file byte-identical (the caller
// then appends a fresh approved row for a novel form, or does nothing
// on decline).
bool applyPronunciationRuling(const std::string& tsvPath,
                              const std::string& colloquial,
                              const std::string& wylie,
                              bool approve,
                              const std::string& ruler,
                              const std::string& isoDate);

}  // namespace allcore
