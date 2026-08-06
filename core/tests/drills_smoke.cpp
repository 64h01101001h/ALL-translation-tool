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

    std::printf("%s (%d failures)\n",
                failures ? "DRILLS SMOKE FAILED" : "DRILLS SMOKE OK", failures);
    return failures ? 1 : 0;
}
