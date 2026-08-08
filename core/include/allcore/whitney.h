// allcore/whitney.h — William Dwight Whitney reference layer (Adam's
// request, 2026-08-08): the 937 roots of "The Roots, Verb-Forms and
// Primary Derivatives of the Sanskrit Language" (1885) with each
// root's section citations into the "Sanskrit Grammar" (1879). Both
// works public domain (Whitney d. 1894); digitization
// github.com/gasyoun/WhitneyRoots, Apache-2.0 (data/whitney/).
//
// REFERENCE ONLY (rule 1): comparanda beside our Sanskrit chain, never
// promoted into HGM equivalents. The class column is form-level
// digitization data (homonyms written identically share it) and the UI
// must label it so; principal-part forms were NOT banked (corrupted
// column in the source — see tools/build_whitney.py).
#pragma once

#include <map>
#include <string>
#include <vector>

namespace allcore {

struct WhitneyRoot {
    int id = 0;
    std::string root;           // IAST
    std::string homonym;        // "1"/"2"/… or empty
    std::string meaning;        // Whitney's gloss (verbatim incl. his (?))
    std::string classes;        // PER-HOMONYM, from the machine hub
                                // (verified: 1 kṛ I|II|V|VIII, 2 kṛ VI)
    std::string classUncertain; // additional candidate classes
    std::string ppp;            // past passive participle, accented (kṛtá)
    std::string grammarSecs;    // §§ into the 1879 Grammar (✦ specific,
                                // ⚠ exception markers preserved)
    std::string dcsClasses;     // DCS corpus classes (comparanda)
    std::string mwId;           // Monier-Williams entry id (crosswalk)
    std::string senses;         // MW/Apte sense citations
    std::string notes;
    std::string slp1;           // SLP1 form (Cologne deep-link key)
    std::string sectionRefs;    // topical Grammar ranges
                                // (perfect:781-823|aor_is:898-911|…)
};

class WhitneyRoots {
public:
    bool load(const std::string& tsvPath);
    size_t size() const { return entries_.size(); }

    // exact-IAST or diacritic-folded lookup (bhū and bhu both match)
    std::vector<const WhitneyRoot*> byRoot(const std::string& iast) const;
    // English meaning word search (whole-word, case-folded)
    std::vector<const WhitneyRoot*> byMeaning(const std::string& word,
                                              int limit = 12) const;
    // past-participle reverse lookup (gata / gatá -> gam)
    std::vector<const WhitneyRoot*> byPpp(const std::string& form) const;

private:
    std::vector<WhitneyRoot> entries_;
    std::map<std::string, std::vector<int>> byFolded_;
    std::map<std::string, std::vector<int>> byPpp_;
};

// IAST → plain-ASCII fold (ā→a, ṛ/ṝ→r, ś/ṣ→s, ṃ→m, ḥ→h, ṅ/ñ/ṇ→n, …)
std::string foldIast(const std::string& s);

}  // namespace allcore
