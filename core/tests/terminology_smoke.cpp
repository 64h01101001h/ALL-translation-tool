// terminology_smoke — the draft-workspace terminology checker.
#include <cstdio>

#include "allcore/terminology.h"

static int failures = 0;
#define CHECK(cond, msg)                                        \
    do {                                                        \
        if (cond) std::printf("  [PASS] %s\n", msg);            \
        else { std::printf("  [FAIL] %s\n", msg); ++failures; } \
    } while (0)

int main(int argc, char** argv) {
    setvbuf(stdout, nullptr, _IONBF, 0);
    if (argc < 2) {
        std::fprintf(stderr, "usage: terminology_smoke <spine.db>\n");
        return 2;
    }
    allcore::Spine spine(argv[1]);
    allcore::HeadwordIndex index(spine);

    // gloss matching mechanics
    CHECK(allcore::glossMatches("Dharma (the teaching)",
                                "the buddha taught the dharma"),
          "parenthetical stripped: 'Dharma (the teaching)' matches 'dharma'");
    CHECK(allcore::glossMatches("things / objects (phenomena)",
                                "all objects are empty"),
          "slash alternatives match independently");
    CHECK(!allcore::glossMatches("is", "this is a draft"),
          "alternatives under 3 letters never match");

    // a draft that uses HGM equivalents
    {
        auto rep = allcore::checkTerminology(
            spine, index, "SANGS RGYAS KYIS CHOS BSTAN",
            "The Buddha taught the Dharma.");
        const allcore::TermUse* sr = nullptr;
        const allcore::TermUse* chos = nullptr;
        for (const auto& t : rep.terms) {
            if (t.wylie == "sangs rgyas") sr = &t;
            if (t.wylie == "chos") chos = &t;
        }
        CHECK(sr != nullptr && chos != nullptr,
              "terms extracted from maximal spans");
        CHECK(sr && !sr->matched.empty(),
              "sangs rgyas matched via its HGM gloss (Buddha)");
        CHECK(chos && !chos->matched.empty(),
              "chos matched via its HGM gloss (Dharma)");
    }

    {   // SQA TEST-1 survivor TERMINOLOGY-NESTING (2026-08-24).
        // A span is dropped when a STRICTLY LONGER glossed span
        // contains it - that is how "sangs rgyas" wins over "sangs".
        // The comparison was `(t.end - t.beg) > (s.end - s.beg)`, and
        // flipping it to `>=` survived the whole battery.
        //
        // What `>=` breaks is homonyms. "shes pa" carries THREE
        // HGM-glossed entries, so a source containing it produces
        // three spans with the SAME extent. Each then "contains" the
        // others at equal length, each suppresses the rest, and the
        // term disappears from the report altogether - a word the
        // master glossed three ways, silently absent from the
        // translator's terminology check.
        auto rep = allcore::checkTerminology(
            spine, index, "SHES PA NI RTOGS PA'O",
            "Consciousness is realization.");
        bool found = false;
        for (const auto& t : rep.terms)
            if (t.wylie == "shes pa") found = true;
        CHECK(found,
              "a homonym with several glossed entries survives the "
              "nesting filter - equal-length spans do not suppress "
              "each other (TEST-1 TERMINOLOGY-NESTING)");
    }

    {   // Maximality never crosses tiers. A longer AUTO-ALIGNED span must
        // not delete a curated term inside it: the row is never built, so no
        // downstream label can rescue it, and the report then reads as an
        // unrendered term for a draft that used his equivalent verbatim.
        //
        // SANGS RGYAS CHOS is the measured repro. Before the tier guard the
        // report held exactly ONE term -- `sangs rgyas chos`, auto-aligned,
        // glossed "sangye chudang tsokyi choknam", which is a pronunciation
        // line rather than English -- and neither curated term at all. Over
        // the corpus, 101,733 GMR-tier terms were being deleted this way, in
        // 65.2% of segments.
        auto rep = allcore::checkTerminology(
            spine, index, "SANGS RGYAS CHOS", "the Buddha's Dharma");
        const allcore::TermUse* sr = nullptr;
        const allcore::TermUse* ch = nullptr;
        for (const auto& t : rep.terms) {
            if (t.wylie == "sangs rgyas") sr = &t;
            if (t.wylie == "chos") ch = &t;
        }
        CHECK(sr && sr->tier == "curated",
              "a curated term survives a longer auto-aligned container "
              "(rule 1: his English outranks the machine's match)");
        CHECK(ch && ch->tier == "curated",
              "and so does the second one");
        CHECK(sr && !sr->matched.empty() && ch && !ch->matched.empty(),
              "and both are reported as RENDERED, because the draft uses "
              "his equivalents verbatim -- the old report called this an "
              "unmatched term");
    }

    // a draft that uses none of them: unmatched terms sort first
    {
        auto rep = allcore::checkTerminology(
            spine, index, "SANGS RGYAS KYIS CHOS BSTAN",
            "Zebras enjoy xylophones.");
        bool anyMatched = false;
        for (const auto& t : rep.terms) anyMatched |= !t.matched.empty();
        CHECK(!rep.terms.empty() && !anyMatched,
              "nonsense draft matches nothing");
        CHECK(rep.terms.front().matched.empty(),
              "unmatched terms are sorted first");
    }

    // repeated term counts occurrences
    {
        auto rep = allcore::checkTerminology(
            spine, index, "BDEN PA DANG BDEN PA", "truth and truth");
        const allcore::TermUse* bp = nullptr;
        for (const auto& t : rep.terms)
            if (t.wylie == "bden pa") bp = &t;
        CHECK(bp && bp->occurrences == 2, "repeated term counted twice");
    }

    // ---- an equivalent must appear as a WORD, not buried in a longer one ---
    // The matcher was a bare substring find(), and on real English that is a
    // false-positive machine. Each of these reported a term as RENDERED when
    // the equivalent never appears as a word at all — and the pane's headline
    // verdict is a count of exactly that judgement. Found 2026-09-11 by
    // probing the shipped matcher rather than by reading it.
    {
        CHECK(!allcore::glossMatches("mind", "he reminded them of the vow"),
              "gloss: \"mind\" does NOT match inside \"reminded\"");
        CHECK(!allcore::glossMatches("art", "a departure from the path"),
              "gloss: \"art\" does NOT match inside \"departure\"");
        CHECK(!allcore::glossMatches("one", "he was honest about it"),
              "gloss: \"one\" does NOT match inside \"honest\"");

        // and the true matches still hold, because a fix that only tightens
        // is a fix that breaks the feature
        CHECK(allcore::glossMatches("mind", "the mind is clear"),
              "gloss: a real occurrence still matches");
        CHECK(allcore::glossMatches("merit", "the merit of giving"),
              "gloss: and so does a term mid-sentence");
        // The contract is that the DRAFT arrives lowered (checkTerminology
        // lowers once and calls this per term). Asserting the contract rather
        // than assuming it, because a caller who forgets gets silent misses.
        CHECK(!allcore::glossMatches("mind", "MIND is clear"),
              "gloss: the draft must arrive lowered — an unlowered draft "
              "misses, and the parameter name and header say so");
        CHECK(allcore::glossMatches("mind", "mind is clear"),
              "gloss: lowered, the same draft matches");

        // punctuation is a word edge, so possessives and hyphenates count
        CHECK(allcore::glossMatches("buddha", "the buddha's own words"),
              "gloss: a possessive still counts as the word");
        CHECK(allcore::glossMatches("being", "well-being of all"),
              "gloss: a hyphenate still counts as the word");

        // a multi-word equivalent is bounded as a whole, not per word
        CHECK(allcore::glossMatches("good deeds", "his good deeds ripen"),
              "gloss: a multi-word equivalent matches as a phrase");
        CHECK(!allcore::glossMatches("good deeds", "good deedsmanship"),
              "gloss: and is bounded at its end like any other word");
    }

    std::printf("%s (%d failures)\n",
                failures ? "TERMINOLOGY SMOKE FAILED" : "TERMINOLOGY SMOKE OK",
                failures);
    return failures ? 1 : 0;
}
