// textdiff_smoke — the compare & merge engine battery.
#include <cstdio>
#include <string>
#include <vector>

#include "allcore/textdiff.h"
#include "allcore/textspan.h"

using namespace allcore::textdiff;
static int failures = 0;
#define CHECK(cond, msg) do { if (cond) std::printf("  [PASS] %s\n", msg); else { std::printf("  [FAIL] %s\n", msg); ++failures; } } while (0)

static std::vector<std::string> L(const char* t) { return splitLines(t); }
static int kindCount(const Result& r, Kind k, bool important = true) { int n = 0; for (const auto& h : r.hunks) if (h.kind == k && (!important || !h.unimportant)) ++n; return n; }
static bool coversAll(const Result& r) {
    int a = 0, b = 0;
    for (const auto& h : r.hunks) { if (h.aBeg != a || h.bBeg != b) return false; a = h.aEnd; b = h.bEnd; }
    return a == r.aLines && b == r.bLines;
}

int main() {
    setvbuf(stdout, nullptr, _IONBF, 0);
    Options o;

    // lines
    CHECK(splitLines("a\r\nb\rc\n").size() == 3 && splitLines("a\r\nb\rc\n")[1] == "b", "splitLines handles CRLF, CR and LF; no empty tail");
    CHECK(splitLines("").empty() && splitLines("x").size() == 1, "empty text has no lines; unterminated last line kept");

    // identical
    auto r0 = diffLines(L("a\nb\nc\n"), L("a\nb\nc\n"), o);
    CHECK(r0.differences() == 0 && r0.hunks.size() == 1 && r0.hunks[0].kind == Kind::Equal && coversAll(r0), "identical inputs → one Equal hunk");

    // classic edit
    auto r1 = diffLines(L("a\nb\nc\nd\ne\n"), L("a\nx\nc\ne\nf\n"), o);
    CHECK(coversAll(r1), "hunks cover both inputs in order");
    CHECK(kindCount(r1, Kind::Change) == 1 && kindCount(r1, Kind::Delete) == 1 && kindCount(r1, Kind::Insert) == 1, "b→x change, d deleted, f inserted");
    CHECK(r1.differences() == 3 && summary(r1).find("3 differences") == 0, "summary counts the three");

    // one side empty
    auto r2 = diffLines({}, L("a\nb\n"), o);
    CHECK(r2.hunks.size() == 1 && r2.hunks[0].kind == Kind::Insert && r2.inserts == 1 && coversAll(r2), "empty left → one Insert hunk");
    auto r2b = diffLines(L("a\nb\n"), {}, o);
    CHECK(r2b.hunks.size() == 1 && r2b.hunks[0].kind == Kind::Delete && coversAll(r2b), "empty right → one Delete hunk");

    // no common line at all
    auto r3 = diffLines(L("a\nb\n"), L("c\nd\n"), o);
    CHECK(r3.differences() == 1 && r3.hunks.size() == 1 && r3.hunks[0].kind == Kind::Change && coversAll(r3), "nothing in common → a single Change hunk");

    // longer: shortest edit script property (LCS length) on a known case
    auto r4 = diffLines(L("A\nB\nC\nA\nB\nB\nA\n"), L("C\nB\nA\nB\nA\nC\n"), o);
    int equalLines = 0; for (const auto& h : r4.hunks) if (h.kind == Kind::Equal) equalLines += h.aEnd - h.aBeg;
    CHECK(equalLines == 4 && coversAll(r4), "ABCABBA vs CBABAC keeps an LCS of 4 (Myers' paper example)");

    // minor differences under rules
    Options ws; ws.ignoreWhitespaceChange = true;
    auto r5 = diffLines(L("SEMS  CAN\nTHAMS CAD\n"), L("SEMS CAN\nTHAMS CAD\n"), ws);
    CHECK(r5.differences() == 0 && r5.unimportant == 1 && r5.hunks[0].kind == Kind::Change && r5.hunks[0].unimportant, "whitespace-only change is minor, not important");
    Options bl; bl.ignoreBlankLines = true;
    auto r6 = diffLines(L("a\n\nb\n"), L("a\nb\n"), bl);
    CHECK(r6.differences() == 0 && r6.unimportant == 1, "an extra blank line is minor under Ignore Blank Lines");
    Options cs; cs.ignoreCase = true;
    CHECK(diffLines(L("Sems Can\n"), L("SEMS CAN\n"), cs).differences() == 0, "case is ignored when asked");
    Options rx; rx.ignoreRegex = {"^@[0-9]+[AB]"};
    auto r7 = diffLines(L("@001A\nBLA MA\n"), L("@002A\nBLA MA\n"), rx);
    CHECK(r7.differences() == 0 && r7.unimportant == 1, "a line filter makes matching-line differences minor");
    Options sub; sub.substitutions = {{"NGO BO", "NGO-BO"}};
    CHECK(diffLines(L("NGO BO NYID\n"), L("NGO-BO NYID\n"), sub).differences() == 0, "a substitution filter equalises the two spellings");

    // Tibetan rules
    Options tp; tp.ignoreTibetanPunct = true;
    CHECK(diffLines(L("SEMS CAN THAMS CAD,\n"), L("SEMS CAN THAMS CAD,,\n"), tp).differences() == 0, "shad vs double shad is minor under Ignore Tibetan Punctuation");
    CHECK(diffLines(L("\xE0\xBD\xA6\xE0\xBD\xBA\xE0\xBD\x98\xE0\xBD\xA6\xE0\xBC\x8B\xE0\xBD\x85\xE0\xBD\x93\xE0\xBC\x8D\n"), L("\xE0\xBD\xA6\xE0\xBD\xBA\xE0\xBD\x98\xE0\xBD\xA6\xE0\xBC\x8B\xE0\xBD\x85\xE0\xBD\x93\xE0\xBC\x8E\n"), tp).differences() == 0, "Unicode shad vs nyis shad is minor under the same rule");
    Options fm; fm.ignoreFolioMarkers = true;
    CHECK(diffLines(L("@012B SEMS CAN\n"), L("@013A SEMS CAN\n"), fm).differences() == 0, "folio markers are ignored when asked");
    Options ap; ap.ignoreApparatus = true;
    CHECK(diffLines(L("SEMS [CAN] THAMS {sic} CAD\n"), L("SEMS THAMS CAD\n"), ap).differences() == 0, "ACIP apparatus brackets are ignored when asked");
    Options sa; sa.scriptAgnostic = true;
    CHECK(diffLines(L("SEMS CAN THAMS CAD\n"), L("sems can thams cad\n"), sa).differences() == 0, "ACIP and Wylie of the same words compare equal under Script-agnostic");
    auto rsa = diffLines(L("\xE0\xBD\xA6\xE0\xBD\xBA\xE0\xBD\x98\xE0\xBD\xA6\xE0\xBC\x8B\xE0\xBD\x85\xE0\xBD\x93\n"), L("sems can\n"), sa);
    CHECK(rsa.differences() == 0 || rsa.unnormalised > 0, "Tibetan script vs Wylie: equal, or honestly reported as not normalised");
    CHECK(rsa.differences() == 0, "…and the reverse converter does convert སེམས་ཅན cleanly");

    // moved blocks
    auto r8 = diffLines(L("a\nb\nc\nd\ne\n"), L("a\nd\ne\nb\nc\n"), o);
    CHECK(r8.moved == 1, "a block moved down is detected as one moved pair");
    bool peers = false; for (const auto& h : r8.hunks) if (h.movedPeer >= 0 && r8.hunks[h.movedPeer].movedPeer >= 0) peers = true;
    CHECK(peers, "moved hunks point at each other");

    // in-line: Tibetan syllables (ACIP)
    auto sp = diffInline("SEMS CAN THAMS CAD", "SEMS CAN KUN");
    int diffSpans = 0; std::string aDiff, bDiff;
    for (const auto& s : sp) if (!s.equal) { ++diffSpans; aDiff += std::string("SEMS CAN THAMS CAD").substr(s.aBeg, s.aEnd - s.aBeg); bDiff += std::string("SEMS CAN KUN").substr(s.bBeg, s.bEnd - s.bBeg); }
    CHECK(diffSpans == 1 && aDiff == "THAMS CAD" && bDiff == "KUN", "syllable-level in-line diff isolates THAMS CAD ↔ KUN");
    auto sp2 = diffInline("the great book", "the small book", Mode::Words);
    std::string w; for (const auto& s : sp2) if (!s.equal) w = std::string("the great book").substr(s.aBeg, s.aEnd - s.aBeg);
    CHECK(w == "great", "word-level in-line diff isolates the changed word");
    auto sp3 = diffInline("abc", "abd", Mode::Chars);
    CHECK(sp3.size() >= 2 && !sp3.back().equal && sp3.back().aBeg == 2, "character mode isolates the last character");
    CHECK(looksTibetan("SEMS CAN") && looksTibetan("\xE0\xBD\xA6\xE0\xBD\xBA\xE0\xBD\x98\xE0\xBD\xA6") && looksTibetan("sems can"), "looksTibetan recognises ACIP, Unicode and Wylie (lower-case English is undecidable and tokenises the same way)");

    // three-way merge
    auto base = L("a\nb\nc\nd\ne\n");
    auto m1 = merge3(base, L("a\nB\nc\nd\ne\n"), L("a\nb\nc\nd\nE\n"), o);
    CHECK(m1.conflicts == 0 && m1.autoMerged == 2, "non-overlapping edits on both sides auto-merge");
    auto out1 = renderMerge(m1, base, L("a\nB\nc\nd\ne\n"), L("a\nb\nc\nd\nE\n"));
    CHECK(joinLines(out1) == "a\nB\nc\nd\nE\n", "merged output takes both edits");
    auto m2 = merge3(base, L("a\nX\nc\nd\ne\n"), L("a\nY\nc\nd\ne\n"), o);
    CHECK(m2.conflicts == 1, "the same line changed differently is one conflict");
    auto out2 = renderMerge(m2, base, L("a\nX\nc\nd\ne\n"), L("a\nY\nc\nd\ne\n"), "MINE", "THEIRS");
    CHECK(joinLines(out2).find("<<<<<<< MINE\nX\n=======\nY\n>>>>>>> THEIRS\n") != std::string::npos, "an unresolved conflict renders with markers");
    for (auto& c : m2.chunks) if (c.kind == ChunkKind::Conflict) c.resolution = 2;
    CHECK(joinLines(renderMerge(m2, base, L("a\nX\nc\nd\ne\n"), L("a\nY\nc\nd\ne\n"))) == "a\nY\nc\nd\ne\n", "Take Right resolves it");
    auto m3 = merge3(base, L("a\nZ\nc\nd\ne\n"), L("a\nZ\nc\nd\ne\n"), o);
    CHECK(m3.conflicts == 0 && joinLines(renderMerge(m3, base, L("a\nZ\nc\nd\ne\n"), L("a\nZ\nc\nd\ne\n"))) == "a\nZ\nc\nd\ne\n", "the same change on both sides is taken once");
    auto m4 = merge3(base, L("a\nb\nc\nd\n"), L("a\nb\nc\nd\ne\nf\n"), o);
    CHECK(m4.conflicts == 0 && joinLines(renderMerge(m4, base, L("a\nb\nc\nd\n"), L("a\nb\nc\nd\ne\nf\n"))) == "a\nb\nc\nd\nf\n" || m4.conflicts == 1, "delete-at-end vs append-at-end: merged or flagged, never silently wrong");
    auto m5 = merge3(base, L("a\nb\nc\nd\ne\n"), L("a\nb\nc\nd\ne\n"), o);
    CHECK(m5.conflicts == 0 && m5.autoMerged == 0 && joinLines(renderMerge(m5, base, base, base)) == "a\nb\nc\nd\ne\n", "no changes → base unchanged");

    // reports
    const auto a = L("@001A\nBLA MA LA\nPHYAG 'TSHAL LO\nSEMS CAN THAMS CAD\n"), b = L("@001A\nBLA MA LA\nPHYAG 'TSHAL LO\nSEMS CAN KUN\nBDE BA\n");
    auto rr = diffLines(a, b, o);
    const auto u = unifiedDiff("A.act", "B.act", a, b, rr, 1);
    if (u.find("@@ -3,2 +3,3 @@") == std::string::npos) std::printf("---- unified diff was:\n%s----\n", u.c_str());
    CHECK(u.find("--- A.act\n+++ B.act\n") == 0 && u.find("-SEMS CAN THAMS CAD\n+SEMS CAN KUN\n+BDE BA\n") != std::string::npos && u.find("@@ -3,2 +3,3 @@") != std::string::npos, "unified diff has headers, hunk range and ± lines");
    const auto html = sideBySideHtml("A", "B", a, b, rr, true, 1);
    CHECK(html.find("<mark>THAMS CAD</mark>") != std::string::npos && html.find("<mark>KUN</mark>") != std::string::npos, "HTML report marks the differing syllables");
    const auto app = apparatus(a, b, rr);
    if (!(app.size() == 1 && app[0].cite == "@001A.4" && app[0].left == "THAMS CAD" && app[0].right == "KUN / BDE BA")) for (auto& e : app) std::printf("---- app cite=%s L=%s R=%s\n", e.cite.c_str(), e.left.c_str(), e.right.c_str());
    CHECK(app.size() == 1 && app[0].cite == "@001A.4" && app[0].left == "THAMS CAD" && app[0].right == "KUN / BDE BA", "apparatus cites folio+line and gives only the variant readings (\" / \" = line boundary)");
    const auto md = apparatusMarkdown("A", "B", app);
    CHECK(md.find("**@001A.4** A: THAMS CAD ] B:") != std::string::npos, "apparatus Markdown entry format");
    const auto csv = apparatusCsv(app);
    CHECK(csv.find("cite,kind,a_line,b_line,a_reading,b_reading\n\"@001A.4\",change,4,4,\"THAMS CAD\"") == 0, "apparatus CSV header and first row");

    // F0 (2026-09-09): the one folio definition and hunk selection
    {
        std::string f;
        CHECK(allcore::textspan::folioMarkerEnd("@012B KA", &f) == 5 && f == "012B", "folioMarkerEnd: @012B → end 5, id 012B");
        CHECK(allcore::textspan::folioMarkerEnd("@012BX KA", &f) == -1, "@012BX is not a marker (letter follows)");
        CHECK(allcore::textspan::folioMarkerEnd("@1A", &f) == 3 && f == "1A", "@1A is a marker");
        CHECK(allcore::textspan::folioMarkerEnd("@0012", &f) == 5 && f == "0012", "@0012 (no side) is a marker");
        CHECK(allcore::textspan::folioMarkerEnd("@x", &f) == -1, "@x is not a marker");
        CHECK(allcore::textspan::lastFolio("@001A ka @001B kha") == "001B", "lastFolio takes the last marker on the line");
        int n = 0, w = 0; char side = 0;
        CHECK(allcore::textspan::folioParts("012B", n, side, w) && n == 12 && side == 'B' && w == 3, "folioParts splits number, side and digit width");
        CHECK(allcore::textspan::folioParts("0012", n, side, w) && n == 12 && side == '\0' && w == 4, "folioParts: no side letter");
        Options fm; fm.ignoreFolioMarkers = true;
        CHECK(normalizeLine("@12b KA", fm) == "KA", "normalizeLine strips a lower-case marker through the shared definition");
        bool bal = true;
        const auto sp = allcore::textspan::apparatusSpans("SEMS [CAN] THAMS {sic} CAD", &bal);
        CHECK(bal && sp.size() == 2 && sp[0].kind == '[' && sp[1].kind == '{', "apparatusSpans finds both bracket kinds");
        CHECK(allcore::textspan::stripApparatus("SEMS [CAN] THAMS {sic} CAD") == "SEMS THAMS CAD", "stripApparatus removes spans and collapses spacing");
        bool bal2 = true; allcore::textspan::apparatusSpans("SEMS [CAN THAMS", &bal2);
        CHECK(!bal2 && allcore::textspan::stripApparatus("SEMS [CAN THAMS", &bal2) == "SEMS [CAN THAMS", "an unbalanced bracket is refused: line returned unchanged, balanced=false");
        // applySelected
        const auto a2 = L("x\ny\nz\n"), b2 = L("x\nY\nz\nw\n");
        const auto r2 = diffLines(a2, b2, o);
        int chg = -1, ins = -1;
        for (int i = 0; i < (int)r2.hunks.size(); ++i) { if (r2.hunks[i].kind == Kind::Change) chg = i; if (r2.hunks[i].kind == Kind::Insert) ins = i; }
        CHECK(chg >= 0 && ins >= 0, "fixture: one change and one insert hunk");
        CHECK(applySelected(a2, b2, r2, {}) == a2, "applySelected with nothing selected returns a");
        CHECK(joinLines(applySelected(a2, b2, r2, {chg})) == "x\nY\nz\n", "selecting the change takes Y only");
        CHECK(joinLines(applySelected(a2, b2, r2, {ins})) == "x\ny\nz\nw\n", "selecting the insert appends w only");
        CHECK(applySelected(a2, b2, r2, {chg, ins}) == b2, "selecting every non-Equal hunk yields b");
    }
    std::printf("textdiff_smoke: %d failure(s)\n", failures);
    return failures ? 1 : 0;
}
