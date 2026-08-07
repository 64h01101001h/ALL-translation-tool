#pragma once
// verbstems.h — the CC0 verbs-database reference layer.
//
// Source: tibetan-nlp/tibetan-verbs-database (CC0 1.0), banked by
// tools/import_verbs_db.py as data/extracted/verb_stems.tsv. Stems are
// Tibetan UNICODE; callers compare their own tokens through the proven
// wylieToUnicode engine, so no new converter enters the core.
//
// REFERENCE ONLY: hits are displayed as labeled comparanda ("verbs-db")
// and never drive parse decisions — those stay bound to the
// dictionary's own tense evidence (rules 1 and 3).

#include <string>
#include <unordered_map>
#include <vector>

namespace allcore {

struct VerbStemHit {
    std::string roles;      // "past" / "present,future" …
    std::string sources;    // "TDC,GT" …
    std::string paradigm;   // "pres|past|fut|imp" as published
};

class VerbStems {
public:
    // loads the TSV; returns false (empty table) if missing — callers
    // treat an absent bank as "no reference info", never an error
    bool load(const std::string& tsv_path);
    // exact-stem lookup on a Tibetan-unicode syllable (no tsheg)
    std::vector<VerbStemHit> lookup(const std::string& unicode) const;
    size_t size() const { return map_.size(); }

private:
    std::unordered_map<std::string, std::vector<VerbStemHit>> map_;
};

}  // namespace allcore
