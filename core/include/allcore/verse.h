// allcore/verse.h — verse meter tools (docs/PEDAGOGY_ROADMAP.md, item 5).
//
// Classical Tibetan verse is syllabic: a stanza's lines share one syllable
// count (7, 9, 11 …). ACIP tokens are syllables, so metrics reduce to
// counting tokens per line (newline- or shad-delimited). The analyzer
// reports the dominant meter and flags lines that break it — a review flag
// for the translator (scribal loss, prose intrusion) and a register signal
// (verse translates differently than prose).
#pragma once

#include <string>
#include <vector>

namespace allcore {

struct VerseLine {
    int number = 0;         // 1-based line number in the input
    int syllables = 0;
    std::string text;       // the line's ACIP
    bool irregular = false; // breaks the dominant meter (only when is_verse)
};

struct VerseReport {
    bool is_verse = false;  // a dominant meter covers most lines
    int meter = 0;          // the dominant syllable count
    int irregular_count = 0;
    std::vector<VerseLine> lines;
};

VerseReport analyzeVerse(const std::string& acip);

}  // namespace allcore
