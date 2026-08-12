// allcore/engines.h — C++ ports of the canonical Python engines (roadmap
// item 7). Founding rule 2: each port is transcribed from the canonical file
// in engines/ and proven by its validation battery (engines_battery test);
// a port that fails its battery does not ship.
//
//   acipToEwts      ← engines/hgm_tools.py::acip_to_ewts
//   wylieToUnicode  ← engines/ewts_unicode.py::wylie_to_unicode (v0.27.1)
//
// Failed syllables render as ⟨wylie⟩ markers with ok=false — never guessed
// (founding rule 3).
#pragma once

#include <string>
#include <utility>
#include <vector>

namespace allcore {

// ACIP transliteration → EWTS wylie (TS/TZ, Sanskrit lowercase, long-vowel
// apostrophe K'A → kA vs achung BA'I → ba'i).
std::string acipToEwts(const std::string& acip);

// the inverse (a NEW engine, no canonical original): wylie -> ACIP.
// Proven by round-trip in engines_battery — acipToEwts(ewtsToAcip(w))
// must reproduce w across the full dictionary.
std::string ewtsToAcip(const std::string& ewts);

// Is this document wylie (EWTS) rather than ACIP? ACIP writes its
// letters uppercase (lowercase appears only in Sanskrit passages and
// special marks), wylie is lowercase throughout — so the dominant case
// of the alphabetic characters decides. Used by the Overlay to route
// per-token conversion; battery-covered in engines_battery.
bool looksLikeWylie(const std::string& text);

// Per-token variant: convert a token to EWTS if it carries ACIP's
// uppercase letters, pass wylie through untouched. The lattice has
// always normalized this way (why matching worked for wylie docs);
// promoted here so display/export paths share the exact idiom.
std::string tokenToEwts(const std::string& tok);

// wylie (EWTS + IAST-flavored) → Tibetan unicode. Returns {unicode, ok};
// ok=false means at least one syllable failed and is shown as ⟨syl⟩.
std::pair<std::string, bool> wylieToUnicode(const std::string& wylie);

// wylie → GMR-convention phonetics (word-segmented per the printed cards).
//   ← engines/pron_engine.py::pronounce
std::string pronounce(const std::string& wylie);

// pronounce with the engine's OWN word segmentation surfaced: each
// word carries its pron and the half-open syllable span [syl_beg,
// syl_end) it consumed from the normalized whitespace-split input.
// ADDITIVE view of the same canonical logic — joining the non-empty
// prons with spaces is IDENTICAL to pronounce() (asserted across the
// full reference dump in the battery). Empty prons are kept so every
// input syllable stays accounted for (display fallback decides).
struct PronSegWord {
    std::string pron;
    int syl_beg = 0;
    int syl_end = 0;
};
std::vector<PronSegWord> pronounceSegmented(const std::string& wylie);

}  // namespace allcore
