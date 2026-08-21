// allcore/title_xlat.h — the title translator's workbench (queue item 5,
// Adam's ask 2026-08-19).
//
// RULE 1 SHAPES EVERYTHING HERE: the machine may MATCH the published
// English, never COMPOSE it. So this is not a translator — it is an
// attestation engine. Given a Tibetan title it answers two questions:
//
//   1. Has this (or a near-identical) title already been rendered? —
//      whole-title matches with the published English, scored.
//   2. For each PHRASE of the title, where has that phrase appeared in
//      other published title pairs? — fragment cards, longest match
//      first, each carrying up to a few full (Tibetan -> English) pairs
//      as evidence. No word alignment is invented: the card shows the
//      whole pair and the human reads where the phrase landed.
//
// Syllables no fragment covers are listed UNCOVERED — a real answer,
// not a gap to paper over. The human composes; the machine attests.
//
// Precedent: Jamie Plant's title glossary (session 2), built for title-
// translation consistency — "if you want to know how a title was
// translated in the past, so you can keep it consistent."
#pragma once

#include <string>
#include <vector>

namespace allcore {

struct TitlePair {
    std::string tib_norm;   // normalized (normalizeAcipTitle)
    std::string tib_raw;    // as filed
    std::string eng;        // the published English rendering
    std::string source;     // where the pair comes from
};

class TitlePairBank {
public:
    // Normalizes; skips titles under 2 syllables or empty English.
    bool add(const std::string& tib_raw, const std::string& eng,
             const std::string& source);
    // Bank every "NUMBER_TIBETAN_ENGLISH_…" filename in a library tree
    // (both fields must be present; META companions skipped). Returns
    // the number banked.
    int addLibraryTree(const std::string& root);
    size_t size() const { return entries_.size(); }
    const std::vector<TitlePair>& entries() const { return entries_; }

private:
    std::vector<TitlePair> entries_;
};

struct WholeTitleMatch {
    std::string tib_raw, eng, source;
    double score = 0.0;   // shared syllables / longer title
};

struct FragmentCard {
    std::string fragment;      // the attested span, normalized
    size_t from = 0, count = 0;   // syllable positions in the query
    struct Example {
        std::string tib_raw, eng, source;
    };
    std::vector<Example> examples;   // up to max_examples full pairs
};

struct TitleWorkbench {
    std::vector<WholeTitleMatch> whole;   // best first, floor-gated
    std::vector<FragmentCard> fragments;  // greedy, longest-first cover
    std::vector<std::string> uncovered;   // syllables with no attestation
    double coverage = 0.0;                // covered syllables / total
};

// Build the workbench. Fragments are contiguous syllable spans of the
// query (2..max_span syllables) that appear as contiguous spans in some
// banked title; greedy longest-match-first cover of the query.
TitleWorkbench buildTitleWorkbench(const std::string& tibetan_title,
                                   const TitlePairBank& bank,
                                   int max_whole = 3,
                                   double whole_floor = 0.5,
                                   int max_span = 8,
                                   int max_examples = 3);

}  // namespace allcore
