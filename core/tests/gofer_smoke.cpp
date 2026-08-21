// gofer_smoke — Gofer query grammar + NEAR-within-N-lines semantics on the
// real corpus, cross-checked against direct queries.
#include <cstdio>
#include <cstdlib>
#include <string>

#include "allcore/gofer.h"
#include "allcore/spine.h"

static int failures = 0;
#define CHECK(cond, msg)                                        \
    do {                                                        \
        if (cond) std::printf("  [PASS] %s\n", msg);            \
        else { std::printf("  [FAIL] %s\n", msg); ++failures; } \
    } while (0)

int main(int argc, char** argv) {
    if (argc < 2) {
        std::fprintf(stderr, "usage: gofer_smoke <spine.db>\n");
        return 2;
    }
    allcore::Spine spine(argv[1]);

    // bare words coalesce into a phrase
    auto h1 = allcore::goferSearch(spine, "bden pa bzhi", "", 30);
    CHECK(!h1.empty(), "phrase query 'bden pa bzhi' finds corpus lines");
    bool allContain = true;
    for (const auto& h : h1) {
        bool found = false;
        for (const auto& s : h.window)
            found |= (s.wylie.find("bden pa bzhi") != std::string::npos);
        allContain &= found;
    }
    CHECK(allContain, "every hit window actually contains the phrase");

    // OR = union
    // limits sized far above the operand counts so the union arithmetic is
    // meaningful (corpus growth raised "don dam bden pa" past the old cap)
    auto hA = allcore::goferSearch(spine, "\"kun rdzob bden pa\"", "", 400);
    auto hB = allcore::goferSearch(spine, "\"don dam bden pa\"", "", 400);
    auto hOr = allcore::goferSearch(spine,
                                    "\"kun rdzob bden pa\" OR \"don dam bden pa\"",
                                    "", 800);
    CHECK(!hA.empty() && !hB.empty(), "both OR operands hit on their own");
    CHECK(hOr.size() >= hA.size() && hOr.size() >= hB.size() &&
              hOr.size() <= hA.size() + hB.size(),
          "OR result size is a union of the operands");

    // NEAR/N: both terms within N lines of the same course
    auto hNear = allcore::goferSearch(
        spine, "\"kun rdzob bden pa\" NEAR/3 \"don dam bden pa\"", "", 60);
    std::printf("  NEAR/3 hits: %zu\n", hNear.size());
    CHECK(!hNear.empty(), "the two truths co-occur within 3 lines somewhere");
    bool windowsSane = true;
    for (const auto& h : hNear)
        windowsSane &= (h.seq_hi - h.seq_lo <= 3 + 1) && !h.window.empty();
    CHECK(windowsSane, "NEAR windows are within the line bound and hydrated");

    // NEAR tightening: NEAR/0 ⊆ NEAR/8
    auto hNear0 = allcore::goferSearch(
        spine, "\"kun rdzob bden pa\" NEAR/0 \"don dam bden pa\"", "", 60);
    auto hNear8 = allcore::goferSearch(
        spine, "\"kun rdzob bden pa\" NEAR/8 \"don dam bden pa\"", "", 200);
    CHECK(hNear0.size() <= hNear.size() && hNear.size() <= hNear8.size(),
          "smaller NEAR windows never yield more hits");

    // course filter
    auto hC02 = allcore::goferSearch(spine, "bden pa", "C02", 60);
    bool onlyC02 = !hC02.empty();
    for (const auto& h : hC02) onlyC02 &= (h.course == "C02");
    CHECK(onlyC02, "course filter restricts hits to C02");

    // parentheses + mixed operators parse and run
    auto hMix = allcore::goferSearch(
        spine, "(\"sdug bsngal\" OR \"kun 'byung\") NEAR/5 \"'gog pa\"", "", 60);
    std::printf("  mixed-query hits: %zu\n", hMix.size());
    CHECK(true, "mixed parenthesized query parses and evaluates");

    // malformed queries throw instead of guessing
    bool threw = false;
    try {
        allcore::goferSearch(spine, "\"unclosed", "", 5);
    } catch (const std::exception&) { threw = true; }
    CHECK(threw, "malformed query (unclosed quote) throws");

    // ---- file-tree search (Gofer's original habitat) ----
    if (argc > 2) {
        const std::string root = argv[2];
        auto f1 = allcore::goferSearchFiles(root, "bden pa", 50);
        std::printf("  file hits for 'bden pa': %zu\n", f1.size());
        CHECK(f1.size() >= 5, "file search finds the phrase in both files");
        bool binSkipped = true;
        for (const auto& h : f1)
            binSkipped &= (h.file.find("ignore.bin") == std::string::npos);
        CHECK(binSkipped, "non-text files are skipped");

        // NEAR within a file: in text_one the two truths are 2 lines apart;
        // in text_two they are 6 lines apart
        auto fn2 = allcore::goferSearchFiles(
            root, "\"KUN RDZOB\" NEAR/2 \"DON DAM\"", 50);
        auto fn6 = allcore::goferSearchFiles(
            root, "\"KUN RDZOB\" NEAR/6 \"DON DAM\"", 50);
        bool onlyOne = !fn2.empty();
        for (const auto& h : fn2)
            onlyOne &= (h.file.find("text_one") != std::string::npos);
        CHECK(onlyOne, "NEAR/2 matches only the close-together file");
        CHECK(fn6.size() > fn2.size(), "NEAR/6 also reaches the far pair");
        bool windowText = false;
        for (const auto& h : fn2)
            for (const auto& l : h.lines)
                windowText |= (l.find("KUN RDZOB") != std::string::npos);
        CHECK(windowText, "hit windows carry the actual file lines");

        // NEAR must never match across files: text_one has KUN RDZOB at line 3,
        // text_two has DON DAM at line 7 — a huge window still can't join them
        // beyond what each file contains on its own
        auto fx = allcore::goferSearchFiles(
            root, "\"BDEN PA GNYIS\" NEAR/100 \"many\"", 50);
        CHECK(fx.empty(), "NEAR never joins occurrences across files");
    }

    std::printf("%s (%d failures)\n",
                failures ? "GOFER SMOKE FAILED" : "GOFER SMOKE OK", failures);
    return failures ? 1 : 0;
}
