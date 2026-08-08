// poslex_smoke — the SOAS Classical Tibetan POS lexicon (CC BY 4.0) +
// its one sanctioned use: resolving the Wilson NA/NN dot between two
// bare nominals, evidence-labeled, unambiguous tags only.
#include <cstdio>
#include <string>

#include "allcore/engines.h"
#include "allcore/lattice.h"
#include "allcore/poslex.h"
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
                     "usage: poslex_smoke <classical-lexicon.txt> <spine.db>\n");
        return 2;
    }
    allcore::PosLexicon pos;
    CHECK(pos.load(argv[1]), "lexicon loads");
    std::printf("  %zu distinct forms\n", pos.size());
    CHECK(pos.size() > 13000, "13k+ tsheg-normalized forms");

    auto u = [](const char* w) {
        return allcore::wylieToUnicode(w).first;
    };
    CHECK(pos.unambiguousAdj(u("chen po")), "chen po: unambiguous adj");
    CHECK(pos.unambiguousAdj(u("dkar po")), "dkar po: unambiguous adj");
    CHECK(pos.unambiguousNoun(u("sangs rgyas")),
          "sangs rgyas: unambiguous noun");
    // ཀ appears with AND without tsheg carrying different rows — the
    // loader unions them, so it stays honestly ambiguous
    CHECK(pos.tags(u("ka")).size() >= 4, "ka: rows unioned, stays ambiguous");
    CHECK(!pos.unambiguousAdj(u("ka")) && !pos.unambiguousNoun(u("ka")),
          "ambiguous word decides nothing");
    CHECK(pos.tags(u("kag dag kag")).empty(), "unknown word: no tags");

    // the sanctioned use: the NA dot between noun + adjective
    {
        allcore::Spine spine(argv[2]);
        allcore::HeadwordIndex index(spine);
        auto doc = allcore::buildOverlay(spine, index, "SANGS RGYAS CHEN PO");
        auto cls = allcore::refineClauses(
            doc, allcore::splitClauses(doc.tokens, doc.barrier_after));
        auto parses = allcore::wilsonParse(spine, doc, cls, &pos);
        bool sawNa = false, sawUndet = false;
        for (const auto& cp : parses)
            for (const auto& d : cp.dots) {
                if (d.label.find("NA (adjective") != std::string::npos)
                    sawNa = true;
                if (d.label.find("undetermined") != std::string::npos)
                    sawUndet = true;
            }
        CHECK(sawNa, "sangs rgyas | chen po: NA dot resolved via SOAS");

        // without the lexicon the same dot stays honestly undetermined
        auto plain = allcore::wilsonParse(spine, doc, cls);
        bool plainUndet = false;
        for (const auto& cp : plain)
            for (const auto& d : cp.dots)
                if (d.label.find("undetermined") != std::string::npos)
                    plainUndet = true;
        CHECK(plainUndet, "without POS data the dot stays undetermined");
        (void)sawUndet;

        // ADV: an unambiguous adverb before the verb (kha sang = yesterday)
        CHECK(pos.unambiguousAdv(u("kha sang")), "kha sang: unambiguous adv");
        auto d2 = allcore::buildOverlay(spine, index, "KHA SANG BSTAN");
        auto c2 = allcore::refineClauses(
            d2, allcore::splitClauses(d2.tokens, d2.barrier_after));
        auto p2 = allcore::wilsonParse(spine, d2, c2, &pos);
        bool sawAdv = false;
        for (const auto& cp : p2)
            for (const auto& d : cp.dots)
                if (d.label.find("ADV (adverb") != std::string::npos)
                    sawAdv = true;
        CHECK(sawAdv, "kha sang | bstan: ADV dot resolved via SOAS");
    }

    std::printf("poslex_smoke: %s (%d failures)\n",
                failures ? "FAIL" : "ALL PASS", failures);
    return failures ? 1 : 0;
}
