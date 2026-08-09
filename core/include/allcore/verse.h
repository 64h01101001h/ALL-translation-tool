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

// ---- reading-order guidance for verse (docs/PEDAGOGY_ROADMAP.md) ----
// The load-bearing fact for a verse reader: classical verse does NOT
// read line by line. The meter forces the poet to displace words from
// their prose positions and to DROP case particles that the reader must
// supply (Wilson's "understood particle"). So the unit of reading is the
// stanza, assembled and then read by the prose rules. A shloka is four
// verse lines (tshig bcad kyi rkang pa bzhi); many stanzas are also
// closed by a double shad. This groups a verse report's lines into those
// reading units — deterministic: by explicit shad-closed groups when
// present, else by the meter's natural 4-line shloka.

struct VerseStanza {
    int first_line = 0, last_line = 0;   // 1-based, inclusive
    std::vector<std::string> lines;      // the ACIP lines, in order
    bool regular = true;                 // all lines share the meter
};

// raw_acip is the original source (the report's line texts are cleaned
// of trailing shads, so boundary detection needs the raw lines).
std::vector<VerseStanza> groupStanzas(const VerseReport& report,
                                      const std::string& raw_acip);

}  // namespace allcore
