#include "allcore/cases.h"

namespace allcore {

CaseReading caseOf(const std::string& marker) {
    CaseReading r;
    if (marker.empty()) {
        // Unmarked IS the first case. Preston is explicit that the object of a
        // transitive verb "is in the first case, thus has no case particle
        // marking it" — the absence is the marking, and a reader who is told
        // only "no particle" has been told the shape of the fact and not the
        // fact.
        r.cases = {1};
        r.family = "ming tsam";
        r.gloss = "first case — unmarked. Not a missing particle: the first "
                  "case IS the bare noun";
        // Preston p.218: nineteen case particles across EIGHT cases, and the
        // eighth — the vocative — also takes no particle. So a bare noun is
        // the first case in almost every instance and is not PROOF of it. The
        // caveat is carried rather than folded into `cases`, because putting
        // {1,8} on every unmarked chunk would mark the whole page ambiguous
        // to record something rare, and drown the la don ambiguity that
        // actually bites.
        r.caveat = "the eighth case (vocative) is also unmarked, so a bare "
                   "noun is not by itself proof of the nominative";
        return r;
    }
    // agentive / instrumental — unambiguous
    if (marker == "kyis" || marker == "gyis" || marker == "gis" ||
        marker == "yis" || marker == "s") {
        r.cases = {3};
        r.family = "byed sgra";
        r.gloss = "third case — marks the AGENT of a transitive verb, or the "
                  "instrument (\"by\", \"with\")";
        return r;
    }
    // genitive — unambiguous
    if (marker == "kyi" || marker == "gyi" || marker == "gi" ||
        marker == "yi" || marker == "'i") {
        r.cases = {6};
        r.family = "'brel sgra";
        r.gloss = "sixth case — connects this noun to the NEXT one (\"the Y "
                  "of X\"), so it reads rightwards";
        return r;
    }
    // elative — unambiguous
    if (marker == "nas" || marker == "las") {
        r.cases = {5};
        r.family = "'byung khungs";
        r.gloss = "fifth case — source or comparison (\"from\", \"than\")";
        return r;
    }
    // la don — 2nd, 4th or 7th, and the particle cannot tell you which
    if (marker == "la" || marker == "na" || marker == "ru" || marker == "su" ||
        marker == "tu" || marker == "du" || marker == "r") {
        r.cases = {2, 4, 7};
        r.family = "la don";
        r.gloss = "";   // deliberately empty — see below
        return r;
    }
    return r;   // not a case particle
}

Transitivity transitivityOf(VerbClass c) {
    switch (c) {
        // ag-nom, ag-obj — agent in the 3rd, object in the 1st or 2nd
        case VerbClass::AgentiveNom:
        case VerbClass::AgentiveObj:
            return Transitivity::Transitive;
        // b/p-nom (subject in the 4th) and loc-nom (subject in the 7th), each
        // with an object in the 1st. Preston p.218 sets these apart from both
        // buckets and p.219 calls loc-nom transitive while still giving it a
        // subject. Neither bucket is honest for them.
        case VerbClass::Necessity:
        case VerbClass::LocativeNom:
            return Transitivity::Specialized;
        default:
            return Transitivity::Intransitive;
    }
}

bool isTransitive(VerbClass c) {
    return transitivityOf(c) == Transitivity::Transitive;
}

const char* actorWord(VerbClass c) {
    // "agent" belongs to the two transitive classes alone. The specialized
    // pair takes a SUBJECT despite having an object — which is exactly why
    // they are a third group.
    return transitivityOf(c) == Transitivity::Transitive ? "agent" : "subject";
}

const char* specializedNote(VerbClass c) {
    switch (c) {
        case VerbClass::Necessity:
            return "a SPECIALIZED class (b/p-nom): its subject sits in the "
                   "fourth case and its object in the first, so it has an "
                   "object like a transitive verb and an actor named like an "
                   "intransitive one";
        case VerbClass::LocativeNom:
            return "a SPECIALIZED class (loc-nom): its subject sits in the "
                   "seventh case and its object in the first. Preston calls "
                   "it transitive and still calls its actor a subject";
        default:
            return "";
    }
}

const char* unstatedAgentCaution(VerbClass c, bool sawThirdCase) {
    // Only the two genuinely transitive classes take a third-case agent, so
    // only they can be missing one. A specialized verb's actor is a subject
    // in the 4th or 7th and its absence is a different question.
    if (!isTransitive(c) || sawThirdCase) return "";
    return "the verb is transitive but no third-case chunk appears, so the "
           "agent is UNSTATED. Tibetan leaves agents out far more often than "
           "English does — this is not evidence that the clause is "
           "intransitive, and the tool will not treat it as such";
}

}  // namespace allcore
