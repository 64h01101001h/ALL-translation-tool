#pragma once
// lexicon.h — the Monlam word-list reference layer (Apache-2.0).
//
// Source: MonlamIT/Tibetan-Lexicon (license verified in-repo): plain
// word lists from the Monlam Dictionary (~107k) and Monlam Grand
// Dictionary (~367k), Tibetan Unicode, interior tshegs, no trailing
// punctuation. Only the word LISTS are open — Monlam definitions are
// not, and none are included.
//
// REFERENCE ONLY: attestation in these lists is displayed as labeled
// comparanda ("word attested in the Monlam Grand Dictionary list");
// it never drives segmentation or parse decisions.

#include <string>
#include <unordered_set>

namespace allcore {

class RefLexicon {
public:
    // each file: one unicode word per line; label names the list
    bool load(const std::string& path, const std::string& label);
    // "" = unattested; otherwise the label(s) of the attesting list(s)
    std::string attested(const std::string& unicode_word) const;
    size_t size() const { return a_.size() + b_.size(); }

private:
    std::unordered_set<std::string> a_, b_;
    std::string labelA_, labelB_;
};

}  // namespace allcore
