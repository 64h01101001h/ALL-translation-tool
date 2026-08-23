#pragma once
// allcore/regenreg.h — approved-register regeneration (the proposal
// system's v2 item 2, Adam's design 2026-08-08, built 2026-08-14):
// the canonical register files are RE-DERIVED from the ruling store
// rather than accumulating live appends. Idempotent by construction:
// rows this tool previously folded in (marked "# approved by") are
// stripped and re-written from the store's CURRENTLY approved
// proposals — so a later decline falls out on the next run, running
// twice changes nothing, and a freshly imported data release
// re-folds the whole approved layer with one call.
//
// Boundary honored: only the app-native register files are touched
// (honorifics, colloquial pronunciation, approved abbreviations).
// Dictionary/corpus approvals still leave only via the signed
// export — the corpus keeps its single pipeline.
#include <string>
#include <vector>

#include "allcore/proposals.h"

namespace allcore {

struct RegenStats {
    int honorific = 0;  // rows folded into honorific_register.tsv
    int pron = 0;       // pronunciation rulings applied or folded
    int abbrev = 0;     // rows in approved_abbreviations.tsv
    // Review finding, 2026-08-23: writeLines() was made honest about a
    // failed write and every one of its four call sites here threw the
    // answer away, so ApprovalPane could still print "N proposal(s)
    // approved and stamped" over registers that never reached disk —
    // the same defect FAIL-2 fixed one layer up, reintroduced by the
    // fix itself. The struct now carries the failure, and the caller
    // must look at it (house rule 4).
    std::vector<std::string> unwritten;   // paths that did NOT land
    bool ok() const { return unwritten.empty(); }
};

RegenStats regenerateApprovedRegisters(const ProposalStore& store,
                                       const std::string& dataRoot);

}  // namespace allcore
