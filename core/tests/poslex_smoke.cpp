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

        {   // Rule 1 in the Wilson layer: a gloss that came from an
            // AUTO-ALIGNED entry must announce itself. ParseUnit had no field
            // for the tier, so the Trainer printed the machine's English in
            // the same plain grey as a rule-table fact -- and with the gloss
            // layer switched off, that unmarked line was the only place the
            // gloss appeared at all.
            auto pdoc = allcore::buildOverlay(spine, index, "DKU LA");
            auto pcls = allcore::refineClauses(
                pdoc, allcore::splitClauses(pdoc.tokens, pdoc.barrier_after));
            auto pp = allcore::wilsonParse(spine, pdoc, pcls);
            bool sawGloss = false, everMarked = false, unmarkedProv = false;
            for (const auto& cp : pp)
                for (const auto& un : cp.units) {
                    if (un.detail.find("\u2261") == std::string::npos) continue;
                    sawGloss = true;
                    if (un.detail_provisional) everMarked = true;
                    // the entry the unit came from decides the truth
                    for (const auto& sp : pdoc.spans) {
                        if (sp.beg != un.beg || sp.end != un.end) continue;
                        if (sp.entry_ix < 0) continue;
                        const auto& ent = pdoc.entries[sp.entry_ix];
                        if (ent.provisional() && !ent.hgm_gloss.empty() &&
                            un.detail.find(ent.hgm_gloss.front()) !=
                                std::string::npos &&
                            !un.detail_provisional)
                            unmarkedProv = true;
                    }
                }
            CHECK(sawGloss, "wilson: the probe passage does carry a gloss");
            CHECK(everMarked,
                  "wilson: an auto-aligned gloss is flagged provisional so the "
                  "Trainer can badge it (rule 1)");
            CHECK(!unmarkedProv,
                  "wilson: no provisional gloss reaches the parse line without "
                  "its tier -- the machine's match never wears Geshe Michael's authority");
        }
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

        // UP: Wilson's understood-particle dot, attested-only (p.573 —
        // resolved because OUR corpus attests the uncontracted form
        // "tshul khrims kyi phung po"; no POS lexicon needed)
        auto d3 = allcore::buildOverlay(spine, index, "TSHUL KHRIMS PHUNG PO");
        auto c3 = allcore::refineClauses(
            d3, allcore::splitClauses(d3.tokens, d3.barrier_after));
        auto p3 = allcore::wilsonParse(spine, d3, c3);
        bool sawUp = false;
        for (const auto& cp : p3)
            for (const auto& d : cp.dots)
                if (d.label.find("UP (6") != std::string::npos &&
                    d.label.find("attested uncontracted") != std::string::npos)
                    sawUp = true;
        CHECK(sawUp,
              "tshul khrims | phung po: UP (6) dot resolved by attestation");

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
