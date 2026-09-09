// provenance.cpp — see allcore/provenance.h.
#include "allcore/provenance.h"

namespace allcore {

namespace {
// Never print an empty value as if it were one: an unset field is
// reported as unset.
std::string said(const std::string& s) { return s.empty() ? "not recorded" : s; }
}  // namespace

std::string provenanceBlock(const Provenance& p) {
    std::string o;
    o += "engine: " + said(p.engine) + "\n";
    o += "built (UTC): " + said(p.built_utc) + "\n";
    o += "source: " + said(p.source) + "\n";
    o += "source sha256: " + said(p.source_sha256) + "\n";
    o += "script in: " + said(p.script_in) + "\n";
    o += "normalization: " + said(p.norm) + "\n";
    o += "apparatus: " + said(p.apparatus) + "\n";
    o += "syllables read: " + std::to_string(p.units_read) + "\n";
    o += "syllables refused: " + std::to_string(p.units_refused) + "\n";
    o += "apparatus marks stripped: " +
         std::to_string(p.apparatus_marks_stripped) + "\n";
    if (p.refusals.empty()) {
        o += "refusals: none\n";
    } else {
        o += "refusals:\n";
        for (const auto& r : p.refusals) o += "  " + r + "\n";
    }
    return o;
}

}  // namespace allcore
