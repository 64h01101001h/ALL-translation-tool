// textnorm_smoke — the Normalize engine battery (analysis-suite F3).
// One numbered check per acceptance test in the plan; fixture-free, no data
// files, no Qt. Every fixture built here is also re-run through the engine a
// second time by check 13 (idempotence) and measured by check 16 (Normalize
// never joins or splits a line).
#include <cstdio>
#include <string>
#include <vector>

#include "allcore/textdiff.h"   // splitLines, to build fixtures from raw text
#include "allcore/textnorm.h"

using namespace allcore::textnorm;
using allcore::textdiff::splitLines;

static int failures = 0;
#define CHECK(cond, msg) do { if (cond) std::printf("  [PASS] %s\n", msg); else { std::printf("  [FAIL] %s\n", msg); ++failures; } } while (0)

// Every fixture is banked as it runs, for checks 13 and 16.
struct Fixture { std::vector<std::string> lines; std::string eol; Options opt; };
static std::vector<Fixture> banked;

static Result run(const std::vector<std::string>& lines, const Options& o,
                  const std::string& eol = "\n") {
    banked.push_back({lines, eol, o});
    return normalizeText(lines, eol, o);
}
// One line in, one line out — the common shape of checks 1–10.
static std::string one(const std::string& line, const Options& o) {
    const Result r = run({line}, o);
    return r.lines.empty() ? std::string() : r.lines[0];
}
static int countSub(const std::string& hay, const std::string& needle) {
    int n = 0;
    for (size_t p = hay.find(needle); p != std::string::npos; p = hay.find(needle, p + needle.size())) ++n;
    return n;
}

