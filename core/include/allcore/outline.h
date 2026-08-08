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

// ---- Canonical structural units: bam po / le'u / shloka ----
// Definitions per critical-kangyur-tei-tags/segmentation-units.md (survey
// item ⑥): a shloka (tshig bcad) is 4 verse lines in verse or ~30 syllables
// in prose; a bampo (bam po) is 300 shlokas; both edition-dependent. ACIP
// canon texts write BAM PO and LE'U markers in the text itself — those
// explicit markers are AUTHORITATIVE; syllable-derived counts are ESTIMATES
// and must be labeled as such wherever displayed.

struct StructMarker {
    int tok = -1;           // token index of the marker word
    int number = 0;         // parsed ordinal; 0 = present but unparsed
    std::string label;      // ACIP text of the marker clause (raw, surfaced)
    bool irregular = false; // parsed ordinal out of sequence (review flag)
    long syllables = 0;     // bam po only: syllables from here to next marker
};

struct TextStructure {
    std::vector<StructMarker> bampos;    // explicit BAM PO markers
    std::vector<StructMarker> chapters;  // LE'U + parsed ordinal markers
    long syllables = 0;                  // syllable tokens in the document
    long preamble_syllables = 0;         // before the first BAM PO marker
    // estimates from the 30-syllable prose shloka (4 verse lines average
    // out near 30 syllables too) — display as ESTIMATE, never as count
    double shlokaEstimate() const { return syllables / 30.0; }
    double bampoEstimate() const { return syllables / 9000.0; }
};

// Scan a tokenized ACIP document (tokens + barriers from tokenizeDocument)
// for explicit structural markers and count syllables. Deterministic:
// ordinals that fail to parse are kept with number = 0 and their raw text,
// never guessed (rule 3).
TextStructure extractStructure(const std::vector<std::string>& tokens,
                               const std::vector<bool>& barrier_after);

}  // namespace allcore
