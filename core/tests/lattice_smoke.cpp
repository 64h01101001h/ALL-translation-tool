// lattice_smoke — overlay lattice on real text: nesting, barriers, coverage.
#include <algorithm>
#include <cstdio>
#include <vector>
#include <string>

#include "allcore/lattice.h"
#include "allcore/particles.h"
#include "allcore/spine.h"

static int failures = 0;
#define CHECK(cond, msg)                                        \
    do {                                                        \
        if (cond) std::printf("  [PASS] %s\n", msg);            \
        else { std::printf("  [FAIL] %s\n", msg); ++failures; } \
    } while (0)

int main(int argc, char** argv) {
    if (argc < 2) {
        std::fprintf(stderr, "usage: lattice_smoke <spine.db>\n");
        return 2;
    }
    allcore::Spine spine(argv[1]);

    // the four-truths phrase: nesting is the whole point here
    auto doc = allcore::buildOverlay(spine, "'PHAGS PA'I BDEN PA BZHI LA");
    std::printf("  tokens %zu, spans %zu, entries %zu\n", doc.tokens.size(),
                doc.spans.size(), doc.entries.size());

    auto findSpan = [&](const std::string& wylie) -> const allcore::OverlaySpan* {
        for (const auto& s : doc.spans)
            if (doc.entries[s.entry_ix].wylie == wylie) return &s;
        return nullptr;
    };
    auto* whole = findSpan("'phags pa'i bden pa bzhi");
    auto* mid = findSpan("'phags pa'i bden pa");
    auto* inner = findSpan("bden pa");
    auto* pakpa = findSpan("'phags pa");
    CHECK(whole != nullptr, "span: 'phags pa'i bden pa bzhi (whole phrase)");
    CHECK(mid != nullptr, "span: 'phags pa'i bden pa (nested phrase)");
    CHECK(inner != nullptr, "span: bden pa (nested word)");
    CHECK(pakpa != nullptr && pakpa->clitic == "'i",
          "'phags pa found through the genitive clitic (PA'I → PA + 'i)");
    CHECK(whole && whole->depth == 0, "whole phrase has depth 0 (outermost)");
    CHECK(mid && mid->depth >= 1, "'phags pa'i bden pa nested inside the whole");
    CHECK(inner && inner->depth >= 2, "bden pa is doubly nested (depth >= 2)");

    // innermost-first ordering at the bden-pa position
    if (inner) {
        auto at = doc.spansAt(inner->beg);
        CHECK(!at.empty() &&
                  doc.spans[at.front()].end - doc.spans[at.front()].beg <=
                      doc.spans[at.back()].end - doc.spans[at.back()].beg,
              "spansAt returns innermost first");
    }

    // barriers: a comma must stop phrase matching across it
    auto doc2 = allcore::buildOverlay(spine, "BDEN, PA");
    bool crossed = false;
    for (const auto& s : doc2.spans)
        if (s.end - s.beg > 1) crossed = true;
    CHECK(!crossed, "no span crosses a comma barrier");

    // depth wash: cover depth is bounded and nonzero where spans exist
    auto d = doc.coverDepth(3);
    bool anyCovered = false, bounded = true;
    for (int x : d) { anyCovered |= (x > 0); bounded &= (x <= 3); }
    CHECK(anyCovered && bounded, "cover depth populated and capped");

    // scale check: passage [598] builds without pathology
    const char* p598 =
        "SNGA DRO'I KA BA PHYI DRO MED PA DUS MA 'DRES PA'I DON YIN GYI, "
        "KA BA SNGA DRO YOD NA PHYI DRO MED DGOS PA DUS CHA MA 'DRES PA'I DON MA YIN LA,";
    auto doc3 = allcore::buildOverlay(spine, p598);
    CHECK(doc3.spans.size() > 10 && doc3.spans.size() < 400,
          "passage [598] lattice is populated and sane");

    // ---- Wilson particle layer: fused -s / -r on open stems ----
    // 'PHAGS PAS has no exact entry → 'phags pa + agentive s; PAR alone has no
    // exact entry → pa + la-don r; MED PAR matches its exact entry (med par).
    auto doc5 = allcore::buildOverlay(spine, "'PHAGS PAS MED PAR");
    auto findClitic = [&](const std::string& wylie, const std::string& cl)
        -> const allcore::OverlaySpan* {
        for (const auto& s : doc5.spans)
            if (doc5.entries[s.entry_ix].wylie == wylie && s.clitic == cl)
                return &s;
        return nullptr;
    };
    CHECK(findClitic("'phags pa", "s") != nullptr,
          "'phags pa found through fused agentive (PAS -> PA + s)");
    CHECK(findClitic("pa", "r") != nullptr,
          "pa found through fused la don (PAR -> PA + r)");
    CHECK(findClitic("med par", "") != nullptr,
          "exact entry med par still beats the split at its position");

    // closed syllable must NOT split: SEMS is never sem + s
    auto doc6 = allcore::buildOverlay(spine, "SEMS");
    bool badSplit = false;
    for (const auto& s : doc6.spans) badSplit |= !s.clitic.empty();
    CHECK(!badSplit && !doc6.spans.empty(),
          "SEMS matches exactly, never splits (closed syllable)");

    // freestanding particle classifier (rule tables)
    const auto* kyis = allcore::classifyParticle("KYIS");
    const auto* dang = allcore::classifyParticle("DANG");
    CHECK(kyis && std::string(kyis->function).find("agentive") == 0,
          "classifyParticle: KYIS is agentive");
    CHECK(dang && std::string(dang->function).find("conjunctive") == 0,
          "classifyParticle: DANG is conjunctive");
    CHECK(allcore::classifyParticle("SGRUB") == nullptr,
          "classifyParticle: SGRUB is not a particle");

    // ---- suffix-agreement tables (soft diagnostics) ----
    using A = allcore::Agreement;
    auto agree = [](const char* prev, const char* part) {
        return allcore::checkAgreement(prev, part).verdict;
    };
    CHECK(agree("MED", "KYANG") == A::Agrees, "agreement: MED KYANG (da->kyang)");
    CHECK(agree("BDEN", "GYI") == A::Agrees, "agreement: BDEN GYI (na->gyi)");
    CHECK(agree("DBANG", "GI") == A::Agrees, "agreement: DBANG GI (nga->gi)");
    CHECK(agree("LAS", "SU") == A::Agrees, "agreement: LAS SU (sa->su)");
    CHECK(agree("THABS", "GYI") == A::Disagrees,
          "agreement: THABS GYI flagged (sa wants kyi)");
    CHECK(agree("DGE", "GI") == A::Disagrees,
          "agreement: DGE GI flagged (open stem wants fused 'i / yi)");
    CHECK(agree("SHIN", "TU") == A::DaDragPossible,
          "agreement: SHIN TU is the da-drag case, not an error");
    CHECK(agree("YIN", "KYANG") == A::DaDragPossible,
          "agreement: YIN KYANG is the da-drag case, not an error");
    CHECK(agree("MIG", "DE") == A::NotAParticleVariant,
          "agreement: demonstrative DE is never diagnosed");
    CHECK(agree("BDEN", "LA") == A::NotAParticleVariant,
          "agreement: LA is unrestricted, never diagnosed");

    // ---- indexed fast path must produce the IDENTICAL span set ----
    allcore::HeadwordIndex index(spine);
    std::printf("  headword index: %zu candidates\n", index.size());
    auto sig = [](const allcore::OverlayDoc& d) {
        std::vector<std::string> v;
        for (const auto& s : d.spans)
            v.push_back(std::to_string(s.beg) + ":" + std::to_string(s.end) + ":" +
                        d.entries[s.entry_ix].wylie + ":" + s.clitic + ":" +
                        std::to_string(s.depth));
        std::sort(v.begin(), v.end());
        std::string out;
        for (auto& x : v) out += x + "|";
        return out;
    };
    auto docFast = allcore::buildOverlay(spine, index, p598);
    CHECK(sig(doc3) == sig(docFast),
          "indexed lattice == SQL lattice on passage [598] (span-for-span)");
    auto docA = allcore::buildOverlay(spine, "'PHAGS PA'I BDEN PA BZHI LA");
    auto docB = allcore::buildOverlay(spine, index, "'PHAGS PA'I BDEN PA BZHI LA");
    CHECK(sig(docA) == sig(docB),
          "indexed lattice == SQL lattice on the four-truths phrase");
    auto docC = allcore::buildOverlay(spine, index, "'PHAGS PAS MED PAR");
    CHECK(sig(doc5) == sig(docC),
          "indexed lattice == SQL lattice on fused -s / -r forms");

    std::printf("%s (%d failures)\n",
                failures ? "LATTICE SMOKE FAILED" : "LATTICE SMOKE OK", failures);
    return failures ? 1 : 0;
}
