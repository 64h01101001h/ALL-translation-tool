#pragma once
#include <string>

namespace allcore {

// Is a corpus segment's ENGLISH field not English at all?
//
// 702 of the 42,013 segments carrying an English field (1.67%) hold something
// that was never a translation. Two kinds, and both matter because every
// surface that shows one of these labels it "Geshe Michael Roach's English for
// this segment", which is then a false claim:
//
//   the chanted prayers, printed in transliteration so a student can chant
//   along -- "sashi pukyi jukshing metok tram," is the SOUND of its own
//   Tibetan, not a rendering of it;
//
//   the mantras, which are Sanskrit carried over in transliteration --
//   "Om argham praticha sva ha." -- and are not translated by anyone on
//   purpose.
//
// This is also, almost certainly, where the auto-aligned tier learned its
// worst entries: `rab rdzogs` is glossed "sadang lamgyi yunten rabdzok" in the
// shipped spine, which is a chant line from exactly this population.
//
// Two tests, unioned, because each catches what the other cannot:
//
//   soundsLikeItsOwnTibetan -- the English is >=60% inside the pronunciation
//   computed from the segment's own ACIP. Precise, and catches the chants
//   (234 segments). Needs the ACIP, and misses the mantras, whose Sanskrit is
//   not the Tibetan's sound.
//
//   carriesNoEnglishFunctionWord -- four or more words and not one of "the",
//   "of", "is", "and"... Catches the mantras and the chants the first test
//   spells differently. Crude, but a real English sentence of four words with
//   no function word at all is very rare, and the cost of a false positive
//   here is one drill not built.
//
// Callers pass the ACIP when they have it; without it only the second test
// runs and the answer is a floor.
bool englishIsNotEnglish(const std::string& english, const std::string& acip);

// the two halves, exposed so a caller can say WHICH test fired
bool soundsLikeItsOwnTibetan(const std::string& english, const std::string& acip);
bool carriesNoEnglishFunctionWord(const std::string& english);

}  // namespace allcore