int main() {
    setvbuf(stdout, nullptr, _IONBF, 0);

    // 1. whitespace
    {
        Options o; o.collapseSpaces = true; o.trimTrailing = true;
        const Result r = run({"A  B\t C  "}, o);
        CHECK(r.lines.size() == 1 && r.lines[0] == "A B C", "1. collapse runs of spaces and tabs, trim the trailing run");
        CHECK(r.changedLines == 1, "1. changedLines counts the one line that moved");
    }

    // 2. spacing around a single shad (ACIP)
    {
        Options o; o.spaceBeforeShad = ShadSpace::None;
        CHECK(one("BLA MA ,LA", o) == "BLA MA,LA", "2. space before a shad: none");
        Options p; p.spaceBeforeShad = ShadSpace::One;
        CHECK(one("BLA MA ,LA", p) == "BLA MA ,LA", "2. space before a shad: one (already one — unchanged)");
        Options q; q.spaceAfterShad = true;
        CHECK(one("BLA MA,LA", q) == "BLA MA, LA", "2. one space after a shad");
    }

    // 3. the double shad is one atomic unit
    {
        Options o; o.doubleShad = ",,";
        CHECK(one("KUN , , BDE", o) == "KUN,, BDE", "3. double shad respelled ',,'");
        Options p; p.doubleShad = ", ,";
        CHECK(one("KUN , , BDE", p) == "KUN, , BDE", "3. double shad respelled ', ,'");
        Options q;   // no form chosen: the house ruling has not been supplied
        CHECK(one("KUN , , BDE", q) == "KUN , , BDE", "3. no double-shad form chosen → untouched");
    }

    // 4. a single shad is never merged into a double
    {
        Options o; o.doubleShad = ",,";
        CHECK(one("KA, KHA", o) == "KA, KHA", "4. single shad is never merged");
    }

    // 5. English is not ACIP: a comma is a comma there
    {
        Options o; o.doubleShad = ",,"; o.spaceBeforeShad = ShadSpace::None; o.spaceAfterShad = true;
        const Result r = run({"Yes, no, maybe"}, o);
        CHECK(r.lines.size() == 1 && r.lines[0] == "Yes, no, maybe", "5. English line untouched by every shad rule");
        CHECK(r.changedLines == 0 && r.refused.empty(), "5. no change and no refusal on an English line");
        CHECK(r.shadRulesSkipped == 1, "5. the skip is counted, not hidden");
        CHECK(classifyLine("Yes, no, maybe") == LineClass::Other, "5. classifier calls it Other");
    }

    // 6. mixed script is refused, not guessed
    {
        Options o; o.doubleShad = ",,"; o.spaceBeforeShad = ShadSpace::None; o.spaceAfterShad = true;
        const Result r = run({"SEMS ,, sems"}, o);
        CHECK(r.lines.size() == 1 && r.lines[0] == "SEMS ,, sems", "6. mixed-script line is byte-identical");
        CHECK(r.refused.size() == 1 && r.refused[0].line == 0 && r.refused[0].reason == "mixed script", "6. refused with reason 'mixed script'");
        CHECK(classifyLine("SEMS ,, sems") == LineClass::Mixed, "6. classifier calls it Mixed");
    }

    // 7. Tibetan Unicode shads
    {
        Options o; o.spaceAfterShad = true;
        CHECK(one("ཀ།ཁ", o) == "ཀ། ཁ", "7. one space after U+0F0D");
        Options p; p.spaceBeforeShad = ShadSpace::None;
        CHECK(one("ཀ ། ཁ", p) == "ཀ། ཁ", "7. no space before U+0F0D");
        Options q; q.doubleShad = "༎";
        CHECK(one("ཀ།།ཁ", q) == "ཀ༎ཁ", "7. double shad respelled U+0F0E");
        CHECK(classifyLine("ཀ།ཁ") == LineClass::TibetanUnicode, "7. classifier calls it Tibetan Unicode");
    }

    // 8. the tsheg is not whitespace and is never touched
    {
        Options o; o.collapseSpaces = true;
        const std::string in = "བཀྲ་ཤིས་  ཀ";
        const std::string out = one(in, o);
        CHECK(out == "བཀྲ་ཤིས་ ཀ", "8. only the ASCII run collapses");
        CHECK(countSub(in, "་") == 2 && countSub(out, "་") == countSub(in, "་"), "8. every U+0F0B tsheg survives");
    }

    // 9. apparatus
    {
        Options o; o.stripApparatus = true; o.collapseSpaces = true;
        CHECK(one("[12] BLA {MA} LA", o) == "BLA LA", "9. [ ] and { } spans removed");
        const Result r1 = run({"[12 BLA"}, o);
        CHECK(r1.lines[0] == "[12 BLA" && r1.refused.size() == 1 && r1.refused[0].reason == "unbalanced [ ]", "9. a bracket that never closes refuses the line");
        const Result r2 = run({"BLA }"}, o);
        CHECK(r2.lines[0] == "BLA }" && r2.refused.size() == 1 && r2.refused[0].reason == "unbalanced { }", "9. a brace that closes unopened refuses the line");
        CHECK(bracketsBalanced("[a] [b]", '[', ']') && !bracketsBalanced("[12 BLA", '[', ']') && !bracketsBalanced("BLA }", '{', '}'), "9. bracketsBalanced agrees");
    }

    // 10. folio markers
    {
        Options o; o.stripFolioMarkers = true;
        CHECK(one("@001A KA", o) == "KA", "10. folio marker removed, its gap closed");
        const Result r = run({"@001AX KA"}, o);
        CHECK(r.lines[0] == "@001AX KA" && r.refused.size() == 1 && r.refused[0].reason == "marker glued to text", "10. a marker glued to text refuses the line");
    }

    // 11. line endings
    {
        Options o; o.eol = Eol::LF;
        const std::string crlf = "a\r\nb\r\n";
        CHECK(detectEol(crlf) == "\r\n" && detectEol("a\rb") == "\r" && detectEol("a\nb") == "\n" && detectEol("ab").empty(), "11. detectEol reads the first terminator");
        const Result r = run(splitLines(crlf), o, detectEol(crlf));
        CHECK(r.lines.size() == 2 && r.lines[0] == "a" && r.lines[1] == "b" && r.eol == "\n", "11. CRLF → LF");
        Options k;
        const Result r2 = run(splitLines(crlf), k, detectEol(crlf));
        CHECK(r2.eol == "\r\n", "11. keep keeps CRLF");
        const Result r3 = run(splitLines("a\rb"), k, detectEol("a\rb"));
        CHECK(r3.eol == "\r", "11. keep keeps CR");
        CHECK(!r.notes.empty() && r.notes[0] == "line endings CRLF → LF", "11. the line-ending change is stated");
    }

    // 12. blank-line runs and the final newline
    {
        Options o; o.maxBlankLines = 1; o.ensureFinalNewline = true;
        const Result r = run(splitLines("a\n\n\n\nb"), o);
        CHECK(r.lines.size() == 3 && r.lines[0] == "a" && r.lines[1].empty() && r.lines[2] == "b", "12. a run of blank lines is reduced to the maximum");
        CHECK(r.finalNewlineAdded, "12. ensureFinalNewline is reported");
        Options p; p.maxBlankLines = 1;
        const Result r2 = run(splitLines("a\n\n\n\nb"), p);
        CHECK(!r2.finalNewlineAdded, "12. and is not claimed when the switch is off");
    }

    // 14. every switch off changes nothing (banked before 13 runs over it)
    {
        Options o;
        const std::vector<std::string> in = {"A  B\t ", "[12 BLA", "SEMS ,, sems", "@001A KA", ""};
        const Result r = run(in, o);
        CHECK(r.lines == in, "14. all options off → byte-identical lines");
        CHECK(r.changedLines == 0 && r.refused.empty() && r.shadRulesSkipped == 0, "14. nothing counted as changed, refused or skipped");
    }

    // 15. refusals are byte-identical and carry the input line number
    {
        Options o;
        o.stripApparatus = true; o.stripFolioMarkers = true;
        o.collapseSpaces = true; o.trimTrailing = true; o.spaceAfterShad = true;
        const std::vector<std::string> in = {"KA KHA", "[12 BLA", "SEMS ,, sems", "@001AX KA"};
        const Result r = run(in, o);
        CHECK(r.lines == in, "15. all four lines come through byte-identical");
        CHECK(r.refused.size() == 3, "15. three lines refused");
        CHECK(r.refused.size() == 3 && r.refused[0].line == 1 && r.refused[0].reason == "unbalanced [ ]" &&
              r.refused[1].line == 2 && r.refused[1].reason == "mixed script" &&
              r.refused[2].line == 3 && r.refused[2].reason == "marker glued to text", "15. 0-based line numbers and reasons are right");
        CHECK(r.changedLines == 0, "15. a refused line is not counted as changed");
    }

    // 13. idempotence over every fixture above
    {
        int bad = 0;
        for (const auto& f : banked) {
            const Result once = normalizeText(f.lines, f.eol, f.opt);
            const Result twice = normalizeText(once.lines, once.eol.empty() ? f.eol : once.eol, f.opt);
            if (twice.lines != once.lines) ++bad;
        }
        CHECK(bad == 0, "13. normalizing twice equals normalizing once, for every fixture");
    }

    // 16. no line count changes except by the blank-line rule
    {
        int bad = 0;
        for (const auto& f : banked) {
            if (f.opt.maxBlankLines >= 0) continue;
            const Result r = normalizeText(f.lines, f.eol, f.opt);
            if (r.lines.size() != f.lines.size()) ++bad;
        }
        CHECK(bad == 0, "16. Normalize never joins or splits a line");
    }

    std::printf("textnorm_smoke: %d failure(s)\n", failures);
    return failures ? 1 : 0;
}
