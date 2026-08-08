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
    };
    size_t size() const { return n_; }

    // tiered for canon-scale documents: single-syllable headwords by
    // exact token (O(1)), phrases by their first TWO tokens — common
    // syllables like pa/ba no longer force thousand-candidate scans
    const long long* single(const std::string& tok) const;
    const std::vector<Cand>* pair(const std::string& t1,
                                  const std::string& t2) const;

private:
    std::unordered_map<std::string, long long> single_;
    std::unordered_map<std::string,
        std::unordered_map<std::string, std::vector<Cand>>> multi_;
    size_t n_ = 0;
};

OverlayDoc buildOverlay(const Spine& spine, const HeadwordIndex& index,
                        const std::string& acip_document,
                        int max_phrase_syllables = 14);

}  // namespace allcore
