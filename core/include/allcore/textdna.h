// allcore/textdna.h — the Text DNA strip (idea bank, 2026-08-20):
// a text's structure as one class per line, so a whole work's shape
// reads at a glance — verse blocks, prose stretches, quote-bearing
// lines, blanks. Pure measurement over the same line/syllable
// arithmetic the verse tools use (verse.h); nothing is inferred
// beyond the classes' stated definitions.
#pragma once

#include <string>
#include <vector>

namespace allcore {

enum class DnaClass {
    Blank,            // no syllables on the line
    Prose,            // syllable-bearing, outside the dominant meter
    Verse,            // matches the document's dominant meter
    VerseIrregular,   // inside a verse document but off the meter
};

struct DnaLine {
    int number = 0;        // 1-based source line
    DnaClass cls = DnaClass::Blank;
    int syllables = 0;
    bool quote_mark = false;   // carries a ZHES/CES quote-end marker
                               // followed nearby by a speech verb —
                               // the citations lane's announcement
                               // shape, per line
};

struct DnaReport {
    bool is_verse = false;     // from the verse analyzer
    int meter = 0;
    std::vector<DnaLine> lines;
};

DnaReport textDna(const std::string& acip);

}  // namespace allcore
