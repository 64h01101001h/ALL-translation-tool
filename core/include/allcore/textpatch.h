// allcore/textpatch.h — the Apply Patch engine (2026-09-09, analysis-suite F5).
//
// Read a GNU unified diff, match its hunks against the text in hand, and say —
// for every hunk, one at a time — exactly how it landed. Qt-free: the caller
// supplies the target's lines and re-joins the result in the target's own EOL,
// encoding and BOM. The algorithm is the documented behaviour of patch(1)
// (Larry Wall; the GNU patch manual, "Applying Imperfect Patches", and the GNU
// Diffutils manual, "Detailed Description of Unified Format"). No product code
// is consulted, and app/thirdparty's diff_match_patch is deliberately NOT used:
// it reads its own patch format, throws bare const char*, and is QString-bound.
//
// Honesty rules this engine enforces — they are the reason it exists:
//
//  - FIVE DISTINCT VERDICTS, never folded into one word "applied". Every hunk
//    comes back Exact, Offset, Fuzz, AlreadyApplied or Rejected, with the line
//    it landed on, the offset from where the patch said it would be, and the
//    fuzz it needed. A fuzzy placement can never read as an exact one.
//
//  - FUZZ NEVER RELAXES A '-' LINE. Fuzzing drops leading and trailing CONTEXT
//    lines and nothing else; a line the patch proposes to DELETE must be
//    present verbatim, at every fuzz level. looseWhitespace likewise relaxes
//    context lines only.
//
//  - A HUNK WITH NO CONTEXT LEFT AFTER FUZZING IS REJECTED, not matched
//    somewhere. Once fuzzing has eaten a hunk's whole context there is nothing
//    left to place it BY, and in repetitive liturgical text its '-' lines would
//    match a dozen places. Refusing is the only honest answer.
//
//  - ALREADY APPLIED IS A FIRST-CLASS VERDICT, not a failure. When the '+' side
//    matches at the position, the hunk is reported as already applied, nothing
//    is changed, and it is NOT written to the .rej — re-running a corrector's
//    patch is a normal accident and must not look like a rejection.
//
//  - REJECTED HUNKS ARE NEVER FORCED. A rejected hunk changes nothing, leaves
//    the running delta alone, and is exported verbatim (its own @@ header,
//    byte for byte) by rejectedAsUnified for the corrector to look at.
//
//  - PARSE PROBLEMS NAME THE LINE. Every error carries "line N: " and the text
//    it choked on. A hunk whose body length disagrees with its header is an
//    error for that hunk and the hunk is DROPPED — never repaired by guessing.
//
//  - NOTHING IS GUESSED. pickFileForTarget returns -1 rather than pick between
//    two candidates; a patch that does not parse yields no hunks; the caller
//    writes nothing until a human says so.
#pragma once

#include <string>
#include <vector>

namespace allcore::textpatch {

// ------------------------------------------------------------ the patch
struct PatchLine { char tag; std::string text; };   // ' ' context, '-' remove, '+' add
struct Hunk {
    int aStart = 0, aLen = 0, bStart = 0, bLen = 0;   // 1-based, as in the header
    std::string header;                               // the @@ line verbatim (CR stripped)
    std::vector<PatchLine> lines;
    bool noNewlineA = false, noNewlineB = false;      // "\ No newline at end of file"
};
struct PatchFile {
    std::string aName, bName;                         // as written: "a/x", not stripped
    std::vector<Hunk> hunks;
    std::vector<std::string> warnings;                // tolerated preamble lines
};
struct ParseResult { std::vector<PatchFile> files; std::vector<std::string> errors; };

// Parse a GNU unified diff. Tolerated as WARNINGS, not errors: "diff --git",
// "index …", "similarity …" and the git mode/rename family, "Index:", "====",
// blank separators, CRLF patch text, and anything before the first ---/+++
// pair (patch(1) skips leading garbage). Everything else where a @@ header
// was due is an error naming the line.
ParseResult parseUnified(const std::string& patchText);

// ------------------------------------------------------------ applying
enum class Placement { Exact, Offset, Fuzz, AlreadyApplied, Rejected };
struct Outcome {
    int hunkIx = 0; Placement how = Placement::Rejected;
    int atLine = -1, offset = 0, fuzz = 0;   // atLine 0-based in the target; -1 = nowhere
    std::string reason;                      // rejections: what did not match, and where
};
struct ApplyOptions {
    int maxFuzz = 2; bool reverse = false; bool looseWhitespace = true;
    std::vector<int> onlyHunks;              // empty = all; others → Rejected "not selected"
};
struct ApplyResult {
    std::vector<std::string> lines;
    std::vector<Outcome> outcomes;
    int exact = 0, offsetApplied = 0, fuzzApplied = 0, alreadyApplied = 0, rejected = 0;
    int applied() const { return exact + offsetApplied + fuzzApplied; }
};

// Apply p's hunks to target, in order, carrying a running line delta from the
// hunks that landed. Context lines are copied from the TARGET, so a loose
// whitespace match never silently reformats the target's spacing; '+' lines
// come from the patch. Every comparison ignores one trailing '\r', so a CRLF
// target and an LF patch interoperate.
ApplyResult applyPatch(const std::vector<std::string>& target, const PatchFile& p,
                       const ApplyOptions& o);

// The rejected hunks, verbatim, as a unified diff for a .rej file — original @@
// headers byte for byte, hunks that were merely "not selected" excluded, and an
// empty string when nothing was rejected (so the caller writes no file).
std::string rejectedAsUnified(const PatchFile& p, const ApplyResult& r);

// patch -R: swap '-' and '+' (and the names, ranges and no-newline flags).
PatchFile reversePatch(const PatchFile& p);

// The index of the SINGLE file in pr whose basename — of aName or bName, after
// stripping a "a/" or "b/" prefix — equals targetBasename. -1 when none match
// and -1 when more than one does: the caller must not guess.
int pickFileForTarget(const ParseResult& pr, const std::string& targetBasename);

}  // namespace allcore::textpatch
