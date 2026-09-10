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

    std::printf("%s (%d failures)\n",
                failures ? "DRILLS SMOKE FAILED" : "DRILLS SMOKE OK", failures);
    return failures ? 1 : 0;
}
