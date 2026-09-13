// table_conformance — the five hand-kept particle tables, made answerable to
// each other.
//
// allcore carries FIVE independent particle/case tables: kRoleMarkers and
// fusedRole in reader.cpp, kEndings and classifyParticle in particles.cpp,
// caseOf in cases.cpp, and caseLabel in wilsonparse.cpp. Every gate in this
// repository proves one of them consistent with ITSELF. Nothing has ever
// checked them against each other, and they already disagree — on fused r,
// which reader.cpp calls a la don ("to/at/in") and Preston diagrams as the
// second-case particle marking a COMPLEMENT, and on the gis-family, which
// cases.cpp calls unambiguously third case while verbclass.h records class
// IV.2 as "nominative-syntactic, GIS-FAMILY (stong)" — a qualifier use.
//
// A set of tables that agree with each other and answer to nothing can be
// perfectly self-consistent and wrong together. This battery drives the
// PUBLIC path — tokenize, chunk, read the marker the chunker assigned — and
// asks caseOf the same question, so it tests what the engine does rather than
// an internal exposed for the test's convenience.
#include <cstdio>
#include <string>
#include <vector>

#include "allcore/cases.h"
#include "allcore/lattice.h"
#include "allcore/reader.h"
#include "allcore/spine.h"
#include "allcore/verbclass.h"
#include "allcore/wilsonparse.h"

static int failures = 0;
#define CHECK(cond, msg)                                        \
    do {                                                        \
        if (cond) std::printf("  [PASS] %s\n", msg);            \
        else { std::printf("  [FAIL] %s\n", msg); ++failures; } \
    } while (0)

