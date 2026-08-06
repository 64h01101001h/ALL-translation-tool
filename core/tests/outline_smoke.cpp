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

    std::printf("%s (%d failures)\n",
                failures ? "OUTLINE SMOKE FAILED" : "OUTLINE SMOKE OK",
                failures);
    return failures ? 1 : 0;
}
