// allcore/shelf_suggest.h — shelf suggestions for the handoff (phase-3
// item 17). The sessions' endgame is the subject shelf ("thousands of
// categories… no one's ever done that; BDRC has none") and its risk is
// the design's driving warning: "a book on the wrong shelf is lost
// forever." So the machine NEVER places a book — it points at existing
// shelves with reasons, and the human confirms.
//
// Three signals, each carrying its evidence:
//   author  — shelves already holding works by the same author (the
//             filename's author field, dates stripped);
//   title   — shelves holding the closest titles (syllable overlap);
//   subject — shelves whose own folder names share tokens with a
//             subject string the caller supplies (from the subject
//             banks; optional).
#pragma once

#include <string>
#include <vector>

namespace allcore {

struct ShelfSuggestion {
    std::string shelf;     // folder path RELATIVE to the destination root
    double score = 0.0;
    std::vector<std::string> reasons;   // human-readable, evidence-first
};

// Rank existing shelves of dest_root for a file named `filename`
// (NUMBER_TIB_ENG_AUTHOR grammar where available; `title_hint` may add
// the text's own title-page title; `subject_hints` come from the
// subject banks). exclude_name: a file to ignore in the tree (held-out
// measurement and re-shelving both need it).
std::vector<ShelfSuggestion> suggestShelves(
    const std::string& dest_root, const std::string& filename,
    const std::string& title_hint = "",
    const std::vector<std::string>& subject_hints = {},
    const std::string& exclude_name = "", int limit = 5);

}  // namespace allcore
