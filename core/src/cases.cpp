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

bool isTransitive(VerbClass c) {
    // Wilson's own names carry it: the agentive- classes take an agent, and
    // everything else is nominative-, purposive- or locative-nominative.
    return c == VerbClass::AgentiveNom || c == VerbClass::AgentiveObj;
}

const char* actorWord(VerbClass c) {
    return isTransitive(c) ? "agent" : "subject";
}

const char* unstatedAgentCaution(VerbClass c, bool sawThirdCase) {
    if (!isTransitive(c) || sawThirdCase) return "";
    return "the verb is transitive but no third-case chunk appears, so the "
           "agent is UNSTATED. Tibetan leaves agents out far more often than "
           "English does — this is not evidence that the clause is "
           "intransitive, and the tool will not treat it as such";
}

}  // namespace allcore
