// Turning ACIP into something a reader may look at.
//
// ACIP source carries editorial markup that is not Tibetan — correction braces
// {%NGUR}, parentheses (GNAS BRTAN), brackets, page marks. Handed whole to the
// script converter it fails, and the naive response is to print the raw ACIP
// inline, which is how "(RNAMS" and "(GNAS BRTAN)" came to sit among Tibetan
// options in the Drills pane (Adam, 2026-09-09).
//
// This decomposes the text once. Callers decide how to present each piece: the
// desktop pane colours them, the drill-pack generator writes them into a file
// for a phone that has no converter of its own. One implementation, so the two
// can never disagree about what a reader sees.
#pragma once
#include <string>
#include <vector>

namespace allcore {

struct DisplayPiece {
    enum Kind {
        Script,   // converted Tibetan, tsheg already appended
        Markup,   // editorial marks, shown as what they are, never as script
        Failed    // would not convert; must be flagged, never passed off
    };
    Kind kind;
    std::string text;
};

std::vector<DisplayPiece> acipDisplayPieces(const std::string& acip);

// Plain text for a surface that cannot style: script as-is, markup as-is, and
// anything that would not convert wrapped in ⟨ ⟩ so it can never be mistaken
// for Tibetan.
std::string acipToTibetanPlain(const std::string& acip);

// Editorial markup removed entirely. For a DRILL this is not cosmetic: Adam
// noticed on 2026-09-09 that an option beginning "(" against a segment ending
// ")" tells the learner which option is the answer before they have read a
// word of Tibetan. Markup is not Tibetan, it is not part of the answer, and in
// a drill it is a tell. Strip it from both sides and the tell is gone.
std::string acipStripMarkup(const std::string& acip);

}  // namespace allcore
