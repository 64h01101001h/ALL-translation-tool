// allcore/poslex.h — the SOAS Classical Tibetan POS lexicon (CC BY 4.0;
// provenance + license in data/soas_pos/README.md). 15,642 hand-tagged
// forms from the Tibetan in Digital Communication project.
//
// This is the "real POS source" the Wilson dots program required: the
// NOM vs NA/NN/APP decision between two bare nominals is resolvable only
// with part-of-speech DATA (scope directive: never heuristics). Lookups
// are exact on the tsheg-stripped form; a word's tags are the UNION of
// all its source rows, so ambiguity is preserved, and only unambiguous
// tags ever decide anything.
#pragma once

#include <set>
#include <string>
#include <unordered_map>
#include <vector>

namespace allcore {

class PosLexicon {
public:
    bool load(const std::string& path);
    size_t size() const { return map_.size(); }

    // sorted unique tags; empty = word not in the lexicon
    std::vector<std::string> tags(const std::string& unicodeWord) const;
    // tags == {"adj"} exactly
    bool unambiguousAdj(const std::string& unicodeWord) const;
    // non-empty and every tag is a noun tag (n.count / n.mass / n.prop)
    bool unambiguousNoun(const std::string& unicodeWord) const;

private:
    const std::set<std::string>* find(const std::string& w) const;
    std::unordered_map<std::string, std::set<std::string>> map_;
};

}  // namespace allcore
