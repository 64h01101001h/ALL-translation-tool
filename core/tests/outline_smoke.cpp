// outline_smoke — sa bcad extraction: nesting, sequence, irregular flags.
#include <cstdio>
#include <string>
#include <vector>

#include "allcore/lattice.h"
#include "allcore/outline.h"

static int failures = 0;
#define CHECK(cond, msg)                                        \
    do {                                                        \
        if (cond) std::printf("  [PASS] %s\n", msg);            \
        else { std::printf("  [FAIL] %s\n", msg); ++failures; } \
    } while (0)

static allcore::OutlineNode extract(const std::string& acip) {
    std::vector<std::string> toks;
    std::vector<bool> barriers;
    allcore::tokenizeDocument(acip, toks, barriers);
    return allcore::extractOutline(toks, barriers);
}

int main() {
    setvbuf(stdout, nullptr, _IONBF, 0);

    // classic nested sa bcad:
    // "the Mahayana has three parts. first: generating the mind. second has
    //  two parts: first, preparation; second, the actual body. third: the
    //  conclusion."
    auto root = extract(
        "THEG PA CHEN PO LA GSUM STE, DANG PO NI SEMS BSKYED PA, "
        "GNYIS PA LA GNYIS TE, DANG PO NI SBYOR BA, GNYIS PA NI DNGOS GZHI, "
        "GSUM PA NI RJES");
    CHECK(root.announced == 3, "root announcement: la gsum ste -> 3 parts");
    CHECK(root.heading.find("THEG PA CHEN PO") != std::string::npos,
          "root heading captured from the announcement clause");
    CHECK(root.children.size() == 3, "three top-level parts found");
    if (root.children.size() == 3) {
        CHECK(root.children[0].label == "1" &&
                  root.children[1].label == "2" &&
                  root.children[2].label == "3",
              "top-level labels 1 2 3");
        CHECK(root.children[1].announced == 2,
              "part 2 announces its own two sub-parts");
        CHECK(root.children[1].children.size() == 2,
              "part 2 has two children");
        if (root.children[1].children.size() == 2)
            CHECK(root.children[1].children[0].label == "2.1" &&
                      root.children[1].children[1].label == "2.2",
                  "nested labels 2.1 and 2.2");
        CHECK(root.children[2].heading.find("RJES") != std::string::npos,
              "part 3 heading captured");
        bool anyIrregular = false;
        for (const auto& c : root.children) anyIrregular |= c.irregular;
        CHECK(!anyIrregular, "well-formed outline has no irregular flags");
    }

    // out-of-sequence ordinal is attached but flagged
    auto bad = extract(
        "CHOS LA GSUM STE, DANG PO NI KA BA, GSUM PA NI BUM PA");
    CHECK(bad.children.size() == 2, "skipped ordinal still attaches");
    CHECK(bad.children.size() == 2 && bad.children[1].irregular,
          "skipped ordinal is flagged irregular, not reshuffled");

    // plain text without markers yields an empty outline
    auto none = extract("SEMS CAN THAMS CAD LA PHAN PA BYED");
    CHECK(none.children.empty() && none.announced == 0,
          "no sa bcad markers -> empty outline");

    // ordinal mid-clause without ni/la is not an opener (dang po as words)
    auto lex = extract("KA BA DANG PO TA LA BA");
    CHECK(lex.children.empty(),
          "lexical 'dang po' mid-clause is not an outline marker");

    // ---- structural units (bam po / le'u / shloka estimate) ----
    auto structure = [](const std::string& acip) {
        std::vector<std::string> toks;
        std::vector<bool> barriers;
        allcore::tokenizeDocument(acip, toks, barriers);
        return allcore::extractStructure(toks, barriers);
    };

    // explicit bam po markers with simple and compound ordinals
    auto st = structure(
        "BAM PO DANG PO, SEMS CAN THAMS CAD LA PHAN PA, "
        "BAM PO GNYIS PA, BDE BA DANG LDAN PAR GYUR CIG, "
        "BAM PO BCU GSUM PA, KA BA BUM PA, "
        "BAM PO NYI SHU RTSA GCIG PA, TA LA BA");
    CHECK(st.bampos.size() == 4, "four BAM PO markers found");
    if (st.bampos.size() == 4) {
        CHECK(st.bampos[0].number == 1 && st.bampos[1].number == 2,
              "dang po / gnyis pa ordinals parse");
        CHECK(st.bampos[2].number == 13, "compound teen bcu gsum pa = 13");
        CHECK(st.bampos[3].number == 21,
              "compound nyi shu rtsa gcig pa = 21");
        // 1, 2 then 13: the jump is flagged, never reshuffled
        CHECK(!st.bampos[1].irregular && st.bampos[2].irregular,
              "out-of-sequence bampo flagged irregular");
        CHECK(st.bampos[0].syllables > 0 && st.bampos[1].syllables > 0,
              "per-bampo syllable spans counted");
    }

    // unparsed ordinal is kept raw with number 0, never guessed
    auto raw = structure("BAM PO KHA CIG, KA BA");
    CHECK(raw.bampos.size() == 1 && raw.bampos[0].number == 0,
          "unparsable bampo ordinal kept with number 0");
    CHECK(raw.bampos.empty() ||
              raw.bampos[0].label.find("BAM PO") == 0,
          "raw marker text surfaced in the label");

    // le'u chapter colophon with slar-bsdu 'O; bare le'u mention ignored
    auto ch = structure(
        "ZHES BYA BA'I LE'U STE DANG PO'O, "
        "SNGA MA'I LE'U LAS 'BYUNG NGO, "
        "ZHES BYA BA'I LE'U STE GNYIS PA'O");
    CHECK(ch.chapters.size() == 2, "two numbered le'u colophons found");
    if (ch.chapters.size() == 2)
        CHECK(ch.chapters[0].number == 1 && ch.chapters[1].number == 2 &&
                  !ch.chapters[1].irregular,
              "chapter ordinals with 'O suffix parse in sequence");

    // forms attested in the ACIP canon files (KL0009/KL0021/KL4568):
    // lowercase, bco-teens, nyer-contractions, connector-decades, bare
    // units, slar-bsdu closes
    auto attested = structure(
        "bam po bco lnga pa/ ka ba/ "
        "bam po nyer gnyis/ kha ba/ "
        "bam po sum cu gcig pa/ ga ba/ "
        "bam po zhe gsum/ nga ba/ "
        "bam po gcig go/ ca ba/ "
        "bam po drug");
    CHECK(attested.bampos.size() == 6, "six attested-form markers found");
    if (attested.bampos.size() == 6) {
        CHECK(attested.bampos[0].number == 15, "bco lnga pa = 15");
        CHECK(attested.bampos[1].number == 22, "nyer gnyis = 22");
        CHECK(attested.bampos[2].number == 31,
              "sum cu gcig pa (no connector) = 31");
        CHECK(attested.bampos[3].number == 43,
              "connector-decade zhe gsum = 43");
        CHECK(attested.bampos[4].number == 1, "gcig go (slar bsdu) = 1");
        CHECK(attested.bampos[5].number == 6, "bare drug = 6");
    }
    auto hundred = structure("BAM PO BRGYA DANG NYI SHU RTSA BRGYAD PA");
    CHECK(hundred.bampos.size() == 1 && hundred.bampos[0].number == 128,
          "brgya dang nyi shu rtsa brgyad pa = 128");

    // folio references never count as syllables
    auto folio = structure("@012A KA BA GA");
    CHECK(folio.syllables == 3, "@page tokens excluded from syllables");

    // shloka estimate: 60 syllables of prose ≈ 2 shlokas (30-syl rule)
    {
        std::string prose;
        for (int i = 0; i < 60; ++i) prose += "KA ";
        auto est = structure(prose);
        CHECK(est.syllables == 60, "syllable count exact");
        CHECK(est.shlokaEstimate() > 1.99 && est.shlokaEstimate() < 2.01,
              "shloka estimate: 60 prose syllables = 2.0");
        CHECK(est.bampos.empty() && est.chapters.empty(),
              "plain prose has no structural markers");
    }

    std::printf("%s (%d failures)\n",
                failures ? "OUTLINE SMOKE FAILED" : "OUTLINE SMOKE OK",
                failures);
    return failures ? 1 : 0;
}
