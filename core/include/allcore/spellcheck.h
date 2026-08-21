// allcore/spellcheck.h — syllable-legality checking (roadmap item 5) from the
// tibetan-spellchecker CC0 data (Elie Roux): onset stacks classed A/NB, class
// suffix sets, plus listed rare/wasur/proper-name/exception syllables.
//
// A syllable is legal iff it is a listed full syllable (optionally + its
// class's suffixes) or stack+suffix with suffix in the stack's class set.
// NB is implemented as A minus the བ/བས suffix forms (the "no ba" class);
// this reading is validated empirically in spellcheck_smoke against the
// 26,318 source-attested ground-truth entries.
#pragma once

#include <map>
#include <set>
#include <string>
#include <vector>

namespace allcore {

class SyllableChecker {
public:
    // data_dir must contain root.txt, rare.txt, wasurs.txt, proper-names.txt,
    // exceptions.txt, suffixes.json. Throws std::runtime_error when missing.
    explicit SyllableChecker(const std::string& data_dir);

    // syl: one Tibetan-script syllable, no tsheg. UTF-8.
    bool legalUnicode(const std::string& syl) const;

    // wylie syllable → engine conversion → legality; unconvertible = illegal.
    bool legalWylie(const std::string& wylie_syl) const;

    size_t stackCount() const { return bases_.size(); }

private:
    // base (stack or listed syllable) → class name ("A"/"NB"/"C"/"" = exact)
    std::vector<std::pair<std::string, std::string>> bases_;
    std::map<std::string, std::set<std::string>> classes_;
};

}  // namespace allcore
