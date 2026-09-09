// textpatch_smoke — the Apply Patch engine battery (analysis-suite F5).
// Fixture-free: every patch here is either written by textdiff::unifiedDiff
// (the in-house writer whose exact output the parser must round-trip) or typed
// out by hand to pin one documented patch(1) behaviour.
#include <cstdio>
#include <string>
#include <vector>

#include "allcore/textdiff.h"
#include "allcore/textpatch.h"

using namespace allcore::textpatch;
static int failures = 0;
#define CHECK(cond, msg) do { if (cond) std::printf("  [PASS] %s\n", msg); else { std::printf("  [FAIL] %s\n", msg); ++failures; } } while (0)

static std::string nn(const char* p, int i) { char b[16]; std::snprintf(b, sizeof b, "%s%02d", p, i); return b; }

// The spec's fixture: a = L01..L20; b changes L05, drops L12, adds NEW after L17.
static void fixture1(std::vector<std::string>& a, std::vector<std::string>& b) {
    for (int i = 1; i <= 20; ++i) a.push_back(nn("L", i));
    for (int i = 1; i <= 20; ++i) {
        if (i == 12) continue;
        b.push_back(i == 5 ? "L05x" : nn("L", i));
        if (i == 17) b.push_back("NEW");
    }
}
// A three-hunk fixture (the changes are far enough apart that the writer cannot
// merge them): M05 changed, M20 deleted, M35 changed.
static void fixture2(std::vector<std::string>& a, std::vector<std::string>& b) {
    for (int i = 1; i <= 40; ++i) a.push_back(nn("M", i));
    for (int i = 1; i <= 40; ++i) {
        if (i == 20) continue;
        b.push_back(i == 5 ? "M05x" : (i == 35 ? "M35x" : nn("M", i)));
    }
}
// A two-hunk fixture whose FIRST hunk inserts two lines.
static void fixture3(std::vector<std::string>& a, std::vector<std::string>& b) {
    for (int i = 1; i <= 30; ++i) a.push_back(nn("N", i));
    for (int i = 1; i <= 30; ++i) {
        b.push_back(i == 25 ? "N25x" : nn("N", i));
        if (i == 5) { b.push_back("X1"); b.push_back("X2"); }
    }
}

static std::string patchOf(const char* an, const char* bn,
                           const std::vector<std::string>& a, const std::vector<std::string>& b) {
    using namespace allcore::textdiff;
    return unifiedDiff(an, bn, a, b, diffLines(a, b, Options()), 3);
}
static bool allAre(const ApplyResult& r, Placement p) {
    if (r.outcomes.empty()) return false;
    for (const auto& o : r.outcomes) if (o.how != p) return false;
    return true;
}
static bool has(const std::string& hay, const char* needle) { return hay.find(needle) != std::string::npos; }

