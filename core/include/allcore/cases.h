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

// The Tibetan grammarians' seven cases (rnam dbye), plus the unmarked first.
struct CaseReading {
    std::vector<int> cases;      // possible case numbers, ascending; empty = not a case particle
    const char* family = "";     // the Tibetan family name, or ""
    const char* gloss = "";      // what the case does, when unambiguous
    bool ambiguous() const { return cases.size() > 1; }
    bool known() const { return !cases.empty(); }
};

// `marker` is the lowercase wylie a Chunk carries ("la", "kyis", "'i", …).
// An empty marker is the FIRST case — unmarked, and that is a fact about the
// noun, not an absence of information.
CaseReading caseOf(const std::string& marker);

// Preston: "subject" belongs to intransitive constructions and "agent" to
// transitive ones, exclusively. Wilson's class names already carry the split —
// the agentive- classes take an agent, the nominative- classes a subject — so
// this reads it off rather than inventing it.
bool isTransitive(VerbClass c);

// "agent" for a transitive verb, "subject" for an intransitive one.
const char* actorWord(VerbClass c);

// Preston's warning, which a machine most needs: a Tibetan sentence is NOT
// conclusively intransitive because no agent appears in it. Tibetan leaves
// agents unstated far more often than English does. Returns the caution when
// the clause has a transitive verb and no 3rd-case chunk, empty otherwise.
const char* unstatedAgentCaution(VerbClass c, bool sawThirdCase);

}  // namespace allcore
