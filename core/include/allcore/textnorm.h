// allcore/textnorm.h — the Normalize engine (2026-09-09, analysis-suite F3).
// The mechanical clean-ups an input-centre file needs — collapse doubled
// spacing, trim trailing blanks, reduce blank-line runs, respell the double
// shad to one house form, space the shads, drop [ ] / { } apparatus, drop
// folio markers, fix the line endings — computed as data so the UI can show
// the diff BEFORE anything is written. Qt-free; the app supplies the text and
// applies the result.
//
// Honesty rules this engine enforces (they are the reason it exists):
//  - PER LINE ONLY. A rule never joins two lines and never splits one, so an
//    ACIP syllable running onto the next line cannot be silently fused. The
//    only rule that changes the line COUNT is the blank-line run rule.
//  - A REFUSED LINE IS BYTE-IDENTICAL. When a rule cannot see what it is
//    looking at — an apparatus bracket that never closes, a folio marker glued
//    to text, a line carrying both ACIP and English — the line is copied
//    through unchanged and listed in `refused` with its 0-based INPUT line
//    number and a plain reason. Nothing is half-normalized in silence.
//  - NO DEFAULT DOUBLE-SHAD FORM. `doubleShad` ships empty and the shad
//    spacing ships Keep. The GMR / input-centre formatting sheet has not been
//    supplied; shipping a guess would re-spell every shad in a volume.
//  - THE TSHEG U+0F0B IS NEVER TOUCHED. It is a letter-level separator owned
//    by the converters, not punctuation and not whitespace.
//  - NO SCRIPT CONVERSION, no case folding, and a single shad is never merged
//    into a double. A Tibetan double-shad form is only ever written onto a
//    Tibetan Unicode line, an ACIP form only onto an ACIP line.
//  - Every rule is a fixed point of itself: normalizing twice equals
//    normalizing once (pinned by textnorm_smoke).
#pragma once

#include <string>
#include <vector>

namespace allcore::textnorm {

enum class Eol { Keep, LF, CRLF, CR };
enum class ShadSpace { Keep, None, One };
// How a line is read for the shad rules. Tibetan Unicode wins on sight of one
// U+0F00–U+0FFF codepoint; otherwise the case of the WORDS decides: all-caps
// words and no lower-case word is ACIP; both together is Mixed (refused);
// anything else — English, Wylie, digits — is Other and no shad rule runs.
enum class LineClass { Acip, TibetanUnicode, Other, Mixed };

struct Options {
    bool collapseSpaces = false, trimTrailing = false;
    int  maxBlankLines = -1;                 // -1 = keep
    std::string doubleShad;                  // "" = leave; ",," | ", ," | "།།" | "༎"
    ShadSpace spaceBeforeShad = ShadSpace::Keep;
    bool spaceAfterShad = false;
    bool stripApparatus = false, stripFolioMarkers = false, ensureFinalNewline = false;
    Eol eol = Eol::Keep;
};
struct Refusal { int line; std::string reason; };   // 0-based line
struct Result {
    std::vector<std::string> lines;
    std::string eol;                          // the output EOL ("" unchanged rules)
    int changedLines = 0, shadRulesSkipped = 0;
    bool finalNewlineAdded = false;
    std::vector<Refusal> refused;             // left byte-identical
    std::vector<std::string> notes;
};

// The classifier above, stated as a function so the UI can label a line.
LineClass classifyLine(const std::string& line);
// Every `open` closed by a `close`, none closing unopened. Cheap counter — it
// says nothing about nesting or interleaving; normalizeLineText asks
// textspan::apparatusSpans for that.
bool bracketsBalanced(const std::string& line, char open, char close);
// "\r\n", "\n", "\r" from the FIRST terminator in the text, or "" when the
// text carries no line break at all (nothing to keep).
std::string detectEol(const std::string& text);
// One line through the fixed order: classify → apparatus → folio markers →
// shad rules → collapse spaces → trim trailing. On refusal the ORIGINAL line
// is returned and *refuseReason is set; `cls` (may be null) always receives
// the classification.
std::string normalizeLineText(const std::string& line, const Options& o,
                              LineClass* cls, std::string* refuseReason);
// The whole text. `lines` is splitLines() output — the terminator is not part
// of a line — and `detectedEol` is what detectEol() said about the text those
// lines came from (it is what Eol::Keep keeps).
Result normalizeText(const std::vector<std::string>& lines,
                     const std::string& detectedEol, const Options& o);

}  // namespace allcore::textnorm
