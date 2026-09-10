// speak_battery — the C++ audio resolver must agree with the Python it was
// ported from, syllable for syllable, across every syllable in the corpus and
// the dictionary (11,940 of them).
//
// Founding rule 2: a port is proved by its battery, not by inspection. The
// reference is produced by tools/dump_speak_reference.py from
// tools/kawachen_speak.py, the canonical resolver.
//
// Exits 77 (CTest SKIP) when the audio is absent: it is a gitignored local
// asset held under in-house permission, so a fresh clone has none and that is
// not a failure.
#include <cstdio>
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "allcore/speak.h"

namespace {

std::string basename(const std::string& p) {
    const size_t s = p.find_last_of('/');
    return s == std::string::npos ? p : p.substr(s + 1);
}

const char* tierWord(allcore::SpeakTier t) {
    switch (t) {
        case allcore::SpeakTier::Exact:         return "exact";
        case allcore::SpeakTier::SilentDropped: return "silent";
        case allcore::SpeakTier::BaseEnding:    return "ending";
        case allcore::SpeakTier::Reduced:       return "reduced";
        default:                                return "none";
    }
}

}  // namespace

int main(int argc, char** argv) {
    const std::string root = argc > 1 ? argv[1] : "data/audio/kawachen";
    const std::string ref  = argc > 2 ? argv[2] : "build/speak_reference.tsv";

    allcore::SpeakBank bank(root);
    if (!bank.available()) {
        std::printf("no Kawachen audio in this tree - nothing to prove\n");
        return 77;
    }
    std::ifstream in(ref);
    if (!in) {
        std::printf("no reference at %s - run tools/dump_speak_reference.py\n",
                    ref.c_str());
        return 77;
    }
    std::printf("bank: %d recordings, %d distinct readings\n",
                bank.recordings(), bank.readings());

    int checked = 0, tierBad = 0, fileBad = 0;
    std::map<std::string, int> firstFew;
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::istringstream ls(line);
        std::string syl, tier, files;
        std::getline(ls, syl, '\t');
        std::getline(ls, tier, '\t');
        std::getline(ls, files, '\t');

        const allcore::SpokenSyllable got = bank.resolve(syl);
        ++checked;

        if (tierWord(got.tier) != tier) {
            if (++tierBad <= 8)
                std::printf("  TIER  %-12s python=%-8s cpp=%s\n",
                            syl.c_str(), tier.c_str(), tierWord(got.tier));
            continue;
        }
        std::string mine;
        for (const std::string& f : got.files) {
            if (!mine.empty()) mine += '|';
            mine += basename(f);
        }
        if (mine != files) {
            if (++fileBad <= 8)
                std::printf("  FILE  %-12s python=%-40s cpp=%s\n",
                            syl.c_str(), files.c_str(), mine.c_str());
        }
    }
    std::printf("\nchecked %d syllables\n", checked);
    std::printf("  tier disagreements : %d\n", tierBad);
    std::printf("  file disagreements : %d\n", fileBad);
    if (tierBad || fileBad) {
        std::printf("PORT DOES NOT MATCH ITS ORACLE\n");
        return 1;
    }
    std::printf("the C++ resolver reproduces the Python exactly\n");
    return 0;
}
