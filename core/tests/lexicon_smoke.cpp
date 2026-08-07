// lexicon_smoke — the Monlam word-list reference layer.
#include <cstdio>
#include <string>

#include "allcore/engines.h"
#include "allcore/lexicon.h"

static int failures = 0;
#define CHECK(cond, msg)                                        \
    do {                                                        \
        if (cond) std::printf("  [PASS] %s\n", msg);            \
        else { std::printf("  [FAIL] %s\n", msg); ++failures; } \
    } while (0)

int main(int argc, char** argv) {
    if (argc < 3) {
        std::fprintf(stderr,
                     "usage: lexicon_smoke <lexicon1> <lexicon2>\n");
        return 2;
    }
    allcore::RefLexicon lex;
    CHECK(lex.load(argv[1], "Monlam Dictionary"), "list 1 loads");
    CHECK(lex.load(argv[2], "Monlam Grand Dictionary"), "list 2 loads");
    CHECK(lex.size() > 440000, "440k+ words banked (unique: 107,108 + 342,340)");

    // the app path: our own wylie->unicode output must equal the
    // lists' storage form exactly (interior tsheg, no trailing mark)
    auto u = [](const char* w) {
        return allcore::wylieToUnicode(w).first;
    };
    CHECK(!lex.attested(u("bsod nams")).empty(),
          "bsod nams attested via the real conversion path");
    CHECK(lex.attested(u("byang chub sems dpa'")).find("Grand") !=
              std::string::npos,
          "byang chub sems dpa' attested in the Grand list");
    CHECK(lex.attested(u("sangs rgyas")).find("Monlam") == 0,
          "sangs rgyas attested with list label");
    CHECK(lex.attested("ZZZ").empty(), "garbage is unattested");
    CHECK(lex.attested(u("kag dag kag")).empty(),
          "invented phrase is unattested");

    std::printf("%s (%d failures)\n",
                failures ? "LEXICON SMOKE FAILED" : "LEXICON SMOKE OK",
                failures);
    return failures ? 1 : 0;
}
