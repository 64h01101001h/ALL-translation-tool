// allcore/mvp.h — the Mahāvyutpatti reference table (DILA/DDBC TEI edition,
// CC BY-SA 3.0; provenance + license in data/mahavyutpatti/README.md).
//
// The classical 9th-century Skt⇄Tib glossary: 9,379 entries keyed by the
// standard Mahāvyutpatti numbers, with IAST + Devanāgarī Sanskrit, Wylie +
// Tibetan-script Tibetan, and Chinese. REFERENCE ONLY in the app (labeled
// "Mahāvyutpatti (DILA TEI, CC BY-SA 3.0)") — never HGM-tier. Lookups are
// exact (case-insensitive); nothing is stemmed or guessed.
#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace allcore {

struct MvpEntry {
    int key = 0;               // the standard Mahāvyutpatti number
    std::string chapter;       // classical chapter heading (Chinese)
    std::string iast, deva;    // Sanskrit
    std::vector<std::string> wylie, tibetan, chinese;  // source order;
                               // Chinese may carry {ddbc} editorial markers
};

class Mvp {
public:
    // loads data/extracted/mahavyutpatti.tsv (tools/extract_mahavyutpatti.py)
    bool load(const std::string& tsvPath);
    size_t size() const { return entries_.size(); }

    std::vector<const MvpEntry*> byWylie(const std::string& wylie) const;
    std::vector<const MvpEntry*> byIast(const std::string& iast) const;
    const MvpEntry* byKey(int key) const;

private:
    std::vector<MvpEntry> entries_;
    std::unordered_map<std::string, std::vector<int>> wylieIx_, iastIx_;
    std::unordered_map<int, int> keyIx_;
};

}  // namespace allcore
