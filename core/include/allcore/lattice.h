// allcore/lattice.h — the overlay pane's segmentation lattice (roadmap item 6,
// docs/OVERLAY_PANE_DESIGN.md).
//
// Precomputed at document load, never per-hover: every dictionary span in the
// document is found once (greedy longest-match per position, nested shorter
// matches kept), with nesting depth for the shading and punctuation barriers
// capping spans exactly as the Hypercontext tool's hard barriers did.
#pragma once

#include <map>
#include <unordered_map>
#include <string>
#include <vector>

#include "allcore/spine.h"

namespace allcore {

struct OverlaySpan {
    int beg = 0;                 // token span [beg, end)
    int end = 0;
    int entry_ix = -1;           // index into OverlayDoc::entries
    int depth = 0;               // number of strictly containing spans
    // non-empty = the span matched through an a-chung clitic on its last
    // syllable (Wilson particle layer): the clitic ("'i","'o","'u","'am","'ang")
    // was split off before lookup (PA'I → PA + 'i)
    std::string clitic;
};

struct OverlayDoc {
    std::vector<std::string> tokens;   // normalized ACIP syllables, in order
    std::vector<bool> barrier_after;   // true = shad/comma boundary follows token i
    std::vector<Entry> entries;        // deduplicated matched entries
    std::vector<OverlaySpan> spans;    // sorted by (beg asc, length desc)

    // indices of spans covering token `tok`, innermost (shortest) first
    std::vector<int> spansAt(int tok) const;

    // The best span among `candidates`, by the project's one ranking:
    //
    //   1. a span carrying an hgm_gloss beats one carrying none
    //   2. among glossed spans, a GMR tier (curated/glossary) beats an
    //      auto-aligned one -- rule 1, his English outranks the machine's
    //      match even when the machine's match covers more of the line
    //   3. and only then, longer beats shorter
    //
    // Returns -1 when nothing qualifies.
    //
    // This exists because the rule was written out by hand in five places and
    // four of them stopped at step 3 -- walk::bestGlossSpan, the DraftPane
    // scaffold chip and its anchor list, wilsonParse's unit picker, and the
    // Trainer's vocabulary layer, which did not even reach step 1 and printed
    // "no Geshe Michael Roach equivalent" over words he had glossed. Each was
    // found separately, on 2026-09-13, and each could have been the last one.
    // A sixth copy should call this instead.
    int bestSpan(const std::vector<int>& candidates) const;
    // per-token cover count (capped at `cap`) — drives the depth wash
    std::vector<int> coverDepth(int cap = 3) const;
};

// Tokenize a document keeping barrier information: ',' ';' '/' and line breaks
// mark boundaries no span may cross. Folio markers [NNN] and curly quotes are
// normalized away as in tokenizeAcip.
void tokenizeDocument(const std::string& raw, std::vector<std::string>& tokens,
                      std::vector<bool>& barrier_after);

// Build the full lattice for a document.
OverlayDoc buildOverlay(const Spine& spine, const std::string& acip_document,
                        int max_phrase_syllables = 14);

// In-memory first-syllable index over the ACIP headwords (~106k entries,
// built once in ~0.2s). buildOverlay with an index is ~10x faster than the
// per-position SQL probing and produces the IDENTICAL span set (asserted in
// lattice_smoke).
class HeadwordIndex {
public:
    explicit HeadwordIndex(const Spine& spine);
    struct Cand {
        std::vector<std::string> tokens;
        long long entry_id;
        // Spine::Headword::tier_rank. Two entries can share one headword; rule
        // 1 says Geshe Michael's glossary entry wins over the machine's
        // auto-aligned one, and the index must apply that itself.
        int tier_rank = 2;
    };
    size_t size() const { return n_; }

    // tiered for canon-scale documents: single-syllable headwords by
    // exact token (O(1)), phrases by their first TWO tokens — common
    // syllables like pa/ba no longer force thousand-candidate scans
    const long long* single(const std::string& tok) const;
    const std::vector<Cand>* pair(const std::string& t1,
                                  const std::string& t2) const;

private:
    // id + tier_rank: on a duplicate headword the better tier is kept.
    std::unordered_map<std::string, std::pair<long long, int>> single_;
    std::unordered_map<std::string,
        std::unordered_map<std::string, std::vector<Cand>>> multi_;
    size_t n_ = 0;
};

OverlayDoc buildOverlay(const Spine& spine, const HeadwordIndex& index,
                        const std::string& acip_document,
                        int max_phrase_syllables = 14);

}  // namespace allcore
