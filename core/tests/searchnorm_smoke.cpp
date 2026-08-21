// searchnorm_smoke — the remaining lucene-bo analyzers.
#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>

#include "allcore/engines.h"
#include "allcore/searchnorm.h"

static int failures = 0;
#define CHECK(cond, msg)                                        \
    do {                                                        \
        if (cond) std::printf("  [PASS] %s\n", msg);            \
        else { std::printf("  [FAIL] %s\n", msg); ++failures; } \
    } while (0)

int main(int argc, char** argv) {
    if (argc < 2) {
        std::fprintf(stderr,
                     "usage: searchnorm_smoke <verb_lemmas.tsv>\n");
        return 2;
    }
    using namespace allcore;

    // PaBaFilter semantics: standalone only, post-strip
    CHECK(paBaFoldWylie("ba") == "pa" && paBaFoldWylie("bo") == "po",
          "ba->pa, bo->po");
    CHECK(paBaFoldWylie("bar") == "bar" && paBaFoldWylie("dbang") ==
              "dbang",
          "non-standalone untouched");
    CHECK(searchFoldWylie("ba'i") == "pa",
          "chained: ba'i -> (strip) ba -> (fold) pa");

    // lemma table: EVERY row round-trips through our own converter
    // against the bank's unicode — pyewts was bootstrap only
    {
        std::ifstream f(argv[1]);
        std::string line;
        int rows = 0, rt = 0;
        while (std::getline(f, line)) {
            if (line.empty() || line[0] == '#') continue;
            std::istringstream in(line);
            std::string wf, wp, uf, up;
            std::getline(in, wf, '\t');
            std::getline(in, wp, '\t');
            std::getline(in, uf, '\t');
            std::getline(in, up, '\t');
            ++rows;
            if (wylieToUnicode(wf).first == uf &&
                wylieToUnicode(wp).first == up)
                ++rt;
        }
        std::printf("  lemma table: %d/%d rows round-trip exactly\n",
                    rt, rows);
        CHECK(rows > 1500 && rt == rows,
              "full-table round-trip through our converter");
    }
    configureLemmaFold(argv[1]);
    CHECK(globalLemmaFold() != nullptr, "global fold configured");
    {
        const auto* v = globalLemmaFold()->fold("bklubs");
        CHECK(v && *v == "klub", "bklubs -> klub (unique present)");
        CHECK(globalLemmaFold()->fold("byed") == nullptr,
              "present stems never fold");
        CHECK(searchFoldWylie("bklubs") == "klub",
              "chained fold reaches the lemma");
    }

    // unicode normalization: upstream examples
    CHECK(normalizeTibetanUnicode("དྲངསྟེ") == "དྲངས་ཏེ",
          "MergedSyl1: drangste -> drangs te");
    CHECK(normalizeTibetanUnicode("སྟགི") == "སྟག་གི",
          "MergedSyl3: stagi -> stag gi");
    CHECK(normalizeTibetanUnicode("པདྨ") == "པད་མ",
          "lenient: padma respelled pad+ma");
    CHECK(normalizeTibetanUnicode("གྷ") ==
              normalizeTibetanUnicode("གྷ"),
          "gha decomposes to the same form either way");
    CHECK(normalizeTibetanUnicode("ཊ") == "ཏ" &&
              normalizeTibetanUnicode("ཊ", false) == "ཊ",
          "retroflex folds only in lenient mode");
    CHECK(normalizeTibetanUnicode("བཀའ") == "བཀའ",
          "plain classical text passes through");

    std::printf("%s (%d failures)\n",
                failures ? "SEARCHNORM SMOKE FAILED"
                         : "SEARCHNORM SMOKE OK",
                failures);
    return failures ? 1 : 0;
}
