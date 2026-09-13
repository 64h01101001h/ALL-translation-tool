// allcore/cases.h — the eight cases of declension, and transitivity.
//
// Source: Craig Preston, "How to Read Classical Tibetan, Volume One: Summary
// of the General Path" (Snow Lion), front matter pp. xi–xviii and the grammar
// review p. 59 — working in the Hopkins / Napper / Wilson line. Adam supplied
// the pages 2026-09-12.
//
// Why this exists. The reader already knew Wilson's eight VERB classes and
// which particle closed a chunk. It did not know the CASE NUMBER a particle
// declines its noun into, and it did not know whether a verb is transitive. So
// the Walkthrough pane could say "la" and "la don" and could not say what
// Preston's students are drilled to say: which of the eight syntactic
// functions this chunk is filling, and therefore what relationship it bears to
// the verb.
//
// The honest part, and the reason this is a table and not a classifier. The
// la-family particles (la na ru su tu du r) decline into the 2nd, 4th OR 7th
// case, and NOTHING in the particle distinguishes them — the relationship
// does. A machine that picked one would be guessing, so caseOf reports all
// three and says it cannot choose (rule 3). Only the agentive, genitive and
// elative families are unambiguous.
#pragma once

#include <string>
#include <vector>

#include "allcore/verbclass.h"

namespace allcore {

// EIGHT cases, not seven. Preston p.218 lists nineteen case-marking particles
// across eight cases, and the eighth — the vocative — takes NO PARTICLE, the
// same surface as the first. Corrected 2026-09-12 from Adam's scan of p.218;
// the first version of this file said seven and treated a bare noun as proof
// of the nominative, which it is not.
struct CaseReading {
    std::vector<int> cases;      // possible case numbers, ascending; empty = not a case particle
    const char* family = "";     // the Tibetan family name, or ""
    const char* gloss = "";      // what the case does, when unambiguous
    const char* caveat = "";     // what this marking still cannot settle
    bool ambiguous() const { return cases.size() > 1; }
    bool known() const { return !cases.empty(); }
};

// `marker` is the lowercase wylie a Chunk carries ("la", "kyis", "'i", …).
// An empty marker is the FIRST case — unmarked, and that is a fact about the
// noun, not an absence of information.
CaseReading caseOf(const std::string& marker);

// Preston p.218 groups Wilson's eight classes in THREE, not two:
//
//   4 intransitive   nom-nom, nom-loc, nom-obj, nom-s.p.
//   2 transitive     ag-nom, ag-obj
//   2 SPECIALIZED    b/p-nom  (subject in the 4th, object in the 1st)
//                    loc-nom  (subject in the 7th, object in the 1st)
//
// The specialized pair breaks the clean mapping this file shipped with. They
// take a SUBJECT — Preston's word for the intransitive actor — and they also
// take an OBJECT, which he says only transitive verbs have; and his p.219
// entry calls loc-nom outright "class of transitive verbs whose subject is in
// the locative (7th) case". My first version answered isTransitive(LocativeNom)
// = false, which p.219 contradicts.
//
// So the answer is three-valued. Collapsing the specialized classes into
// either bucket would be tidier and would misreport two of the eight.
enum class Transitivity { Intransitive, Transitive, Specialized };

Transitivity transitivityOf(VerbClass c);

// Kept for callers that only need "does this take an agent". A specialized
// verb does NOT — it takes a subject — so this answers false for those, and a
// caller that needs to tell a specialized verb from an intransitive one must
// ask transitivityOf.
bool isTransitive(VerbClass c);

// What Preston calls the actor of this class: "agent" for the two transitive
// classes, "subject" for the four intransitive AND the two specialized.
const char* actorWord(VerbClass c);

// The specialized classes in one sentence, or "" for the other six.
const char* specializedNote(VerbClass c);

// Preston's warning, which a machine most needs: a Tibetan sentence is NOT
// conclusively intransitive because no agent appears in it. Tibetan leaves
// agents unstated far more often than English does. Returns the caution when
// the clause has a transitive verb and no 3rd-case chunk, empty otherwise.
const char* unstatedAgentCaution(VerbClass c, bool sawThirdCase);

}  // namespace allcore
