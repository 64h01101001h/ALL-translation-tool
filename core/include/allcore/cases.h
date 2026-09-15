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

// ---- Preston's p.xv chart, inverted ---------------------------------------
//
// The chart reads forwards: each of the eight syntactic functions lists the
// cases it may be declined into.
//
//     agent of a transitive verb        3rd
//     object of a transitive verb       1st, 2nd
//     complement to the object          1st, 2nd
//     subject of an intransitive verb   1st, 4th, 7th
//     complement to the subject         1st, 2nd
//     qualifier of the verb             2nd, 3rd, 4th, 5th, 7th, s.p.
//
// A reader with a chunk in front of them needs it BACKWARDS: given this case,
// what can this chunk be doing? Inverting it is arithmetic, not judgement,
// and the answer is a SET. Only the fifth case names one function.
//
// Then one deterministic filter, from Preston's two structural exclusions,
// both stated verbatim on p.xvi-xviii: a transitive clause has no subject and
// no complement to a subject; an intransitive clause has no agent and no
// object. The verb's class shrinks the set without inferring anything.
//
// This exists because the engine's role labels commit to ONE function each —
// "la don, to/at/in/for" for la, which is the qualifier reading — and Preston
// diagrams the same fused particle as the complement in both of his worked
// examples. A label that names one of four possibilities as though it were
// the answer is the same overreach as resolving a la don particle to a single
// case, and this file already refuses that.
enum class Function {
    AgentOfTransitive,
    ObjectOfTransitive,
    ComplementToObject,
    SubjectOfIntransitive,
    ComplementToSubject,
    QualifierOfVerb,
    Vocative,
};

const char* functionName(Function f);

// Every function the given case admits, before the verb is considered.
std::vector<Function> functionsForCase(int case_number);

// ... and after. `t` narrows by Preston's two exclusions. Pass
// Transitivity::Specialized to apply neither: those classes carry a subject
// AND an object, so neither exclusion holds for them.
std::vector<Function> functionsFor(int case_number, Transitivity t);

// Wilson's nom-s.p. classes take a qualifier marked by a SYNTACTIC particle
// rather than by a case, which is the "s.p." entry in Preston's p.xv chart
// alongside the 2nd/3rd/4th/5th/7th.
//
// Three of the four are already reachable through the case tables — the
// separative's las/nas is the 5th, and the absence verbs' gis-family is the
// 3rd, and functionsForCase keeps the qualifier reading open for both. The
// fourth is not: `dang` is not a case particle at all, so caseOf returns
// nothing for it and the Walkthrough printed an em dash in the column that is
// supposed to say what a chunk could be doing.
//
// Our own tables already knew better. verbclass.cpp records, for every
// conjunctive and disjunctive verb, that "the dang chunk completes the verb".
// This reads that off rather than deciding it: nothing here is new grammar,
// it is one table finally being asked what another already recorded.
// (Adversarial pass over Preston, 2026-09-13.)
bool syntacticParticleMarksQualifier(const std::string& marker, VerbClass c);

// The la don particles decline into the 2nd, 4th or 7th and the PARTICLE
// cannot choose between them — but the verb often can, and wilsonparse.cpp's
// caseLabel has been doing exactly that since long before this file existed:
// 2nd under a verb of motion, 7th under a verb of existence, 4th under a verb
// of necessity, and so on.
//
// Which left the tool disagreeing with itself in front of the reader. The
// Wilson Parse pane says "7th (locative)" about the same chunk the Walkthrough
// calls "2/4/7 — the particle cannot tell you which". Both are defensible
// alone; together they are two panes contradicting each other on one word.
//
// This resolves it towards the table that knows more. The honesty is
// unchanged — the particle still cannot choose, and the caller must say that
// the VERB is what narrowed it — but information we already hold is no longer
// thrown away.
//
// DUPLICATION, ACKNOWLEDGED: this mirrors logic inside caseLabel rather than
// replacing it, which makes a sixth table of the kind table_conformance
// exists to police. It is therefore gated there against caseLabel for every
// verb class. The real repair is for wilsonparse.cpp to call this, and it is
// not attempted here because caseLabel also builds display strings for a
// shipped pane. (Adversarial pass over Preston, 2026-09-13.)
struct LaDonNarrowing {
    std::vector<int> cases;   // {2,4,7} when the verb does not narrow it
    const char* because = ""; // what about the verb decided, or ""
    bool narrowed() const { return cases.size() == 1; }
};
LaDonNarrowing narrowLaDon(const std::string& marker, const VerbClassInfo* cls);

