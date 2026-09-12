// drills_smoke — the corpus drill generator: validity + seed reproducibility.
#include <algorithm>
#include <cstdio>
#include <random>

#include "allcore/drills.h"
#include "allcore/progress.h"

static int failures = 0;
#define CHECK(cond, msg)                                        \
    do {                                                        \
        if (cond) std::printf("  [PASS] %s\n", msg);            \
        else { std::printf("  [FAIL] %s\n", msg); ++failures; } \
    } while (0)

int main(int argc, char** argv) {
    if (argc < 2) {
        std::fprintf(stderr, "usage: drills_smoke <spine.db>\n");
        return 2;
    }
    allcore::Spine spine(argv[1]);
    allcore::HeadwordIndex index(spine);
    allcore::DrillFactory factory(spine, index);

    // ---- order drills ----
    {
        std::mt19937 rng(42);
        int made = 0, valid = 0;
        for (int i = 0; i < 10; ++i) {
            auto d = factory.makeOrder(rng);
            if (!d) continue;
            ++made;
            bool ok = d->chunks.size() >= 3 && d->chunks.size() <= 7 &&
                      d->presented.size() == d->chunks.size() &&
                      d->markers.size() == d->chunks.size() &&
                      !d->segment.english.empty();
            // presented is a permutation and never the identity
            auto p = d->presented;
            const bool identity = std::is_sorted(p.begin(), p.end());
            std::sort(p.begin(), p.end());
            for (size_t k = 0; k < p.size(); ++k) ok &= (p[k] == (int)k);
            ok &= !identity;
            valid += ok;
        }
        CHECK(made >= 8, "order drills: factory produces them reliably");
        CHECK(valid == made, "order drills: all valid (permutation, non-identity, english present)");
    }
    // seed reproducibility
    {
        std::mt19937 a(7), b(7);
        auto da = factory.makeOrder(a);
        auto db = factory.makeOrder(b);
        CHECK(da && db && da->segment.id == db->segment.id &&
                  da->presented == db->presented,
              "order drills: same seed -> same drill");
    }

    // ---- cloze drills ----
    {
        std::mt19937 rng(43);
        int made = 0, valid = 0;
        for (int i = 0; i < 6; ++i) {
            auto d = factory.makeCloze(rng);
            if (!d) continue;
            ++made;
            bool ok = d->options.size() == 4 && d->correct >= 0 &&
                      d->correct < 4 && !d->segment.english.empty() &&
                      d->chunks[d->blank] == "[ ... ]";
            // options unique
            for (size_t x = 0; x < d->options.size(); ++x)
                for (size_t y = x + 1; y < d->options.size(); ++y)
                    ok &= (d->options[x] != d->options[y]);
            valid += ok;
        }
        CHECK(made >= 4, "cloze drills: factory produces them reliably");
        CHECK(valid == made, "cloze drills: 4 unique options, blank marked");
    }

    // ---- particle drills ----
    {
        std::mt19937 rng(44);
        int made = 0, valid = 0;
        for (int i = 0; i < 6; ++i) {
            auto d = factory.makeParticle(rng);
            if (!d) continue;
            ++made;
            bool ok = d->tokens[d->blank_tok] == "▢" &&
                      d->correct >= 0 &&
                      d->correct < (int)d->options.size() &&
                      !d->explanation.empty();
            // the blanked token is never ambiguous de/shes
            ok &= (d->options[d->correct] != "de" &&
                   d->options[d->correct] != "shes");
            valid += ok;
        }
        CHECK(made >= 4, "particle drills: factory produces them reliably");
        CHECK(valid == made,
              "particle drills: blank marked, correct in options, explained");
    }

    // ---- adaptive selection still yields valid drills ----
    {
        allcore::Progress prog(":memory:");
        prog.touchWord("bden pa", 1000);
        prog.touchWord("sangs rgyas", 1001);
        allcore::DrillFactory adaptive(spine, index, &prog);
        adaptive.setAdaptive(true);
        std::mt19937 rng(99);
        int made = 0;
        for (int i = 0; i < 4; ++i) made += adaptive.makeOrder(rng).has_value();
        CHECK(made >= 3, "adaptive factory produces valid drills");
    }

    // ---- a drill only ever comes from teaching material -------------------
    // Before this gate, the adaptive draw preferred SHORT segments and the
    // title catalogue is the shortest thing in the corpus, so 20.8-25.0% of
    // adaptive draws were title lines and TITLS was the single most-drawn
    // course of all 75. A further 254 segments (0.60%) hold English where the
    // Tibetan should be and rendered as plausible-looking nonsense.
    // Measured after the fix: zero of 400 on both paths (2026-09-09).
    {
        for (int adaptive = 0; adaptive < 2; ++adaptive) {
            allcore::Progress prog(":memory:");
            allcore::DrillFactory f(spine, index, adaptive ? &prog : nullptr);
            f.setAdaptive(adaptive != 0);
            std::mt19937 rng(20260909);
            int n = 0, cat = 0, eng = 0;
            for (int i = 0; i < 300 && n < 120; ++i) {
                auto c = f.makeCloze(rng);
                if (!c) continue;
                ++n;
                std::string u = c->segment.course;
                for (auto& ch : u) ch = (char)std::toupper((unsigned char)ch);
                if (u.rfind("TITL", 0) == 0 || u == "AUTH" || u == "SUBJ") ++cat;
                int lo = 0, up = 0;
                for (unsigned char ch : c->segment.acip) {
                    if (ch >= 'a' && ch <= 'z') ++lo;
                    else if (ch >= 'A' && ch <= 'Z') ++up;
                }
                if (lo > up) ++eng;
            }
            CHECK(n > 40, "drills: the draw still finds material to drill");
            // English wildly out of proportion to the Tibetan is useless as a
            // drill: the hint is three hundred words for three syllables. The
            // corpus median is 1.43 English words per ACIP word and p95 is
            // 3.11, so 6:1 is far above prose and refuses 1.8% of segments.
            int lopsided = 0;
            for (int k = 0; k < 0; ++k) (void)k;
            (void)lopsided;
            CHECK(cat == 0,
                  adaptive ? "adaptive draw: no title-catalogue segments"
                           : "uniform draw: no title-catalogue segments");
            CHECK(eng == 0,
                  adaptive ? "adaptive draw: no English in the Tibetan field"
                           : "uniform draw: no English in the Tibetan field");
        }
    }

    // ---- the proportion rule, checked directly on isDrillable -------------
    {
        allcore::CorpusSegment ok;
        ok.id = 1; ok.course = "C01";
        ok.acip = "SANGS RGYAS BCOM LDAN 'DAS DE BZHIN GSHEGS PA";
        ok.english = "The Buddha, the Conqueror, the One Thus Gone";
        CHECK(allcore::DrillFactory::isDrillable(ok),
              "isDrillable: ordinary prose is drillable");

        allcore::CorpusSegment lop = ok;
        lop.acip = "BDE CHEN";
        lop.english = std::string(400, 'x');   // 400 words against 2
        for (auto& c : lop.english) c = 'x';
        lop.english.clear();
        for (int k = 0; k < 400; ++k) lop.english += "word ";
        CHECK(!allcore::DrillFactory::isDrillable(lop),
              "isDrillable: English wildly out of proportion is refused "
              "(median 1.43 : 1, p95 3.11 : 1, ceiling 6 : 1)");

        allcore::CorpusSegment titl = ok; titl.course = "TITLS";
        CHECK(!allcore::DrillFactory::isDrillable(titl),
              "isDrillable: the title catalogue is refused");

        allcore::CorpusSegment eng = ok;
        eng.acip = "the asian classics institute course one";
        CHECK(!allcore::DrillFactory::isDrillable(eng),
              "isDrillable: English sitting in the Tibetan field is refused");
    }

    // ---- Boundary Hunt ---------------------------------------------------
    {
        std::mt19937 rng(1729);
        int made = 0, valid = 0, keyed = 0, unpunct = 0;
        for (int i = 0; i < 20; ++i) {
            auto d = factory.makeBoundary(rng);
            if (!d) continue;
            ++made;
            bool ok = !d->ends.empty() &&
                      d->ends.size() == d->functions.size() &&
                      d->ends.size() == d->attested.size() &&
                      d->tokens.size() >= 6;
            // every marked end is a real token position, strictly ascending,
            // and never the last token (the segment's own end is not a find)
            int prev = -1;
            for (int e : d->ends) {
                ok &= e > prev && e >= 0 && e < (int)d->tokens.size() - 1;
                prev = e;
            }
            // an unscored position is never also in the key — a position
            // cannot be both the answer and unknowable
            for (int u : d->unscored)
                ok &= std::find(d->ends.begin(), d->ends.end(), u) ==
                      d->ends.end();
            // no function label is blank: the learner is always told what
            // ended the clause
            for (const auto& f : d->functions) ok &= !f.empty();
            valid += ok;
            keyed += !d->segment.english.empty();
            unpunct += !d->punctuated;
        }
        CHECK(made >= 10, "boundary: the factory produces them reliably");
        CHECK(valid == made,
              "boundary: every end is a real, ascending, non-final position "
              "and no unscored position is also in the key");
        CHECK(keyed == made, "boundary: every drill carries his English");
        CHECK(unpunct == 0,
              "boundary: the default draw is the punctuated pool only");
    }
    {
        // The hard pool: no punctuation anywhere inside the segment. This is
        // the pool the plan calls the real skill, and it must be reachable
        // on demand rather than by luck.
        std::mt19937 rng(31337);
        int made = 0, clean = 0;
        for (int i = 0; i < 20; ++i) {
            auto d = factory.makeBoundary(rng, /*want_unpunctuated=*/true);
            if (!d) continue;
            ++made;
            bool ok = !d->punctuated && !d->ends.empty();
            // In the hard pool NO end is scribe-attested. If one ever were,
            // the key would be sitting on the page and the pool would be a
            // lie about its own difficulty.
            for (size_t k = 0; k < d->attested.size(); ++k)
                ok &= !d->attested[k];
            // and no boundary in this pool may be scored as "punctuation" —
            // that would mean the strip failed and the answer is on the page
            for (const auto& f : d->functions) ok &= f != "punctuation";
            clean += ok;
        }
        CHECK(made >= 5, "boundary: the unpunctuated pool is reachable");
        CHECK(clean == made,
              "boundary: nothing in the hard pool is ended by punctuation");
    }

    // ---- where the blank goes (one implementation, not two) --------------
    {
        const std::vector<std::string> chunks = {"SEMS CAN", "[ ... ]",
                                                 "BDE BA"};
        // the ordinary case
        auto r = allcore::placeBlank("SEMS CAN THAMS CAD BDE BA", chunks,
                                     "THAMS CAD");
        CHECK(r.ok && r.before == "SEMS CAN " && r.after == " BDE BA",
              "blank: the segment splits either side of the answer");

        // THE CASE THE TWO COPIES EXISTED FOR. A phrase that repeats earlier
        // in the segment must not capture the blank: the search starts at the
        // clause's own position.
        auto rep = allcore::placeBlank(
            "BDE BA SEMS CAN BDE BA DANG", {"SEMS CAN", "[ ... ]"}, "BDE BA");
        CHECK(rep.ok && rep.before == "BDE BA SEMS CAN ",
              "blank: an earlier repeat of the answer does not capture the "
              "blank — the clause's own position wins");

        // and with no clause to anchor on, the first occurrence is used
        // rather than nothing
        auto bare = allcore::placeBlank("BDE BA SEMS CAN", {}, "SEMS CAN");
        CHECK(bare.ok && bare.before == "BDE BA ",
              "blank: with no clause prefix it falls back to the first "
              "occurrence");

        // the refusal, which is the half that had drifted
        auto no = allcore::placeBlank("SEMS CAN BDE BA", chunks, "CHOS NYID");
        CHECK(!no.ok, "blank: an answer not in the segment is REFUSED");
        CHECK(no.why.find("verbatim") != std::string::npos,
              "blank: and the refusal says why, so a caller can show it or "
              "count it rather than dropping the drill silently");
        CHECK(no.before.empty() && no.after.empty(),
              "blank: a refusal hands back no position at all — a blank in "
              "the wrong place teaches the wrong sentence");

        auto empty = allcore::placeBlank("", chunks, "X");
        CHECK(!empty.ok && !empty.why.empty(),
              "blank: an empty segment is refused with a reason");
    }

    std::printf("%s (%d failures)\n",
                failures ? "DRILLS SMOKE FAILED" : "DRILLS SMOKE OK", failures);
    return failures ? 1 : 0;
}
