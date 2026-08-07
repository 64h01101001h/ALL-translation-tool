// wynorm — stdin lines of wylie -> per-token affix-normalized lines.
// The spine builder uses this so the FTS index gains a normalized
// column produced by the SAME rules the app uses (allcore affixnorm)
// — one authority, no python re-implementation.
#include <iostream>
#include <sstream>
#include <string>

#include "allcore/affixnorm.h"

int main() {
    std::ios::sync_with_stdio(false);
    std::string line;
    while (std::getline(std::cin, line)) {
        std::istringstream in(line);
        std::string tok, out;
        while (in >> tok) {
            if (!out.empty()) out += ' ';
            out += allcore::stripAffixedParticlesWylie(tok);
        }
        std::cout << out << '\n';
    }
    return 0;
}
