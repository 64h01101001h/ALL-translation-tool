// allcore/verbclass.h — Wilson's eight-class verb system as rule tables
// (framework: "Translating Buddhism from Tibetan", Appendix Four — rules and
// example inventories transcribed as data; no copyrighted prose).
//
// Three categories by the case of the SUBJECT:
//   One   nominative subject  — Classes I–IV
//   Two   agentive subject    — Classes V–VI
//   Three la-group subject    — Classes VII–VIII
// Classes within a category differ by how the object/qualifier is marked.
// Knowing the class corrects the reading: with a verb of absence, a
// gis-family chunk is NOT an agent ("empty OF x"); with possession/necessity,
// the la-marked chunk is the English subject ("x HAS / NEEDS").
#pragma once

#include <string>

namespace allcore {

enum class VerbClass {
    Linking,          // I    nominative-nominative (yin)
    Existence,        // II.1 nominative-locative (yod)
    Living,           // II.2 nominative-locative (gnas)
    Dependence,       // II.3 nominative-locative (rten)
    Attitude,         // II.4 nominative-locative, referential (chags)
    Motion,           // III.1 nominative-objective ('gro)
    NomAction,        // III.2 nominative-objective (snang)
    Rhetorical,       // III.3 nominative-objective (rung)
    Separative,       // IV.1 nominative-syntactic, las/nas (grol)
    Absence,          // IV.2 nominative-syntactic, gis-family (stong)
    Conjunctive,      // IV.3 nominative-syntactic, dang ('brel)
    Disjunctive,      // IV.4 nominative-syntactic, dang (bral)
    AgentiveNom,      // V    agentive-nominative (bstan)
    AgentiveObj,      // VI   agentive-objective (lta)
    Necessity,        // VII  purposive-nominative (dgos)
    LocativeNom,      // VIII locative-nominative (yod as "have"; zer "is called")
};

struct VerbClassInfo {
    const char* wylie;     // base form in the table
    VerbClass cls;
    const char* label;     // short class label for display
    const char* expects;   // what the clause's chunks mean under this verb
};

// Look up a verb's class by its wylie base form; nullptr when the verb is not
// in the tables (the trainer then says so — never guesses a class).
const VerbClassInfo* classifyVerb(const std::string& wylie);

// As above, but also tries each whitespace/slash-separated tense form from the
// dictionary's tenses field (so bltas resolves through lta's tense row).
const VerbClassInfo* classifyVerbWithTenses(const std::string& wylie,
                                            const std::string& tenses);

}  // namespace allcore
