// allcore/contractions.h — the bsdus-tshig (syllabic contraction) register,
// DERIVED from the HGM dictionary itself by attestation (Adam's request,
// 2026-08-07; tools/derive_bsdus_tshig.py; human review doc at
// docs/BSDUS_TSHIG_REGISTER.md).
//
// Every pair has BOTH forms as glossed dictionary entries, Wilson's
// contraction drop-pattern, and gloss agreement — never invention. This
// is what Wilson's OM dot denotes: the surviving dot inside a contracted
// form, standing where syllables were omitted (App. 2 p. 569, rnam shes
// <- rnam par shes pa). Tier: DERIVED — every in-app use says so.
#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace allcore {

struct Contraction {
    std::string shortWylie, longWylie;
    std::string dropped;     // the omitted syllables
    std::string cls;         // "clean" | "end-truncated"
    std::string glossKind;   // "exact" | "overlap"
    std::string gloss;       // the agreeing gloss evidence
    long shortN = 0, longN = 0;  // corpus counts
};

class Contractions {
public:
    // loads data/extracted/bsdus_tshig.tsv (regenerate per release)
    bool load(const std::string& tsvPath);
    size_t size() const { return entries_.size(); }

    // all registered expansions of a contracted form (wylie, exact,
    // ASCII-case-folded); empty = not in the register
    std::vector<const Contraction*> expansions(const std::string& wylie) const;

private:
    std::vector<Contraction> entries_;
    std::unordered_map<std::string, std::vector<int>> ix_;
};

}  // namespace allcore
