// allcore/textdiff.h — the compare & merge engine (2026-09-08).
//
// Line diff (Myers O(ND), linear space), in-line diff at Tibetan-syllable /
// word / character granularity, diff3-style three-way merge, and the
// report writers (unified patch, HTML side-by-side, apparatus criticus).
// Pure C++: no Qt, no product code — implemented from the published
// algorithms. Tibetan awareness comes from the canonical converters in
// engines.h / unicode_wylie.h: a line that does not convert cleanly is
// compared raw and reported as such (never guessed).
#pragma once

#include <string>
#include <utility>
#include <vector>

namespace allcore {
namespace textdiff {

struct Options {
    bool ignoreCase = false;
    bool ignoreWhitespace = false;        // drop all whitespace before comparing
    bool ignoreWhitespaceChange = false;  // runs of whitespace compare equal
    bool ignoreBlankLines = false;        // blank-only differences are minor
    bool ignoreTibetanPunct = false;      // shad family, ACIP , ,, ; * # %  and Wylie / | !
    bool ignoreFolioMarkers = false;      // @NNNA / @NNNB / @NNN
    bool ignoreApparatus = false;         // [..] and {..} spans
    bool scriptAgnostic = false;          // ACIP / Wylie / Unicode → EWTS before comparing
    bool detectMoves = true;
    std::vector<std::string> ignoreRegex;                          // lines matching → minor
    std::vector<std::pair<std::string, std::string>> substitutions; // regex → replacement, applied first
};

enum class Kind { Equal, Insert, Delete, Change };

struct Hunk {
    Kind kind = Kind::Equal;
    int aBeg = 0, aEnd = 0;     // [aBeg, aEnd) in the left lines
    int bBeg = 0, bEnd = 0;     // [bBeg, bEnd) in the right lines
    bool unimportant = false;   // differs only in ignored respects ("minor")
    int movedPeer = -1;         // index of the matching Insert/Delete hunk (moved block), or -1
    bool rawNormalised = true;  // false when scriptAgnostic could not convert a line here
};

struct Result {
    std::vector<Hunk> hunks;    // in order, covering both inputs completely
    int aLines = 0, bLines = 0;
    int changes = 0, inserts = 0, deletes = 0;   // important differences
    int unimportant = 0;        // minor differences
    int moved = 0;              // moved blocks (pairs)
    int unnormalised = 0;       // lines the script-agnostic rule could not convert
    int differences() const { return changes + inserts + deletes; }
};

std::vector<std::string> splitLines(const std::string& text);
std::string joinLines(const std::vector<std::string>& lines, const std::string& eol = "\n");
// The comparison key for a line under the options (public for tests and
// for the folder compare's "rules-based" criterion).
std::string normalizeLine(const std::string& line, const Options& o, bool* normalisedOk = nullptr);
bool matchesIgnoreRegex(const std::string& line, const Options& o);
Result diffLines(const std::vector<std::string>& a, const std::vector<std::string>& b, const Options& o);

// In-line differences within a pair of lines. Auto picks syllables for
// Tibetan (Unicode, Wylie or ACIP), words for English, chars otherwise.
enum class Mode { Auto, Syllables, Words, Chars };
struct Span { int aBeg, aEnd, bBeg, bEnd; bool equal; };   // byte offsets
std::vector<Span> diffInline(const std::string& a, const std::string& b, Mode mode = Mode::Auto);
bool looksTibetan(const std::string& s);   // any of the three scripts

// Three-way merge (diff3): base + two descendants.
enum class ChunkKind { Stable, Left, Right, Both, Conflict };
struct Chunk {
    ChunkKind kind = ChunkKind::Stable;
    int baseBeg = 0, baseEnd = 0, leftBeg = 0, leftEnd = 0, rightBeg = 0, rightEnd = 0;
    int resolution = 0;   // conflicts only: 0 unresolved, 1 left, 2 right, 3 both, 4 base
};
struct MergeResult { std::vector<Chunk> chunks; int conflicts = 0; int autoMerged = 0; };
MergeResult merge3(const std::vector<std::string>& base, const std::vector<std::string>& left,
                   const std::vector<std::string>& right, const Options& o);
std::vector<std::string> renderMerge(const MergeResult& m, const std::vector<std::string>& base,
                                     const std::vector<std::string>& left, const std::vector<std::string>& right,
                                     const std::string& leftName = "LEFT", const std::string& rightName = "RIGHT");

// Apply only the selected non-Equal hunks of r (indices into r.hunks) onto
// a: Equal → a's lines; selected → b's lines; unselected → a's lines. The
// hunk-selection primitive every partial merge in the app uses (F0).
std::vector<std::string> applySelected(const std::vector<std::string>& a, const std::vector<std::string>& b,
                                       const Result& r, const std::vector<int>& selectedHunks);

// Reports.
std::string unifiedDiff(const std::string& aName, const std::string& bName,
                        const std::vector<std::string>& a, const std::vector<std::string>& b,
                        const Result& r, int context = 3);
std::string sideBySideHtml(const std::string& aName, const std::string& bName,
                           const std::vector<std::string>& a, const std::vector<std::string>& b,
                           const Result& r, bool differencesOnly, int context = 2);
struct ApparatusEntry {
    std::string cite;    // "@012B.3" (folio marker + line within it) or "line 57"
    std::string left;    // the reading(s) on the left  (differing syllables when 1:1)
    std::string right;   // the reading(s) on the right
    Kind kind;
    int aLine = 0, bLine = 0;   // 1-based first lines
};
std::vector<ApparatusEntry> apparatus(const std::vector<std::string>& a, const std::vector<std::string>& b, const Result& r);
std::string apparatusMarkdown(const std::string& aName, const std::string& bName, const std::vector<ApparatusEntry>& e);
std::string apparatusCsv(const std::vector<ApparatusEntry>& e);
std::string summary(const Result& r);

}  // namespace textdiff
}  // namespace allcore
