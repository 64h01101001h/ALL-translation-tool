// allcore/textspan.h — the ONE definition of the small spans every text
// tool in the app keeps re-describing: the ACIP folio marker and the
// editorial apparatus brackets. Created 2026-09-09 by the analysis-suite
// plan's F0 (the critic found the folio marker defined three ways in the
// core and four in the app; each is now a call into here).
#pragma once

#include <string>
#include <vector>

namespace allcore {
namespace textspan {

// The folio marker as the input centres write it: "@012B", "@12b",
// "@0012", "@1A". Digits 1–4, optional side letter, and NOT followed by
// another letter or digit (so "@012BX" and "@S0134" are not markers).
// One pattern string, usable by std::regex and by Qt's QRegularExpression.
inline const char* kFolioMarkerPattern = "@([0-9]{1,4}[AaBb]?)(?![A-Za-z0-9])";

struct FolioMarker {
    int begin = 0, end = 0;   // byte offsets in the line, [begin, end)
    std::string id;           // uppercased, e.g. "012B", "0012", "1A"
};
// Every marker on the line, in order.
std::vector<FolioMarker> folioMarkers(const std::string& line);
// -1 when the line carries no marker; otherwise the byte offset just past
// the FIRST marker, with its id written to folioOut (may be null).
int folioMarkerEnd(const std::string& line, std::string* folioOut);
// The id of the LAST marker on the line, or "" (what a citation wants).
std::string lastFolio(const std::string& line);
// Split an id into its number and side letter: "012B" → 12, 'B'; "0012" →
// 12, '\0'. Returns false for an empty id.
bool folioParts(const std::string& id, int& number, char& side, int& digitWidth);

// Editorial apparatus: [operator remark] and {note / correction} spans.
// `balanced` is false when a bracket opens and never closes (or closes
// unopened) — callers that strip apparatus must then REFUSE the line
// rather than guess (the honesty rule; batch 4 F3 depends on it).
struct Span { int begin = 0, end = 0; char kind = '['; };   // kind '[' or '{'
std::vector<Span> apparatusSpans(const std::string& line, bool* balanced = nullptr);
// The line with its apparatus spans removed and whitespace collapsed to
// single spaces; unchanged copy (and balanced=false) when unbalanced.
std::string stripApparatus(const std::string& line, bool* balanced = nullptr);

}  // namespace textspan
}  // namespace allcore
