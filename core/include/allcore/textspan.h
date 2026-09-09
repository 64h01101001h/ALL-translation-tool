// allcore/textspan.h — the ONE definition of the small spans every text
// tool in the app keeps re-describing: the ACIP folio marker and the
// editorial apparatus brackets. Created 2026-09-09 by the analysis-suite
// plan's F0 (the critic found the folio marker defined three ways in the
// core and four in the app; each is now a call into here).
#pragma once

#include <cstddef>
#include <string>
#include <utility>
#include <vector>

#include "allcore/provenance.h"

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
struct Span { int begin = 0, end = 0; char kind = '['; };   // kind '[', '{' or '<'
// `angleBrackets` additionally reads ⟨…⟩ (U+27E8/U+27E9, kind '<'). It is
// OFF by default because textnorm's Normalize… and the compare pane were
// written against the two ASCII kinds and their behaviour must not shift
// under them; the syllable tokenizer below asks for it.
std::vector<Span> apparatusSpans(const std::string& line, bool* balanced = nullptr,
                                 bool angleBrackets = false);
// The line with its apparatus spans removed and whitespace collapsed to
// single spaces; unchanged copy (and balanced=false) when unbalanced.
std::string stripApparatus(const std::string& line, bool* balanced = nullptr);

// ------------------------------------------------------------ syllables
// The ONE wylie syllable tokenizer. Promoted verbatim (F0) from the
// file-private copy in quotation.cpp so that quotation detection, the
// study engines and anything else that counts syllables cannot drift
// apart. Its rule: split on whitespace and / | , ; · strip non-alphanumeric
// characters off both EDGES except the apostrophe · keep ' + . ~ inside ·
// fold A–Z to lower case · drop tokens that are all digits.
//
// KNOWN, DELIBERATELY PRESERVED: the marker test (`s[0] != '@'`) runs
// AFTER the edge strip has already removed the '@', so "@012a" survives
// as the token "012a" while a digits-only "@123" is dropped by the
// all-digit rule. That is what quotation detection has always done and
// what its battery pins; it is recorded here rather than silently
// "fixed", because changing it changes recall. Callers that must not see
// folio markers should strip them first (folioMarkers, above).
std::vector<std::string> syllablesWylie(const std::string& wylie);

// What to do with editorial apparatus before the text is split. The mode
// is a disclosed choice: it is recorded in Provenance.apparatus and
// printed in every saved report header, never applied silently.
enum class Apparatus {
    StripSpan,   // remove whole [ ], { }, ⟨ ⟩ spans before splitting (default)
    KeepInner,   // remove only the delimiters, keep the supplied letters
    AsWritten    // touch nothing; count the transcription as it stands
};
const char* apparatusName(Apparatus a);   // "strip-span" | "keep-inner" | "as-written"

struct Syllable {
    std::string text;                 // the tokenizer's normalized form
    size_t begByte = 0, endByte = 0;  // half-open, into the ORIGINAL text
    int line = 1;                     // 1-based
    bool barrierAfter = false;        // flushed by , ; / | or a line break
};

// syllablesWylie's loop, carrying byte offsets, line numbers and the
// barrier flag, plus the apparatus policy.
//
// Under StripSpan a mark whose span boundary falls INSIDE a syllable
// ("cho[s]") is never truncated to "cho": the whole syllable is REFUSED —
// counted in Provenance.units_refused and listed by line with its raw
// form. `( )` is not house apparatus in ACIP, so it is retained as text
// under every mode (countParenSpans reports how many were kept, so the
// choice is visible in the report).
//
// `out` accumulates: units_read, units_refused, apparatus_marks_stripped
// and refusals are ADDED to whatever the Provenance already carries, so
// one Provenance can travel across a whole run of texts. `apparatus` is
// set to the mode used.
//
// Byte offsets: under StripSpan and AsWritten, text.substr(begByte,
// endByte-begByte) lower-cased and edge-stripped is exactly `.text`.
// Under KeepInner the span runs from the first to the last byte the token
// actually kept, so an interior delimiter is inside it and an edge one is
// not ("cho[s]" → text "chos", bytes [0,5), the ']' left outside) — right
// for highlighting, not for a substring match.
std::vector<Syllable> syllableSpans(const std::string& wylie,
                                    Apparatus mode = Apparatus::StripSpan,
                                    Provenance* out = nullptr);

// Balanced ( … ) spans kept as text by syllableSpans, for disclosure.
int countParenSpans(const std::string& text);

// ------------------------------------------------------------- script
// One line to EWTS wylie through the canonical converters only (rule 2):
// Tibetan unicode → unicodeToWylie, with ok=false when it warned (the
// line is then returned unchanged, never a guessed conversion);
// all-upper-with-no-lower → acipToEwts; anything else returned as it is.
// Promoted from textdiff.cpp's file-static `toEwts`, which now calls it.
std::string toEwtsLine(const std::string& line, bool* ok = nullptr);

struct Converted {
    std::string wylie;
    int unconvertedLines = 0;
    std::vector<int> unconvertedLineNumbers;   // 1-based
};
// Every line through toEwtsLine. A line that would not convert is kept
// as it stands and LISTED: callers must never shingle, count or cite
// such a line as converted, and must disclose the count.
Converted toEwtsDocument(const std::string& text);

// --------------------------------------------------------------- folios
struct FolioLoc {
    std::string folio;        // "012B"; empty when no strict marker precedes
    int lineInFolio = 0;      // 1-based within the folio — or, when `folio`
                              // is empty, the absolute 1-based line
    int linesInFolio = 0;     // lines from the marker up to the next one
};

// TWO folio functions, deliberately, and the difference is pinned by
// textspan_smoke T9 rather than left to drift:
//
//   folioAt  is STRICT — only "@NNN" WITH an A/B side letter is a folio.
//            New code uses this; it is the form catalog_id, catalog_list
//            and the Overlay all require.
//   citeAt   is LENIENT — kFolioMarkerPattern, side letter optional, which
//            is what textdiff's apparatus output has always printed. It is
//            kept so that output does not change.
//
// They agree on every well-formed marker. On a malformed "@012" (no side)
// citeAt says "@012.3" and folioAt says "line 3".
FolioLoc folioAt(const std::string& text, size_t byteOffset);
std::string citeAt(const std::string& text, size_t byteOffset);
// The same walk over already-split lines (textdiff's citeFor is this).
std::string citeAtLine(const std::vector<std::string>& lines, int line0);

// First and last "@NNN[AB]" folio marks in a body — the ACIP catalogue's
// own rule, promoted from catalog_list.cpp: an optional space after the
// '@', digits, then a REQUIRED A/B side letter, uppercased on the way out.
std::pair<std::string, std::string> folioRange(const std::string& text);

// Document Properties' own counts, replicated exactly so Properties and
// the study surfaces can never disagree: markers by kFolioMarkerPattern,
// shads by counting ',' in the body left after @-tokens and [ ]/{ } spans
// are blanked (docprops' markRe — deliberately NOT the apparatusSpans
// rule, because parity with what the user already sees is the point).
int countFolioMarkers(const std::string& text);
int countShads(const std::string& text);

}  // namespace textspan
}  // namespace allcore
