#include "allcore/verbclass.h"

#include <vector>

namespace allcore {

namespace {

// Example inventories from Wilson's Appendix Four, extended with the member
// lists taught in chapters 17 and 19 (attitude-verb table p. 514, cognitive
// agentive verbs p. 513, agentive-nominative examples pp. 413–419); one row
// per verb.
const VerbClassInfo kVerbs[] = {
    // I linking
    {"yin", VerbClass::Linking, "Class I linking (nominative-nominative)",
     "subject and complement are both unmarked: \"A is B\""},
    {"lags", VerbClass::Linking, "Class I linking (nominative-nominative)",
     "subject and complement are both unmarked: \"A is B\" (honorific)"},
    {"min", VerbClass::Linking, "Class I linking (nominative-nominative)",
     "subject and complement are both unmarked: \"A is not B\""},
    // II.1 existence
    {"yod", VerbClass::Existence, "Class II verb of existence",
     "unmarked subject exists; a la-don chunk is the place — and with a "
     "la-marked possessor the verb reads \"has\" (Class VIII use)"},
    {"med", VerbClass::Existence, "Class II verb of existence",
     "unmarked subject does not exist; la-don chunk = place / possessor "
     "(\"x has no …\")"},
    {"mchis", VerbClass::Existence, "Class II verb of existence",
     "unmarked subject exists (humilific); la-don chunk = place"},
    {"gda'", VerbClass::Existence, "Class II verb of existence",
     "unmarked subject exists; la-don chunk = place"},
    {"mnga'", VerbClass::Existence, "Class II verb of existence",
     "unmarked subject exists (honorific); la-marked possessor = \"has\""},
    // II.2 living
    {"gnas", VerbClass::Living, "Class II verb of living",
     "unmarked subject abides; la-don chunk = the place or time"},
    {"bzhugs", VerbClass::Living, "Class II verb of living",
     "unmarked subject dwells (honorific); la-don chunk = the place"},
    {"sdod", VerbClass::Living, "Class II verb of living",
     "unmarked subject stays; la-don chunk = the place"},
    // II.3 dependence
    {"rten", VerbClass::Dependence, "Class II verb of dependence",
     "unmarked subject depends; the la-don chunk = what it depends ON"},
    {"rag las", VerbClass::Dependence, "Class II verb of dependence",
     "unmarked subject depends; the la-don chunk = what it depends ON"},
    {"ltos", VerbClass::Dependence, "Class II verb of dependence",
     "unmarked subject depends; the la-don chunk = what it depends ON"},
    // II.4 attitude (referential locative: the la-don chunk = "about/toward")
    {"chags", VerbClass::Attitude, "Class II attitude verb",
     "unmarked subject feels it; the la-don chunk = toward/about what"},
    {"'jigs", VerbClass::Attitude, "Class II attitude verb",
     "unmarked subject fears; the la-don chunk = of what"},
    {"dad", VerbClass::Attitude, "Class II attitude verb",
     "unmarked subject has faith; the la-don chunk = in what"},
    {"mos", VerbClass::Attitude, "Class II attitude verb",
     "unmarked subject admires; the la-don chunk = what"},
    {"zhen", VerbClass::Attitude, "Class II attitude verb",
     "unmarked subject clings; the la-don chunk = to what"},
    {"gus", VerbClass::Attitude, "Class II attitude verb",
     "unmarked subject trusts/respects; the la-don chunk = whom"},
    {"khro", VerbClass::Attitude, "Class II attitude verb",
     "unmarked subject is angry; the la-don chunk = at whom"},
    {"'khrul", VerbClass::Attitude, "Class II attitude verb",
     "unmarked subject is mistaken; the la-don chunk = about what"},
    {"sdang", VerbClass::Attitude, "Class II attitude verb",
     "unmarked subject feels aversion; the la-don chunk = toward what"},
    {"rmongs", VerbClass::Attitude, "Class II attitude verb",
     "unmarked subject is confused; the la-don chunk = about what"},
    {"byams", VerbClass::Attitude, "Class II attitude verb",
     "unmarked subject loves; the la-don chunk = whom"},
    {"mkhas", VerbClass::Attitude, "Class II attitude verb",
     "unmarked subject is skilled; the la-don chunk = in what"},
    {"skyo", VerbClass::Attitude, "Class II attitude verb",
     "unmarked subject is weary; the la-don chunk = of what"},
    {"bzod", VerbClass::Attitude, "Class II attitude verb",
     "unmarked subject is patient; the la-don chunk = about what"},
    // III.1 motion
    {"'gro", VerbClass::Motion, "Class III verb of motion",
     "unmarked subject moves; the la-don/objective chunk = the DESTINATION"},
    {"'ong", VerbClass::Motion, "Class III verb of motion",
     "unmarked subject comes; the la-don/objective chunk = the destination"},
    {"byon", VerbClass::Motion, "Class III verb of motion",
     "unmarked subject comes/goes (honorific); la-don chunk = destination"},
    // III.2 nominative action
    {"skye", VerbClass::NomAction, "Class III nominative action verb",
     "unmarked subject acts without a separate object; la-don chunk = where"},
    {"'khor", VerbClass::NomAction, "Class III nominative action verb",
     "unmarked subject turns/circles; la-don chunk = where"},
    {"goms", VerbClass::NomAction, "Class III nominative action verb",
     "unmarked subject habituates; la-don chunk = to what"},
    {"snang", VerbClass::NomAction, "Class III nominative action verb",
     "unmarked subject appears; la-don chunk = to whom/where"},
    {"thim", VerbClass::NomAction, "Class III nominative action verb",
     "unmarked subject dissolves; la-don chunk = into what"},
    // III.3 rhetorical
    {"rung", VerbClass::Rhetorical, "Class III rhetorical verb",
     "unmarked subject (often a clause) is admissible/suitable"},
    {"rigs", VerbClass::Rhetorical, "Class III rhetorical verb",
     "unmarked subject (often a clause) is correct/reasonable"},
    {"'thad", VerbClass::Rhetorical, "Class III rhetorical verb",
     "unmarked subject (often a clause) is correct/tenable"},
    {"thal", VerbClass::Rhetorical, "Class III rhetorical verb",
     "debate consequence: \"it [absurdly] follows that …\""},
    // IV.1 separative
    {"grol", VerbClass::Separative, "Class IV separative verb",
     "the las/nas chunk = what the subject is freed FROM"},
    {"thar", VerbClass::Separative, "Class IV separative verb",
     "the las/nas chunk = what the subject is liberated FROM"},
    {"'da'", VerbClass::Separative, "Class IV separative verb",
     "the las/nas chunk = what the subject passes BEYOND"},
    {"ldog", VerbClass::Separative, "Class IV separative verb",
     "the las/nas chunk = what the subject turns back FROM"},
    // IV.2 absence — the gis-family chunk is NOT an agent here
    {"stong", VerbClass::Absence, "Class IV verb of absence",
     "the gis-family chunk = what the subject is empty OF (not an agent!)"},
    {"dben", VerbClass::Absence, "Class IV verb of absence",
     "the gis-family chunk = what the subject is isolated FROM (not an agent)"},
    {"dbul", VerbClass::Absence, "Class IV verb of absence",
     "the gis-family chunk = what the subject is poor IN (not an agent)"},
    {"phongs", VerbClass::Absence, "Class IV verb of absence",
     "the gis-family chunk = what the subject is destitute OF (not an agent)"},
    // IV.3 conjunctive
    {"'brel", VerbClass::Conjunctive, "Class IV conjunctive verb",
     "the dang chunk completes the verb: related/connected WITH x"},
    {"phrad", VerbClass::Conjunctive, "Class IV conjunctive verb",
     "the dang chunk completes the verb: meets WITH x"},
    {"bcas", VerbClass::Conjunctive, "Class IV conjunctive verb",
     "the dang chunk completes the verb: together WITH x"},
    {"mthun", VerbClass::Conjunctive, "Class IV conjunctive verb",
     "the dang chunk completes the verb: accords WITH x"},
    // IV.4 disjunctive
    {"bral", VerbClass::Disjunctive, "Class IV disjunctive verb",
     "the dang chunk completes the verb: free OF x / separated from x"},
    {"'gal", VerbClass::Disjunctive, "Class IV disjunctive verb",
     "the dang chunk completes the verb: contradicts x"},
    // V agentive-nominative
    {"bstan", VerbClass::AgentiveNom, "Class V agentive-nominative verb",
     "look for the agent in a gis-family chunk; the unmarked chunk = object "
     "(irregular nominative subjects do occur)"},
    {"byed", VerbClass::AgentiveNom, "Class V agentive-nominative verb",
     "look for the agent in a gis-family chunk; the unmarked chunk = object "
     "(as \"is called/refers to\" it shifts to Class VIII)"},
    {"'dzin", VerbClass::AgentiveNom, "Class V agentive-nominative verb",
     "look for the agent in a gis-family chunk; the unmarked chunk = object"},
    {"'dod", VerbClass::AgentiveNom, "Class V agentive-nominative verb",
     "asserts/wants: agent in a gis-family chunk, but nominative subjects "
     "are common with this verb"},
    {"zer", VerbClass::AgentiveNom, "Class V agentive-nominative verb",
     "says: quotative (zhes/ces) phrase = what is said; as \"is called\" it "
     "shifts to Class VIII"},
    {"gsung", VerbClass::AgentiveNom, "Class V agentive-nominative verb",
     "says (honorific); as \"is called / spoke of\" it shifts to Class VIII"},
    {"smra", VerbClass::AgentiveNom, "Class V agentive-nominative verb",
     "speaks: agent in a gis-family chunk"},
    // cognitive processes are AGENTIVE verbs (Wilson ch. 19 note), not
    // attitude verbs
    {"shes", VerbClass::AgentiveNom, "Class V agentive-nominative verb",
     "knows: agent in a gis-family chunk; unmarked chunk = what is known"},
    {"rtogs", VerbClass::AgentiveNom, "Class V agentive-nominative verb",
     "realizes: agent in a gis-family chunk; unmarked chunk = what is realized"},
    {"dmigs", VerbClass::AgentiveNom, "Class V agentive-nominative verb",
     "observes / takes as object: agent in a gis-family chunk"},
    {"mthong", VerbClass::AgentiveNom, "Class V agentive-nominative verb",
     "sees: agent in a gis-family chunk; unmarked chunk = what is seen"},
    {"'jal", VerbClass::AgentiveNom, "Class V agentive-nominative verb",
     "measures/assesses: agent in a gis-family chunk"},
    {"sgrub", VerbClass::AgentiveNom, "Class V agentive-nominative verb",
     "accomplishes/proves: agent in a gis-family chunk"},
    {"mdzad", VerbClass::AgentiveNom, "Class V agentive-nominative verb",
     "does/performs (honorific): agent in a gis-family chunk"},
    {"len", VerbClass::AgentiveNom, "Class V agentive-nominative verb",
     "takes/holds (khas len asserts, nyams su len practices): agent in a "
     "gis-family chunk"},
    {"'chad", VerbClass::AgentiveNom, "Class V agentive-nominative verb",
     "explains; as \"is called\" it shifts to Class VIII"},
    // VI agentive-objective
    {"lta", VerbClass::AgentiveObj, "Class VI agentive-objective verb",
     "agent in a gis-family chunk; the la-don chunk = what is looked AT"},
    {"khyab", VerbClass::AgentiveObj, "Class VI agentive-objective verb",
     "the la-don chunk = what is pervaded"},
    {"phan", VerbClass::AgentiveObj, "Class VI agentive-objective verb",
     "the la-don chunk = who is helped (fourth-case direct object)"},
    // VII necessity
    {"dgos", VerbClass::Necessity, "Class VII verb of necessity",
     "the la-marked chunk is the English SUBJECT: \"x needs …\"; the "
     "unmarked chunk = what is needed"},
};

}  // namespace

const VerbClassInfo* classifyVerb(const std::string& wylie) {
    for (const auto& v : kVerbs)
        if (wylie == v.wylie) return &v;
    return nullptr;
}

const VerbClassInfo* classifyVerbWithTenses(const std::string& wylie,
                                            const std::string& tenses) {
    if (const auto* v = classifyVerb(wylie)) return v;
    // tenses fields look like "blta lta bltas ltos" or "bya/ byed/ byas/ byos"
    std::string cur;
    std::vector<std::string> forms;
    for (char c : tenses + " ") {
        if (c == ' ' || c == '/' || c == '\t') {
            if (!cur.empty()) forms.push_back(cur);
            cur.clear();
        } else {
            cur += c;
        }
    }
    // the row is found when the SPOTTED verb's tense list contains a table
    // base form (bltas's row lists lta), or vice versa
    for (const auto& f : forms)
        if (const auto* v = classifyVerb(f)) return v;
    return nullptr;
}

}  // namespace allcore