// ATTRIBUTIVE SYNTAX — the counterexample to the engine's central assumption.
//
// Everything in this file so far treats a verb's Wilson class as settling the
// shape of its clause. Preston's volume two says otherwise on four separate
// pages, in four separate wordings:
//
//   p.60   attributive syntax, "wherein something is called something else…
//          something in the 7th case is being called something in the 1st"
//   p.70   over the verb: "normally transitive, but here used with an
//          attributive syntax"
//   p.163  "Attributive Syntax: A refers to B" — the verb byed, normally
//          ag-nom, diagrammed (nom-loc)
//   p.200  "the normally transitive verb is used with a nom-loc syntax"
//
// And p.219's abbreviation list already had the names for it: attrib. loc-nom
// (attributive subject in the 7th, object in the 1st) and attrib. nom-loc
// (subject in the 1st, object in the 7th).
//
// So a normally-agentive verb can take a SUBJECT rather than an agent, with
// what is attributed sitting in the 7th case. The pane has been announcing
// "this is a transitive verb, so the clause has an agent, not a subject" as
// though the class settled it. For an ag-nom verb with no third-case chunk
// and a la-group chunk that could be a 7th, it does not.
//
// This does not DETECT attributive syntax — nothing here can, and a detector
// would be the guess rule 3 forbids. It reports that the reading is open.
// Returns "" when the shape does not fit.
const char* attributiveSyntaxOpen(const VerbClassInfo* cls, bool sawThirdCase,
                                  bool sawLaGroup);

// THE IMPLIED LINKING VERB — named, because Preston vol. 2 p.44 says it is
// "by far the most frequently seen sentence structure" in the tenets
// literature, and our engine had no name for it at all.
//
// The linking-verb sentence puts BOTH the subject and its complement in the
// first case and ends with yin (nom-nom): A is B. The implied variant drops
// the verb, which leaves two unmarked noun phrases and nothing else — exactly
// the shape spotVerb cannot handle, since it will name the last dictionary
// word as an unverified candidate and the reader cannot tell that from a real
// verb the glossary happens not to mark.
//
// The signal is precise: no chunk carries a case that fills a CLAUSE-level
// slot other than the first, and no verb is attested. A sixth-case chunk does
// not disqualify it — the genitive joins nouns inside a phrase and fills no
// clause-level slot, which functionsForCase already records by answering the
// empty set for it.
//
// Still a report, not a ruling. It names the reading and its evidence; it
// does not assert that the verb is yin. Returns "" when the shape does not fit.
// (Preston vol. 2 p.44, 2026-09-13.)
const char* impliedLinkingVerbLikely(bool verbAttested,
                                     bool anyClauseLevelCaseBeyondFirst);

// THE IMPLIED VERB OF EXISTENCE OR POSSESSION — a second implied-verb shape,
// and unlike the linking one it has a signal on the page.
//
// Preston vol. 2 p.53: the challenge of the sentence is that "it ends not with
// a verb, but with a verbal noun followed by a terminating syntactic
// particle", and the way to read it is to imagine an implicit yod at the end.
//
// That is mechanically checkable, and both halves are things the reader
// already knows: a verbal noun is formed with pa/ba (p.xxv), and
// isFinalParticle in reader.h already identifies a terminating particle by
// the suffix-echo rule.
//
// yod takes a subject in the SEVENTH case and an object in the FIRST, and
// serves both existence ("something exists somewhere") and possession
// ("something has something else") — one verb with two syntaxes, nom-loc and
// loc-nom. Which of the two English constructions fits is a judgement about
// the relationship, not something the grammar settles, so this names both.
// (Preston vol. 2 p.53, 2026-09-13.)
// `endsVerbalNounThenTerminator` — p.53's signal, the clause ending in a
// verbal noun plus a terminating particle rather than in a verb. The
// terminator is often FUSED to the noun (pa'o), which p.71's vocabulary calls
// out as ordinary: "syntactic particle fused to suffixless final syllable".
// A first version of this looked only for two separate tokens and never
// fired.
//
// `seventhCaseSubjectNoVerb` — p.64's signal, and the cleaner of the two: the
// topical-outline sentence has a subject in the SEVENTH case, a list, and an
// implied yod ending it. "A has [seven parts]: 1..7."
// `endsInVerbalNoun` — the clause's last element is nominalised (pa/ba, or a
// fused pa'o/ba'o), so it is a verbal NOUN and not the finite verb.
//
// This is the condition, and finding it corrected a wrong assumption of mine.
// I first gated this on "no verb attested", and it never fired on Preston's
// own examples — because spotVerb is CONFIDENT on both of them:
//
//     grub mtha'i rnam gzhag bshad pa'o   verb=bshad pa  confident=1
//     dang po la mtshan nyid dang dbye ba verb=dbye      confident=1
//
// The dictionary has tense forms for bshad and dbye, so spotVerb marks the
// nominalised form as the clause verb with confidence. Preston p.53 says of
// this very sentence that it "ends not with a verb, but with a verbal noun
// followed by a terminating syntactic particle". The engine is not merely
// silent about the implied verb — it confidently names a verbal noun as the
// finite one, which is worse.
//
// So the signal is the nominalisation itself, and the caution has to fire
// DESPITE spotVerb's confidence rather than because of its absence.
const char* impliedExistenceVerbLikely(bool endsInVerbalNoun,
                                       bool seventhCaseSubjectNoVerb);

// What the engine CANNOT recover, from the same page.
//
// Preston p.53 on implied agents, objects and subjects: they "share a common
// reliance on context" — when "assert" appears with no agent in a chapter on
// the Proponents of the Great Exposition, the agent is a Proponent of the
// Great Exposition, "because that is what the whole chapter is about".
//
// That inference lives in the surrounding discussion, not in the sentence.
// This reader analyses one clause at a time and has no access to the chapter,
// so it can report that an element is missing and must not pretend to supply
// it. Returns the standing caution; the caller decides where to show it.
const char* impliedElementNeedsContext();

}  // namespace allcore
