// allcore/terminology.h — terminology consistency for the draft workspace
// (docs/PEDAGOGY_ROADMAP.md, build-order item 3).
//
// Given a source ACIP passage and an English draft, extract the source's
// terms (maximal dictionary spans with binding HGM glosses) and check which
// of HGM's equivalents the draft actually uses. Rule 1 throughout: the
// checker MATCHES the draft against hgm_gloss, it never composes English.
// An unmatched term is reported as "no HGM equivalent found in the draft" —
// which may be fine (the translator chose other words); it is a review flag,
// not an error.
#pragma once

#include <string>
#include <vector>

#include "allcore/lattice.h"
#include "allcore/spine.h"

namespace allcore {

struct TermUse {
    long long entry_id = 0;
    std::string wylie;
    std::string tier;                  // curated | glossary | auto-aligned
    bool provisional = false;
    int occurrences = 0;               // times the term appears in the source
    std::vector<std::string> glosses;  // binding HGM equivalents
    std::vector<std::string> matched;  // the equivalents found in the draft
};

struct TermReport {
    std::vector<TermUse> terms;        // unmatched first, then by frequency
    // one English rendering matching two or more DIFFERENT Tibetan terms —
    // possible collapse of a distinction
    struct SharedEnglish {
        std::string english;
        std::vector<std::string> term_wylies;
    };
    std::vector<SharedEnglish> shared;
};

// A gloss "matches" when one of its alternatives (split on '/', parentheticals
// stripped, ≥3 letters) appears in the lowercased draft.
// Does any of `gloss`'s alternatives occur in the draft as a WORD?
//
// CONTRACT: `draft_lower` must ALREADY be lowercased. The alternatives are
// lowered internally, the draft is not — checkTerminology lowers once and
// calls this per term, and lowering a long draft on every call would be
// quadratic in the number of terms. The name says so; this comment says why,
// because a caller who forgets gets silent misses rather than an error.
//
// "as a WORD" is load-bearing. Until 2026-09-11 this was a bare substring
// find(), so "mind" matched "reminded", "art" matched "departure" and "one"
// matched "honest" — each reporting a term as rendered when the equivalent
// never appeared. Alphanumeric characters bound a word; an apostrophe or
// hyphen does not, so "buddha" still matches "buddha's".
bool glossMatches(const std::string& gloss, const std::string& draft_lower);

TermReport checkTerminology(const Spine& spine, const HeadwordIndex& index,
                            const std::string& source_acip,
                            const std::string& english_draft);

}  // namespace allcore
