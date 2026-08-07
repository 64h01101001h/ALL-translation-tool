#pragma once
// quotation.h — deterministic quotation detection (APPARATUS_DESIGN §2).
//
// A passage that exactly matches a corpus segment's wylie over at least
// min_syllables consecutive syllables IS that work being quoted — attested
// by the corpus, never inferred. Below-threshold overlaps are not reported
// (rule 3: no guessing); partial similarity belongs to the concordance.

#include <string>
#include <vector>

#include "allcore/spine.h"

namespace allcore {

struct QuotationMatch {
    long long segment_id = 0;
    std::string course;          // the source work's corpus id
    int seq = 0;                 // line position within the course
    std::string matched_wylie;   // the attested span, corpus form
    std::string english;         // the segment's published English
    int start_syllable = 0;      // where the match begins in the passage
    int syllable_count = 0;      // attested length
};

// passage may be ACIP (converted via the canonical acipToEwts first) or
// wylie. Matches are non-overlapping, greedy-longest, in passage order.
std::vector<QuotationMatch> detectQuotations(const Spine& spine,
                                             const std::string& passage,
                                             bool passage_is_acip,
                                             int min_syllables = 7);

}  // namespace allcore
