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
// Old-Tibetan: the da-drag strip IS ported (final ད after ན/ར/ལ —
// gyurd→gyur, stond→ston; grounded in the same historical-da-drag
// model the particles layer ships). The filter's medial-འ rule
// remains unported: no canonical source in-repo to port from, and
// rules are never reconstructed from memory.
// Search-layer normalization ONLY: display, parsing, and the
// dictionary never see normalized forms.

#include <string>

namespace allcore {

// unicode syllable in, normalized unicode syllable out (unchanged if
// no affix rule applies)
std::string stripAffixedParticles(const std::string& syllable_unicode);

// wylie-level convenience: strips the same particles from the final
// syllable of a wylie word by suffix comparison, accepting the strip
// only when the unicode port agrees (the unicode rules ARE the
// authority — this is a projection, not a second rule set). Returns
// the input unchanged when no strip applies.
std::string stripAffixedParticlesWylie(const std::string& wylie_word);

// Old-Tibetan da-drag: strip the historical second-suffix ད after
// ན/ར/ལ (gyurd→gyur, kund→kun, brtsald→brtsal). Search-layer only.
// Safe by construction: no classical syllable ends in those
// clusters — the ད there is always the archaic da-drag.
std::string stripDaDrag(const std::string& syllable_unicode);
std::string stripDaDragWylie(const std::string& wylie_word);

}  // namespace allcore
