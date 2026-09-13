// cases_smoke — the eight cases of declension and transitivity.
// Preston, How to Read Classical Tibetan I, front matter + grammar review.
#include <cstdio>
#include <string>
#include <vector>

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
        // Preston p.218 lists EIGHT cases and the eighth is also unmarked.
        // The first version of this file knew seven and reported a bare noun
        // as the nominative full stop.
        CHECK(r.caveat[0] != '\0' && std::string(r.caveat).find("vocative") !=
                  std::string::npos,
              "an unmarked noun carries the caveat that the vocative is "
              "unmarked too, so it is not PROOF of the nominative");
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

    // Preston p.218 groups the eight classes in THREE, not two.
    CHECK(transitivityOf(VerbClass::AgentiveNom) == Transitivity::Transitive &&
              transitivityOf(VerbClass::AgentiveObj) == Transitivity::Transitive,
          "ag-nom and ag-obj are the two transitive classes");
    CHECK(transitivityOf(VerbClass::Linking) == Transitivity::Intransitive &&
              transitivityOf(VerbClass::Existence) == Transitivity::Intransitive &&
              transitivityOf(VerbClass::Motion) == Transitivity::Intransitive &&
              transitivityOf(VerbClass::Absence) == Transitivity::Intransitive,
          "the four nominative groups are the intransitive classes");
    // THE correction. My first version answered "intransitive" for both of
    // these, and Preston's p.219 entry for loc-nom says "class of TRANSITIVE
    // verbs whose subject is in the locative (7th) case". Neither bucket
    // fits: they have an object like a transitive verb and a subject like an
    // intransitive one. This fails if either is ever collapsed into a bucket
    // to make the enum tidier.
    CHECK(transitivityOf(VerbClass::Necessity) == Transitivity::Specialized &&
              transitivityOf(VerbClass::LocativeNom) == Transitivity::Specialized,
          "b/p-nom and loc-nom are SPECIALIZED \u2014 neither bucket, because "
          "they carry an object and still call their actor a subject");
    CHECK(specializedNote(VerbClass::LocativeNom)[0] != '\0' &&
              specializedNote(VerbClass::Linking)[0] == '\0',
          "and a specialized class says what it is, while the other six say "
          "nothing");
    // Preston's exclusive usage: agent for transitive, subject for intransitive
    CHECK(std::string(actorWord(VerbClass::AgentiveNom)) == "agent",
          "a transitive verb takes an AGENT, in Preston's exclusive sense");
    CHECK(std::string(actorWord(VerbClass::Linking)) == "subject",
          "an intransitive verb takes a SUBJECT, in Preston's exclusive sense");
    CHECK(std::string(actorWord(VerbClass::LocativeNom)) == "subject" &&
              std::string(actorWord(VerbClass::Necessity)) == "subject",
          "and so does a specialized verb, even though it has an object");
    CHECK(unstatedAgentCaution(VerbClass::LocativeNom, false)[0] == '\0',
          "a specialized verb never raises the unstated-AGENT caution \u2014 "
          "its actor is a subject, and its absence is a different question");

    // and the caution a machine most needs
    CHECK(unstatedAgentCaution(VerbClass::AgentiveNom, false)[0] != '\0',
          "a transitive verb with no third-case chunk raises the "
          "unstated-agent caution");
    CHECK(unstatedAgentCaution(VerbClass::AgentiveNom, true)[0] == '\0',
          "and does not raise it when the agent is right there");
    CHECK(unstatedAgentCaution(VerbClass::Linking, false)[0] == '\0',
          "an intransitive verb never raises it — it has no agent to miss");

    // ---- Preston's p.xv chart, inverted ----
    {
        using F = Function;
        auto has = [](const std::vector<F>& v, F f) {
            for (F x : v) if (x == f) return true;
            return false;
        };
        // The fifth case is the ONLY one in the chart that names one function.
        CHECK(functionsForCase(5).size() == 1 &&
                  functionsForCase(5)[0] == F::QualifierOfVerb,
              "the fifth case names exactly one function \u2014 qualifier \u2014 "
              "and is the only case in the chart that does");
        // The sixth fills no slot: it joins noun to noun.
        CHECK(functionsForCase(6).empty(),
              "the sixth case fills no syntactic slot of its own");
        // The first is four ways ambiguous before the verb is known.
        CHECK(functionsForCase(1).size() == 4,
              "a bare first-case noun could be four different things until "
              "the verb is known");
        // Preston's two exclusions, which is what the verb buys you.
        const auto t1 = functionsFor(1, Transitivity::Transitive);
        CHECK(!has(t1, F::SubjectOfIntransitive) &&
                  !has(t1, F::ComplementToSubject),
              "under a TRANSITIVE verb the first case cannot be a subject or "
              "a complement to one \u2014 a transitive clause has no subject");
        CHECK(has(t1, F::ObjectOfTransitive),
              "and can still be the object");
        const auto i1 = functionsFor(1, Transitivity::Intransitive);
        CHECK(!has(i1, F::AgentOfTransitive) &&
                  !has(i1, F::ObjectOfTransitive) &&
                  !has(i1, F::ComplementToObject),
              "under an INTRANSITIVE verb the first case cannot be an agent, "
              "an object, or a complement to an object");
        CHECK(has(i1, F::SubjectOfIntransitive) &&
                  has(i1, F::ComplementToSubject),
              "and can be the subject or its complement");
        // Knowing the verb genuinely narrows: two of four, not four of four.
        CHECK(t1.size() == 2 && i1.size() == 2,
              "knowing the verb's transitivity halves the first case's "
              "possibilities \u2014 the filter earns its place");
        // A specialized verb gets NEITHER exclusion, because it carries a
        // subject and an object at once. Collapsing it into either bucket
        // would silently delete real readings.
        CHECK(functionsFor(1, Transitivity::Specialized).size() == 4,
              "a SPECIALIZED verb narrows nothing \u2014 it has a subject and "
              "an object, so neither exclusion applies");
        // THE overreach gate. The second case is what fused r marks, and the
        // engine used to label it "la don, to/at/in" — the qualifier reading,
        // named as though it were the answer. Preston diagrams that same
        // fused particle as the COMPLEMENT in both of his worked examples.
        const auto second = functionsForCase(2);
        CHECK(second.size() == 4 && has(second, F::QualifierOfVerb) &&
                  has(second, F::ComplementToObject) &&
                  has(second, F::ComplementToSubject) &&
                  has(second, F::ObjectOfTransitive),
              "the second case admits the object, either complement, AND the "
              "qualifier \u2014 naming only the qualifier was the overreach "
              "this table exists to end");
        CHECK(std::string(functionName(F::ComplementToSubject)) ==
                  "complement to the subject",
              "every function can say its own name");
    }

    std::printf("cases_smoke: %s (%d failure(s))\n",
                failures ? "FAILURES" : "ALL PASS", failures);
    return failures ? 1 : 0;
}
