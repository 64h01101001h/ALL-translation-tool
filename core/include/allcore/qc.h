// allcore/qc.h — machine QC over a completed analysis report (pipeline stage 3).
//
// The template makes three properties mechanically checkable; this validator
// enforces them so a hallucination cannot pass silently:
//   1. items 6/7: the English segments must reconstruct Base Translation A/B,
//      and the ACIP segments must reconstruct the source passage.
//   2. item 8: every "HGM default English" cell must quote the spine's actual
//      hgm_gloss for that term (rule 1), and a differing "Chosen English"
//      must carry an explicit DEVIATION mark.
//   3. ACIP integrity: Tibetan quoted in the structural sections (2/6/7/9/10)
//      must appear verbatim in the source passage — no silent "fixes".
#pragma once

#include <string>
#include <vector>

#include "allcore/spine.h"

namespace allcore {

struct QcFinding {
    std::string check;     // reconstruction-a | reconstruction-b | acip-coverage |
                           // hgm-anchor | deviation-mark | acip-integrity | parse
    std::string severity;  // error | warn | info
    std::string message;
};

// Validate a completed report against the spine and the source passage.
// Never throws on malformed reports — unparseable sections become findings.
std::vector<QcFinding> validateReport(const Spine& spine,
                                      const std::string& report_markdown,
                                      const std::string& source_acip);

// Render findings as a short markdown block (for CLI stderr / UI panel).
std::string findingsToMarkdown(const std::vector<QcFinding>& findings);

}  // namespace allcore