int main(int argc, char** argv) {
    if (argc < 2) {
        std::fprintf(stderr, "usage: table_conformance <spine.db>\n");
        return 2;
    }
    allcore::Spine spine(argv[1]);

    // Chunk a two-word phrase ending in the particle, and report the marker
    // the chunker actually assigned plus the role text it chose.
    struct Seen { std::string marker; std::string role; bool chunked = false; };
    auto chunkWith = [&](const std::string& acip) -> Seen {
        auto doc = allcore::buildOverlay(spine, acip);
        auto cls = allcore::splitClauses(doc.tokens, doc.barrier_after);
        cls = allcore::refineClauses(doc, cls);
        Seen out;
        if (cls.empty()) return out;
        auto chunks = allcore::chunkClause(doc, cls[0]);
        if (chunks.empty()) return out;
        out.marker = chunks[0].marker;
        out.role = chunks[0].role ? chunks[0].role : "";
        out.chunked = true;
        return out;
    };

    // The agreement the two tables must reach, stated once. `family` is the
    // phrase reader.cpp uses in its role text; `cases` is what cases.cpp must
    // answer for the same marker.
    struct Row {
        const char* acip;        // what to feed the tokenizer
        const char* marker;      // the wylie the chunker should assign
        const char* roleWord;    // a word that must appear in reader.cpp's role
        std::vector<int> cases;  // what caseOf must return
    };
    const std::vector<Row> kRows = {
        {"CHOS KYI",   "kyi",  "connector", {6}},
        {"CHOS GYI",   "gyi",  "connector", {6}},
        {"CHOS GI",    "gi",   "connector", {6}},
        {"CHOS KYIS",  "kyis", "agent",     {3}},
        {"CHOS GYIS",  "gyis", "agent",     {3}},
        {"CHOS GIS",   "gis",  "agent",     {3}},
        {"CHOS LA",    "la",   "la don",    {2, 4, 7}},
        {"CHOS DU",    "du",   "la don",    {2, 4, 7}},
        {"CHOS TU",    "tu",   "la don",    {2, 4, 7}},
        {"CHOS SU",    "su",   "la don",    {2, 4, 7}},
        {"CHOS RU",    "ru",   "la don",    {2, 4, 7}},
        {"CHOS NAS",   "nas",  "source",    {5}},
        {"CHOS LAS",   "las",  "source",    {5}},
    };

    for (const auto& r : kRows) {
        const Seen s = chunkWith(r.acip);
        const std::string what = std::string("conformance: ") + r.marker;
        if (!s.chunked) {
            CHECK(false, (what + " — the chunker produced no chunk at all").c_str());
            continue;
        }
        CHECK(s.marker == r.marker,
              (what + " — the chunker assigns the marker the table names").c_str());
        CHECK(s.role.find(r.roleWord) != std::string::npos,
              (what + " — reader.cpp's role text names its family").c_str());
        const auto cr = allcore::caseOf(r.marker);
        CHECK(cr.cases == r.cases,
              (what + " — cases.cpp answers the SAME case set reader.cpp's "
                      "family implies").c_str());
    }

    // The known disagreement, pinned so it cannot drift further without
    // saying so. reader.cpp's fusedRole calls fused r a la don "to/at/in",
    // which is the QUALIFIER reading; Preston diagrams the same particle as
    // the second case marking a complement. cases.cpp is the honest one — it
    // returns all three la don cases and refuses to choose.
    //
    // This does not assert the disagreement is fine. It asserts it is exactly
    // this and no wider, so the day someone narrows either side the gate says
    // the ground moved. (See docs/standards/PRESTON_SYNTAX.md.)
    {
        const auto r = allcore::caseOf("r");
        CHECK(r.cases == std::vector<int>({2, 4, 7}),
              "conformance: fused r is 2nd/4th/7th in cases.cpp, which does "
              "not narrow where the grammar cannot");
        const auto fns = allcore::functionsForCase(2);
        bool hasComplement = false, hasQualifier = false;
        for (auto f : fns) {
            if (f == allcore::Function::ComplementToObject ||
                f == allcore::Function::ComplementToSubject) hasComplement = true;
            if (f == allcore::Function::QualifierOfVerb) hasQualifier = true;
        }
        CHECK(hasComplement && hasQualifier,
              "conformance: and the second case admits BOTH the complement "
              "reading Preston diagrams and the qualifier reading reader.cpp "
              "names — neither table may claim the other is wrong");
    }

    // The gis-family. cases.cpp calls it unambiguously third case; verbclass.h
    // records class IV.2 as nominative-syntactic, GIS-FAMILY, where the same
    // particle marks a QUALIFIER of an absence verb. Both cannot be the whole
    // truth. Pinned as a known open question rather than silently resolved.
    {
        const auto gis = allcore::caseOf("gis");
        CHECK(gis.cases == std::vector<int>({3}),
              "conformance: cases.cpp still answers third case for the "
              "gis-family");
        const auto* absence = allcore::classifyVerb("stong");
        CHECK(absence != nullptr,
              "conformance: the Wilson tables still know stong, the absence "
              "verb whose qualifier the gis-family marks");
        // functionsFor(3) must therefore keep the qualifier reading open. If
        // anyone ever makes the third case proof of agency, this fails.
        const auto fns = allcore::functionsForCase(3);
        bool hasQualifier = false, hasAgent = false;
        for (auto f : fns) {
            if (f == allcore::Function::QualifierOfVerb) hasQualifier = true;
            if (f == allcore::Function::AgentOfTransitive) hasAgent = true;
        }
        CHECK(hasAgent && hasQualifier,
              "conformance: the third case is never proof of agency — it "
              "keeps the qualifier reading, which is how the gis-family marks "
              "an absence verb's qualifier");
    }

    // ---- narrowLaDon must agree with wilsonParse ----------------------
    //
    // cases.cpp's narrowLaDon resolves a la don particle from the verb class.
    // wilsonParse has been doing the same thing since long before cases.cpp
    // existed, through caseLabel, and its answers are already gated in
    // reader_smoke. That makes narrowLaDon a SIXTH table of exactly the kind
    // this battery exists to police, and it was written knowing that.
    //
    // So it is held to the older one. wilsonParse is public and already
    // resolves; nothing new is exposed to make this checkable. If the two ever
    // part company, the Walkthrough and the Wilson Parse pane will say
    // different things about the same word in front of the same reader, which
    // is the defect this was written to end rather than to cause.
    {
        struct Case {
            const char* acip;      // passage
            const char* tok;       // the la don token in it
            const char* verb;      // its verb, for classifyVerb
            const char* expectIn;  // what wilsonParse's category must contain
            int narrowTo;          // what narrowLaDon must answer
        };
        const std::vector<Case> kCases = {
            {"RGYA GAR DU 'GRO", "DU", "'gro", "2nd", 2},
            {"BDAG LA NOR YOD",  "LA", "yod",  "7th", 7},
        };
        for (const auto& c : kCases) {
            auto doc = allcore::buildOverlay(spine, c.acip);
            auto cls = allcore::splitClauses(doc.tokens, doc.barrier_after);
            cls = allcore::refineClauses(doc, cls);
            if (cls.empty()) {
                CHECK(false, "conformance: la don passage produced no clause");
                continue;
            }
            auto parsed = allcore::wilsonParse(spine, doc, cls);
            bool wilsonSaid = false;
            if (!parsed.empty())
                for (const auto& u : parsed[0].units)
                    if (u.text == c.tok &&
                        u.category.find(c.expectIn) != std::string::npos)
                        wilsonSaid = true;
            const std::string what =
                std::string("conformance: ") + c.tok + " under " + c.verb;
            CHECK(wilsonSaid,
                  (what + " — wilsonParse still resolves it to the " +
                   c.expectIn).c_str());
            std::string low;
            for (const char* q = c.tok; *q; ++q)
                low += (*q >= 'A' && *q <= 'Z') ? (char)(*q - 'A' + 'a') : *q;
            const auto* vc = allcore::classifyVerb(c.verb);
            CHECK(vc != nullptr,
                  (what + " — the Wilson tables still know the verb").c_str());
            const auto n = allcore::narrowLaDon(low, vc);
            CHECK(n.narrowed() && n.cases[0] == c.narrowTo,
                  (what + " — and cases.cpp narrows it the SAME way, so the "
                          "two panes cannot contradict each other").c_str());
            CHECK(n.because[0] != '\0',
                  (what + " — saying what about the verb decided it").c_str());
        }
        // With no verb, nothing narrows. The particle still cannot choose.
        const auto open = allcore::narrowLaDon("la", nullptr);
        CHECK(open.cases.size() == 3 && !open.narrowed() &&
                  open.because[0] == '\0',
              "conformance: with no verb identified the la don stays 2nd/4th/"
              "7th and claims no reason");
    }

    std::printf("table_conformance: %s (%d failure(s))\n",
                failures ? "FAILURES" : "ALL PASS", failures);
    return failures ? 1 : 0;
}
