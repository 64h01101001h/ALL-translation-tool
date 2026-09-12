// cases_smoke — the eight cases of declension and transitivity.
// Preston, How to Read Classical Tibetan I, front matter + grammar review.
#include <cstdio>
#include "allcore/cases.h"

static int failures = 0;
#define CHECK(cond, msg)                                        \
    do {                                                        \
        if (cond) std::printf("  [PASS] %s\n", msg);            \
        else { std::printf("  [FAIL] %s\n", msg); ++failures; } \
    } while (0)

int main() {
    using namespace allcore;
    // unmarked is the FIRST case, not an absence
    {
        auto r = caseOf("");
        CHECK(r.known() && r.cases.size() == 1 && r.cases[0] == 1,
              "an unmarked noun is the first case, not a missing particle");
        CHECK(!r.ambiguous(), "the first case is not ambiguous");
    }
    // the unambiguous families
    for (const char* p : {"kyis", "gyis", "gis", "yis", "s"}) {
        auto r = caseOf(p);
        CHECK(r.cases.size() == 1 && r.cases[0] == 3,
              "the byed sgra family is the third case, unambiguously");
    }
    for (const char* p : {"kyi", "gyi", "gi", "yi", "'i"}) {
        auto r = caseOf(p);
        CHECK(r.cases.size() == 1 && r.cases[0] == 6,
              "the 'brel sgra family is the sixth case, unambiguously");
    }
    for (const char* p : {"nas", "las"}) {
        auto r = caseOf(p);
        CHECK(r.cases.size() == 1 && r.cases[0] == 5,
              "the 'byung khungs family is the fifth case, unambiguously");
    }
    // THE gate: the la-family must never resolve to one case. Nothing in the
    // particle distinguishes 2nd from 4th from 7th — the relationship does —
    // so a table that picked one would be guessing (rule 3), and this fails
    // if one ever does.
    for (const char* p : {"la", "na", "ru", "su", "tu", "du", "r"}) {
        auto r = caseOf(p);
        CHECK(r.cases.size() == 3 && r.cases[0] == 2 && r.cases[1] == 4 &&
                  r.cases[2] == 7 && r.ambiguous(),
              "a la don particle reports 2nd/4th/7th and refuses to choose");
        CHECK(r.gloss[0] == '\0',
              "and carries no single gloss, because it has no single meaning");
    }
    CHECK(!caseOf("bstan").known(), "a verb is not a case particle");
    CHECK(!caseOf("dang").known(), "dang is not one of the seven cases");

    // transitivity read off Wilson's own class names
    CHECK(isTransitive(VerbClass::AgentiveNom) &&
              isTransitive(VerbClass::AgentiveObj),
          "the agentive classes are transitive");
    CHECK(!isTransitive(VerbClass::Linking) &&
              !isTransitive(VerbClass::Existence) &&
              !isTransitive(VerbClass::Motion) &&
              !isTransitive(VerbClass::Absence) &&
              !isTransitive(VerbClass::Necessity) &&
              !isTransitive(VerbClass::LocativeNom),
          "the nominative, purposive and locative classes are intransitive");
    // Preston's exclusive usage: agent for transitive, subject for intransitive
    CHECK(std::string(actorWord(VerbClass::AgentiveNom)) == "agent",
          "a transitive verb takes an AGENT, in Preston's exclusive sense");
    CHECK(std::string(actorWord(VerbClass::Linking)) == "subject",
          "an intransitive verb takes a SUBJECT, in Preston's exclusive sense");

    // and the caution a machine most needs
    CHECK(unstatedAgentCaution(VerbClass::AgentiveNom, false)[0] != '\0',
          "a transitive verb with no third-case chunk raises the "
          "unstated-agent caution");
    CHECK(unstatedAgentCaution(VerbClass::AgentiveNom, true)[0] == '\0',
          "and does not raise it when the agent is right there");
    CHECK(unstatedAgentCaution(VerbClass::Linking, false)[0] == '\0',
          "an intransitive verb never raises it — it has no agent to miss");

    std::printf("cases_smoke: %s (%d failure(s))\n",
                failures ? "FAILURES" : "ALL PASS", failures);
    return failures ? 1 : 0;
}
