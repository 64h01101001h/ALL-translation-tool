// allcore/provenance.h — what a measurement is allowed to say about
// itself. Created 2026-09-09 by the analysis-suite plan's F0.
//
// Every engine in the analysis suite hands back numbers. A number with
// no account of where it came from is not evidence, it is a claim; so
// each result carries one of these, and every saved report and every
// cache row is stamped with the block below. The two counts that matter
// most are units_read and units_refused: a run that could not read part
// of its input says so in the header, on screen, rather than quietly
// measuring a smaller text.
#pragma once

#include <string>
#include <vector>

namespace allcore {

struct Provenance {
    std::string engine;              // "passages/1", "textstats/1", …
    std::string built_utc;           // when the measurement was taken
    std::string source, source_sha256;
    std::string script_in;           // acip | wylie | unicode | mixed
    std::string norm;                // "verbatim" | "search-fold/<generation>"
    std::string apparatus;           // "strip-span" | "keep-inner" | "as-written"
    long long units_read = 0;        // syllables actually counted
    long long units_refused = 0;
    int apparatus_marks_stripped = 0;
    // Line-numbered and human-readable: everything that could not be read
    // as intended. It is a SUPERSET of units_refused — a syllable refused
    // for an intra-syllable mark appears here and is counted there, while
    // a line whose brackets do not close appears here only, because no
    // syllable was dropped: the line was read exactly as written and the
    // reader is being told so.
    std::vector<std::string> refusals;
};

// The report header text: one field per line, the refusal list last.
// An empty text field prints "not recorded" — never a fabricated value —
// and an empty refusal list prints "none", because "no refusals" is a
// result worth stating out loud.
std::string provenanceBlock(const Provenance& p);

}  // namespace allcore