int main() {
    setvbuf(stdout, nullptr, _IONBF, 0);

    std::vector<std::string> a, b;
    fixture1(a, b);
    const std::string p1text = patchOf("a", "b", a, b);
    const ParseResult pr1 = parseUnified(p1text);
    const PatchFile P1 = pr1.files.empty() ? PatchFile() : pr1.files[0];

    // 1 — round trip through the in-house writer
    {
        CHECK(pr1.files.size() == 1 && pr1.errors.empty() && P1.aName == "a" && P1.bName == "b" && !P1.hunks.empty(),
              "1a unifiedDiff output parses: one file, names a/b, no errors");
        const ApplyResult r = applyPatch(a, P1, ApplyOptions());
        CHECK(r.lines == b && allAre(r, Placement::Exact) && r.rejected == 0 && r.applied() == (int)P1.hunks.size(),
              "1b applying it to a reproduces b, every hunk Exact, none rejected");
    }

    // 2 — offset: three lines of preamble shift every hunk by +3
    {
        std::vector<std::string> t = {"P1", "P2", "P3"}, want = {"P1", "P2", "P3"};
        t.insert(t.end(), a.begin(), a.end());
        want.insert(want.end(), b.begin(), b.end());
        const ApplyResult r = applyPatch(t, P1, ApplyOptions());
        bool allOff = !r.outcomes.empty();
        for (const auto& o : r.outcomes) if (o.how != Placement::Offset || o.offset != 3) allOff = false;
        CHECK(allOff && r.lines == want && r.offsetApplied == (int)r.outcomes.size(),
              "2 a shifted target places every hunk with offset +3 and is labelled Offset");
    }

    // 3 — fuzz: a leading CONTEXT line of the first hunk is altered
    {
        const int lead0 = P1.hunks[0].aStart - 1;   // the hunk's first line in a
        std::vector<std::string> t = a;
        t[(size_t)lead0] = "ZZZ-not-L02";
        const ApplyResult r = applyPatch(t, P1, ApplyOptions());
        CHECK(r.outcomes[0].how == Placement::Fuzz && r.outcomes[0].fuzz == 1 && r.lines[4] == "L05x" && r.fuzzApplied == 1,
              "3a one bad leading context line → Fuzz 1, and line 5 still becomes L05x");
        ApplyOptions o0; o0.maxFuzz = 0;
        const ApplyResult r0 = applyPatch(t, P1, o0);
        CHECK(r0.outcomes[0].how == Placement::Rejected && r0.lines[4] == "L05" && r0.rejected == 1,
              "3b with maxFuzz 0 the same hunk is Rejected and line 5 stays L05");
    }

    std::vector<std::string> a2, b2;
    fixture2(a2, b2);
    const ParseResult pr2 = parseUnified(patchOf("m", "m", a2, b2));
    const PatchFile P2 = pr2.files.empty() ? PatchFile() : pr2.files[0];
    CHECK(P2.hunks.size() == 3 && pr2.errors.empty(), "3c (fixture) the three-change diff parses as three hunks");

    // 4 — fuzz never relaxes a '-' line
    {
        std::vector<std::string> t = a2;
        t[19] = "M20zzz";                       // the line hunk 2 proposes to delete
        for (int fz = 0; fz <= 2; ++fz) {
            ApplyOptions o; o.maxFuzz = fz;
            const ApplyResult r = applyPatch(t, P2, o);
            const bool ok = r.outcomes[1].how == Placement::Rejected &&
                            has(r.outcomes[1].reason, "M20zzz") && has(r.outcomes[1].reason, "line 20") &&
                            r.outcomes[0].how != Placement::Rejected && r.outcomes[2].how != Placement::Rejected &&
                            r.lines[19] == "M20zzz" && r.lines[4] == "M05x";
            CHECK(ok, fz == 0 ? "4a a changed '-' line rejects its hunk at fuzz 0, quoting the target line"
                              : (fz == 1 ? "4b … and at fuzz 1, while the other hunks still apply"
                                         : "4c … and at fuzz 2; the altered M20 survives untouched"));
        }
    }

    // 5 — no context left after fuzzing → Rejected, not matched somewhere else
    {
        const std::string ptext =
            "--- x\n+++ x\n@@ -2,3 +2,3 @@\n C1\n-OLD\n+NEW\n C2\n";
        const ParseResult pr = parseUnified(ptext);
        const std::vector<std::string> t = {"Z0", "Q1", "OLD", "Q2", "Z4"};
        ApplyOptions o; o.maxFuzz = 2;
        const ApplyResult r = applyPatch(t, pr.files[0], o);
        CHECK(pr.errors.empty() && r.outcomes[0].how == Placement::Rejected && r.lines == t &&
              has(r.outcomes[0].reason, "no context"),
              "5 one context line each end + fuzz 2 → Rejected; OLD is not replaced anywhere");
    }

    // 6 — already applied
    {
        const ApplyResult r = applyPatch(b, P1, ApplyOptions());
        CHECK(allAre(r, Placement::AlreadyApplied) && r.lines == b && r.rejected == 0 && r.applied() == 0,
              "6a re-applying the patch to b reports AlreadyApplied and changes nothing");
        CHECK(rejectedAsUnified(P1, r).empty(), "6b an already-applied patch writes no .rej");
    }

    // 7 — reverse
    {
        ApplyOptions o; o.reverse = true;
        const ApplyResult r = applyPatch(b, P1, o);
        CHECK(r.lines == a && r.rejected == 0, "7 reverse unapplies the patch: b → a");
    }

    // 8 — GNU tolerance
    {
        const std::string g =
            "diff --git a/x b/x\r\n"
            "index 1a2b3c4..5d6e7f8 100644\r\n"
            "--- a/x\t2026-09-08 10:00:00.000000000 -0500\r\n"
            "+++ b/x\t2026-09-08 10:05:00.000000000 -0500\r\n"
            "@@ -1,2 +1,2 @@ section text here\r\n"
            " keep\r\n"
            "-old\r\n"
            "+new\r\n"
            "\\ No newline at end of file\r\n";
        const ParseResult pr = parseUnified(g);
        const bool ok = pr.errors.empty() && pr.files.size() == 1 && !pr.files[0].warnings.empty() &&
                        pr.files[0].hunks.size() == 1 && pr.files[0].hunks[0].noNewlineB &&
                        !pr.files[0].hunks[0].noNewlineA &&
                        pr.files[0].aName == "a/x" && pr.files[0].bName == "b/x" &&
                        pickFileForTarget(pr, "x") == 0 &&
                        pr.files[0].hunks[0].header == "@@ -1,2 +1,2 @@ section text here";
        CHECK(ok, "8 git preamble + timestamps + section text + CRLF + no-newline: warnings, no errors, name resolves to x");
        const ApplyResult r = applyPatch({"keep", "old"}, pr.files[0], ApplyOptions());
        CHECK(r.lines == std::vector<std::string>({"keep", "new"}), "8b … and the CRLF patch applies to an LF target");
    }

    // 9 — a missing count means 1
    {
        const ParseResult pr = parseUnified("--- x\n+++ x\n@@ -5 +5 @@\n-old\n+new\n");
        CHECK(pr.errors.empty() && pr.files.size() == 1 && pr.files[0].hunks.size() == 1 &&
              pr.files[0].hunks[0].aLen == 1 && pr.files[0].hunks[0].bLen == 1 &&
              pr.files[0].hunks[0].aStart == 5 && pr.files[0].hunks[0].bStart == 5,
              "9 \"@@ -5 +5 @@\" means one line each side");
    }

    // 10 — parse problems name the line, and nothing is guessed
    {
        const ParseResult bad = parseUnified("--- x\n+++ x\n@@ garbage\n");
        CHECK(bad.errors.size() == 1 && bad.errors[0].rfind("line 3: expected @@ header", 0) == 0 &&
              bad.files.size() == 1 && bad.files[0].hunks.empty(),
              "10a a malformed @@ line is an error naming line 3, and yields no hunks");
        const ParseResult cnt = parseUnified("--- x\n+++ x\n@@ -1,5 +1,5 @@\n a\n-b\n+c\n d\n");
        CHECK(cnt.errors.size() == 1 && cnt.errors[0].rfind("line 3: hunk", 0) == 0 &&
              has(cnt.errors[0], "the hunk was dropped") && cnt.files[0].hunks.empty(),
              "10b a header whose counts disagree with the body is a per-hunk error and the hunk is dropped");
    }

    // 11 — multi-file, and no guessing when nothing matches
    {
        const std::string two =
            "--- a\n+++ a\n@@ -1,1 +1,1 @@\n-one\n+ONE\n"
            "--- b\n+++ b\n@@ -1,1 +1,1 @@\n-two\n+TWO\n";
        const ParseResult pr = parseUnified(two);
        CHECK(pr.errors.empty() && pr.files.size() == 2 && pr.files[1].aName == "b" &&
              pickFileForTarget(pr, "b") == 1 && pickFileForTarget(pr, "zzz") == -1 &&
              pickFileForTarget(pr, "/some/dir/b") == 1,
              "11 two file sections parse separately; the target picks file 1; an unknown target picks nothing");
    }

    // 12 — onlyHunks
    {
        ApplyOptions o; o.onlyHunks = {0};
        const ApplyResult r = applyPatch(a2, P2, o);
        std::vector<std::string> want = a2; want[4] = "M05x";
        CHECK(r.outcomes[0].how == Placement::Exact && r.lines == want &&
              r.outcomes[1].how == Placement::Rejected && r.outcomes[1].reason == "not selected" &&
              r.outcomes[2].reason == "not selected",
              "12a onlyHunks {0} applies the first hunk only; the others say \"not selected\"");
        CHECK(rejectedAsUnified(P2, r).empty(),
              "12b an unticked hunk is not a rejection and is excluded from the .rej");
    }

    // 13 — loose whitespace, context only
    {
        const int lead0 = P1.hunks[0].aStart - 1;
        std::vector<std::string> t = a;
        t[(size_t)lead0] = a[(size_t)lead0] + "  ";   // two trailing spaces on a context line
        ApplyOptions on;  on.looseWhitespace = true;
        ApplyOptions off; off.looseWhitespace = false;
        const ApplyResult rn = applyPatch(t, P1, on);
        const ApplyResult rf = applyPatch(t, P1, off);
        CHECK(rn.outcomes[0].how == Placement::Exact && rn.lines[4] == "L05x",
              "13a loose whitespace: a context line with trailing spaces still matches Exactly");
        CHECK(rn.lines[(size_t)lead0] == t[(size_t)lead0],
              "13b … and the target's own spacing is kept — a loose match never reformats");
        CHECK(rf.outcomes[0].how == Placement::Fuzz || rf.outcomes[0].how == Placement::Rejected,
              "13c with loose whitespace off the same hunk needs fuzz, or rejects");
    }

    // 14 — the .rej carries the rejected hunks only, headers byte for byte
    {
        std::vector<std::string> t = a2;
        t[19] = "M20zzz";
        const ApplyResult r = applyPatch(t, P2, ApplyOptions());
        const std::string rej = rejectedAsUnified(P2, r);
        CHECK(has(rej, P2.hunks[1].header.c_str()) && !has(rej, P2.hunks[0].header.c_str()) &&
              !has(rej, P2.hunks[2].header.c_str()) && has(rej, "\n-M20\n") && has(rej, "--- m\n+++ m\n"),
              "14 rejectedAsUnified holds hunk 2 with its original @@ header and no applied hunk");
    }

    // 15 — the running delta carries into the next hunk
    {
        std::vector<std::string> a3, b3;
        fixture3(a3, b3);
        const ParseResult pr = parseUnified(patchOf("n", "n", a3, b3));
        const PatchFile P3 = pr.files[0];
        const ApplyResult r = applyPatch(a3, P3, ApplyOptions());
        CHECK(P3.hunks.size() == 2 && r.outcomes[0].how == Placement::Exact &&
              r.outcomes[1].how == Placement::Exact && r.outcomes[1].offset == 0 && r.lines == b3,
              "15 a first hunk that inserts two lines leaves the second landing Exact at its adjusted position");
    }

    std::printf("textpatch_smoke: %d failure(s)\n", failures);
    return failures ? 1 : 0;
}
