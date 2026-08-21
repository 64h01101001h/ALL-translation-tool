// wynorm — stdin lines of wylie -> per-token search-folded lines
// (affix strip -> pa/ba fold -> verb-lemma fold when a table is given
// as argv[1]).
// The spine builder uses this so the FTS index gains a normalized
// column produced by the SAME rules the app uses (allcore affixnorm)
// — one authority, no python re-implementation.
#include <iostream>
#include <sstream>
#include <string>

#include "allcore/searchnorm.h"

int main(int argc, char** argv) {
    if (argc > 1) allcore::configureLemmaFold(argv[1]);
    std::ios::sync_with_stdio(false);
    std::string line;
    while (std::getline(std::cin, line)) {
        std::istringstream in(line);
        std::string tok, out;
        while (in >> tok) {
            if (!out.empty()) out += ' ';
            out += allcore::searchFoldWylie(tok);
        }
        std::cout << out << '\n';
    }
    return 0;
}
