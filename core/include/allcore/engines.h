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

namespace allcore {

// ACIP transliteration → EWTS wylie (TS/TZ, Sanskrit lowercase, long-vowel
// apostrophe K'A → kA vs achung BA'I → ba'i).
std::string acipToEwts(const std::string& acip);

// the inverse (a NEW engine, no canonical original): wylie -> ACIP.
// Proven by round-trip in engines_battery — acipToEwts(ewtsToAcip(w))
// must reproduce w across the full dictionary.
std::string ewtsToAcip(const std::string& ewts);

// wylie (EWTS + IAST-flavored) → Tibetan unicode. Returns {unicode, ok};
// ok=false means at least one syllable failed and is shown as ⟨syl⟩.
std::pair<std::string, bool> wylieToUnicode(const std::string& wylie);

// wylie → GMR-convention phonetics (word-segmented per the printed cards).
//   ← engines/pron_engine.py::pronounce
std::string pronounce(const std::string& wylie);

}  // namespace allcore
