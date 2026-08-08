// allcore/wilsonparse.h — the full per-unit grammatical parse (Wilson's
// designations, "Translating Buddhism from Tibetan").
//
// Every atom of a clause gets a designation: case-marking particles with
// their EIGHT-CASE number — assigned contextually, because the same la-group
// particle is 2nd case (destination) under a motion verb, 4th/7th (subject)
// under necessity/possession, and 7th (referential) under attitude verbs;
// the verb-class tables make that synthesis deterministic. Syntactic and
// lexical particles, verbs (class + tense row), verbal nouns, pronouns,
// numbers, and postpositions come from rule tables; everything else is
// honestly labeled a dictionary term (adjective/adverb designation is NOT
// guessed — it waits for a real data source) or unresolved.
#pragma once

#include <string>
#include <vector>

#include "allcore/lattice.h"
#include "allcore/reader.h"

namespace allcore {

struct ParseUnit {
    int beg = 0, end = 0;      // token span
    std::string text;          // ACIP as written
    std::string category;      // e.g. "case particle — 3rd (agentive)"
    std::string detail;        // paradigm, tense row, first gloss, agreement…
};

// One tsheg, designated (Wilson's Science of the Dots). Labels follow the
// book's abbreviations: S (within a word), C n (around a case particle, with
// the case number), NOM (after a nominative noun), SP (around a syntactic
// particle), V (after an open verb / infinitive), VB (after a final verb).
// Where only POS data could decide (NOM vs NA/NN/APP between two bare
// nouns), the dot is honestly marked undetermined.
struct DotMark {
    int after_tok = 0;         // the dot follows this token
    std::string label;
};

struct ClauseParse {
    Clause clause;
    VerbGuess verb;
    std::vector<ParseUnit> units;
    std::vector<DotMark> dots;
};

class PosLexicon;    // allcore/poslex.h — SOAS hand-tagged POS data
class Contractions;  // allcore/contractions.h — derived bsdus-tshig register

// Parse a loaded document clause by clause (clauses come pre-refined).
// pos (optional): the SOAS POS lexicon — consulted ONLY where Wilson's
// rules require POS data (the NOM vs NA/NN/APP dot between two bare
// nominals), unambiguous tags only, evidence named in the label.
// contractions (optional): the derived bsdus-tshig register — a
// two-syllable unit found in it gets its internal dot marked OM
// (contraction), expansion + derived tier named in the label.
std::vector<ClauseParse> wilsonParse(const Spine& spine,
                                     const OverlayDoc& doc,
                                     const std::vector<Clause>& clauses,
                                     const PosLexicon* pos = nullptr,
                                     const Contractions* contractions = nullptr);

}  // namespace allcore
