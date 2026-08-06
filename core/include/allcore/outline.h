// allcore/outline.h — sa bcad (topical outline) extraction
// (docs/PEDAGOGY_ROADMAP.md, build-order item 4).
//
// Tibetan commentaries announce their structure explicitly: "X la gsum ste"
// ("X has three parts:") and then open each part with an ordinal — "dang po
// ni …", "gnyis pa la gnyis te …" (which both opens part two AND announces
// its two sub-parts). This module detects those markers deterministically
// and rebuilds the topic tree. Ordinals that arrive out of sequence are
// attached at the best matching level and FLAGGED irregular, never silently
// reshuffled (rule 3).
#pragma once

#include <string>
#include <vector>

namespace allcore {

struct OutlineNode {
    std::string label;                // "2.1" style dotted path ("" = root)
    std::string heading;              // ACIP text of the opening clause
    int announced = 0;                // children announced by "la N ste"
    int tok = -1;                     // token index where the node opens
    bool irregular = false;           // ordinal out of sequence (review flag)
    std::vector<OutlineNode> children;
};

// Extract the outline from a tokenized ACIP document (tokens + barriers from
// tokenizeDocument). The returned root's children are the top-level parts.
OutlineNode extractOutline(const std::vector<std::string>& tokens,
                           const std::vector<bool>& barrier_after);

}  // namespace allcore
