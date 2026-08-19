// allcore/abbr.h — Tibetan orthographic abbreviations (bskungs yig) from
// the ERC TibSchol table (CC0 1.0; provenance in data/abbreviations/
// README.md; the project asks for a reference to its repository and to
// rKTs, honored in the UI label).
//
// Display-only reference layer: a token that matches a known abbreviation
// (by EWTS or by Unicode rendering) shows its expansion — the companion
// to the syllable-legality red wave, which such forms trigger by design.
// Never auto-expanded, never corpus-bound.
#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace allcore {

struct AbbrEntry {
    std::string abbrWylie, abbrUnicode;   // the squeezed form (EWTS/Unicode)
    std::string expWylie, expUnicode;     // the expansion
    std::string src;   // attesting table, e.g. "rKTs (CC BY 4.0):
                       // BabelStone" — empty = the loading file's
                       // default attribution (TibSchol)
};

class AbbrTable {
public:
    bool load(const std::string& csvPath);
    size_t size() const { return entries_.size(); }

    // exact lookups (trailing tsheg/space-insensitive; EWTS lookup is
    // ASCII-case-folded). Empty = not a known abbreviation.
    std::vector<const AbbrEntry*> byWylie(const std::string& ewts) const;
    std::vector<const AbbrEntry*> byUnicode(const std::string& uni) const;

private:
    std::vector<AbbrEntry> entries_;
    std::unordered_map<std::string, std::vector<int>> wylieIx_, uniIx_;
};

}  // namespace allcore
