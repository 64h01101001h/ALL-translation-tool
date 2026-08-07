#pragma once
// affixnorm.h — Tibetan affixed-particle normalization for search.
//
// A faithful C++ port of BDRC's TibAffixedFilter (buda-base/lucene-bo,
// Apache-2.0, © 2017 BDRC; authors Chris Tomlinson, Hélios Hildt) —
// the affix-stripping rules behind BDRC's production Tibetan search.
// Removes the affixed particles འི, འོ, འིས, འམ, འང (and the stacked
// forms འིའོ/འིའམ/འིའང/འོའམ/འོའང, and the འུར/འུས endings) from a
// syllable so that པོ and པོའི match in search — EXCEPT where the
// syllable's own final འ is part of the word (dga', mkha' …), decided
// by the same prefix/main-stack table the original uses.
//
// Classical-Tibetan path only; the filter's Old-Tibetan rules
// (da-drag, medial འ) are deliberately not ported yet.
// Search-layer normalization ONLY: display, parsing, and the
// dictionary never see normalized forms.

#include <string>

namespace allcore {

// unicode syllable in, normalized unicode syllable out (unchanged if
// no affix rule applies)
std::string stripAffixedParticles(const std::string& syllable_unicode);

}  // namespace allcore
