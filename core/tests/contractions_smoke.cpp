// contractions_smoke — the derived bsdus-tshig register + the OM dot.
// The register is attestation-only (both forms glossed HGM entries,
// Wilson drop-pattern, gloss agreement); the battery pins the classics,
// the false-class exclusion (tail-only drops are NOT contractions), and
// Wilson's own OM illustration: rnam shes mi rtag pa yin (App.2 p.570),
// whose first dot must read OM with the expansion named — and plain S
// without the register.
#include <cstdio>
#include <string>

#include "allcore/contractions.h"
#include "allcore/lattice.h"
#include "allcore/reader.h"
#include "allcore/spine.h"
#include "allcore/wilsonparse.h"

static int failures = 0;
#define CHECK(cond, msg)                                        \
    do {                                                        \
        if (cond) std::printf("  [PASS] %s\n", msg);            \
        else { std::printf("  [FAIL] %s\n", msg); ++failures; } \
    } while (0)

int main(int argc, char** argv) {
    if (argc < 3) {
        std::fprintf(stderr,
                     "usage: contractions_smoke <bsdus_tshig.tsv> <spine.db>\n");
        return 2;
    }
    allcore::Contractions reg;
    CHECK(reg.load(argv[1]), "register loads");
    std::printf("  %zu derived pairs\n", reg.size());
    CHECK(reg.size() > 150, "150+ attested contraction pairs");

    auto rs = reg.expansions("rnam shes");
    bool found = false;
    for (const auto* c : rs)
        if (c->longWylie == "rnam par shes pa" && c->cls == "clean" &&
            c->glossKind == "exact")
            found = true;
    CHECK(found, "rnam shes -> rnam par shes pa (clean, gloss-exact)");
    CHECK(!reg.expansions("theg chen").empty(), "theg chen registered");
    CHECK(!reg.expansions("stong nyid").empty(), "stong nyid registered");
    CHECK(reg.expansions("ma yin").empty(),
          "ma yin NOT registered (tail-only drop is no contraction)");
    CHECK(reg.expansions("kag dag").empty(), "unknown form empty");

    // letter-level fusions (2026-08-08): first syllable fused from the
    // long form's first two (onset+vowel + next first consonant),
    // admitted on exact gloss only
    auto sherOk = false;
    for (const auto* c : reg.expansions("sher phyin"))
        if (c->longWylie == "shes rab kyi pha rol tu phyin pa" &&
            c->cls == "fused" && c->glossKind == "exact")
            sherOk = true;
    CHECK(sherOk, "sher phyin -> shes rab kyi pha rol tu phyin pa "
                  "(fused: shes rab -> sher)");
    auto myangOk = false;
    for (const auto* c : reg.expansions("myang 'das"))
        if (c->cls == "fused") myangOk = true;
    CHECK(myangOk, "myang 'das (mya ngan -> myang) fused class present");
    CHECK(!reg.expansions("phar phyin").empty(),
          "phar phyin (pha rol -> phar) registered");

    // Wilson's own OM example: the first dot of rnam shes mi rtag pa yin
    {
        allcore::Spine spine(argv[2]);
        allcore::HeadwordIndex index(spine);
        auto doc =
            allcore::buildOverlay(spine, index, "RNAM SHES MI RTAG PA YIN");
        auto cls = allcore::refineClauses(
            doc, allcore::splitClauses(doc.tokens, doc.barrier_after));
        auto withReg = allcore::wilsonParse(spine, doc, cls, nullptr, &reg);
        bool sawOm = false;
        for (const auto& cp : withReg)
            for (const auto& d : cp.dots)
                if (d.label.rfind("OM (contraction of rnam par shes pa", 0) ==
                    0)
                    sawOm = true;
        CHECK(sawOm, "rnam shes internal dot reads OM with the expansion");

        auto without = allcore::wilsonParse(spine, doc, cls);
        bool anyOm = false;
        for (const auto& cp : without)
            for (const auto& d : cp.dots)
                if (d.label.rfind("OM", 0) == 0) anyOm = true;
        CHECK(!anyOm, "without the register the dot stays S (honest default)");
    }

    std::printf("contractions_smoke: %s (%d failures)\n",
                failures ? "FAIL" : "ALL PASS", failures);
    return failures ? 1 : 0;
}
