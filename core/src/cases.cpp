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

const char* functionName(Function f) {
    switch (f) {
        case Function::AgentOfTransitive:     return "agent of the transitive verb";
        case Function::ObjectOfTransitive:    return "object of the transitive verb";
        case Function::ComplementToObject:    return "complement to the object";
        case Function::SubjectOfIntransitive: return "subject of the intransitive verb";
        case Function::ComplementToSubject:   return "complement to the subject";
        case Function::QualifierOfVerb:       return "qualifier of the verb";
        case Function::Vocative:              return "vocative — being addressed";
    }
    return "";
}

std::vector<Function> functionsForCase(int n) {
    switch (n) {
        case 1:   // nominative
            return {Function::ObjectOfTransitive,
                    Function::SubjectOfIntransitive,
                    Function::ComplementToObject,
                    Function::ComplementToSubject};
        case 2:   // objective
            return {Function::ObjectOfTransitive,
                    Function::ComplementToObject,
                    Function::ComplementToSubject,
                    Function::QualifierOfVerb};
        case 3:   // agentive
            return {Function::AgentOfTransitive, Function::QualifierOfVerb};
        case 4:   // beneficial / purposive
            return {Function::SubjectOfIntransitive, Function::QualifierOfVerb};
        case 5:   // originative — the only case in the chart that names ONE
            return {Function::QualifierOfVerb};
        case 6:   // connective: joins noun to noun, and fills no slot itself
            return {};
        case 7:   // locative
            return {Function::SubjectOfIntransitive, Function::QualifierOfVerb};
        case 8:   // vocative
            return {Function::Vocative};
        default:
            return {};
    }
}

std::vector<Function> functionsFor(int n, Transitivity t) {
    std::vector<Function> out;
    for (Function f : functionsForCase(n)) {
        if (t == Transitivity::Transitive &&
            (f == Function::SubjectOfIntransitive ||
             f == Function::ComplementToSubject))
            continue;   // a transitive clause has no subject
        if (t == Transitivity::Intransitive &&
            (f == Function::AgentOfTransitive ||
             f == Function::ObjectOfTransitive ||
             f == Function::ComplementToObject))
            continue;   // an intransitive clause has no agent and no object
        // Specialized: neither exclusion. They carry a subject AND an object.
        out.push_back(f);
    }
    return out;
}

bool syntacticParticleMarksQualifier(const std::string& marker, VerbClass c) {
    switch (c) {
        case VerbClass::Conjunctive:
        case VerbClass::Disjunctive:
            // verbclass.cpp: "the dang chunk completes the verb".
            return marker == "dang";
        case VerbClass::Separative:
            // Reachable as the 5th case too; named here so the s.p. reading
            // is not lost if the case route ever changes.
            return marker == "las" || marker == "nas";
        case VerbClass::Absence:
            // The gis-family here is NOT an agent — verbclass.cpp says so in
            // as many words — it is what the subject is empty of.
            return marker == "gis" || marker == "kyis" || marker == "gyis" ||
                   marker == "yis" || marker == "s";
        default:
            return false;
    }
}

LaDonNarrowing narrowLaDon(const std::string& marker, const VerbClassInfo* cls) {
    LaDonNarrowing out;
    const auto cr = caseOf(marker);
    out.cases = cr.cases;
    // Only the la don family is ever narrowed this way.
    if (!(cr.cases.size() == 3 && cr.cases[0] == 2)) return out;
    if (!cls) {
        out.because = "";
        return out;
    }
    switch (cls->cls) {
        case VerbClass::Motion:
        case VerbClass::NomAction:
            out.cases = {2};
            out.because = "the verb is one of motion or nominative action, so "
                          "this is the destination or the place of activity";
            break;
        case VerbClass::Necessity:
            out.cases = {4};
            out.because = "the verb is one of necessity, so this is the one "
                          "who needs";
            break;
        case VerbClass::Existence:
        case VerbClass::Living:
            out.cases = {7};
            out.because = "the verb is one of existence or living, so this is "
                          "the place or the possessor";
            break;
        case VerbClass::Dependence:
        case VerbClass::Attitude:
            out.cases = {7};
            out.because = "the verb is one of dependence or attitude, so this "
                          "is what it is about or toward";
            break;
        default:
            break;   // the verb's class says nothing; all three stand
    }
    return out;
}

const char* attributiveSyntaxOpen(const VerbClassInfo* cls, bool sawThirdCase,
                                  bool sawLaGroup) {
    if (!cls || !isTransitive(cls->cls)) return "";
    // An agent in the third case settles it: this is the ordinary agentive
    // reading and the attributive one is not in play.
    if (sawThirdCase) return "";
    // The attributive reading needs somewhere for the seventh case to sit.
    if (!sawLaGroup) return "";
    return "this verb is normally transitive, but no agent appears and a "
           "la don chunk is present \u2014 so ATTRIBUTIVE SYNTAX is open: the "
           "verb would be read nom-loc, its actor a SUBJECT rather than an "
           "agent, and the la don chunk a seventh case naming what is being "
           "called or referred to. Nothing here can tell the two readings "
           "apart; both are live";
}

}  // namespace allcore
