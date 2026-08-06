// reader_smoke — clause segmentation, phrase chunks, verb spotting, reading
// order, + the new particle families.
#include <cstdio>
#include <string>
#include <vector>

#include "allcore/lattice.h"
#include "allcore/particles.h"
#include "allcore/reader.h"
#include "allcore/spine.h"
#include "allcore/wilsonparse.h"

static int failures = 0;
#define CHECK(cond, msg)                                        \
    do {                                                        \
        if (cond) std::printf("  [PASS] %s\n", msg);            \
        else { std::printf("  [FAIL] %s\n", msg); ++failures; } \
    } while (0)

static std::vector<allcore::Clause> split(const std::string& acip) {
    std::vector<std::string> toks;
    std::vector<bool> barriers;
    allcore::tokenizeDocument(acip, toks, barriers);
    return allcore::splitClauses(toks, barriers);
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::fprintf(stderr, "usage: reader_smoke <spine.db>\n");
        return 2;
    }
    // ---- new particle families ----
    using A = allcore::Agreement;
    auto agree = [](const char* p, const char* t) {
        return allcore::checkAgreement(p, t).verdict;
    };
    CHECK(allcore::classifyParticle("ZHING") != nullptr &&
              allcore::classifyParticle("ZHES") != nullptr,
          "classifyParticle knows the coordinating and quotative families");
    CHECK(agree("BYED", "CING") == A::Agrees, "agreement: BYED CING (da->cing)");
    CHECK(agree("'DZIN", "ZHING") == A::Agrees,
          "agreement: 'DZIN ZHING (na->zhing)");
    CHECK(agree("LAS", "SHING") == A::Agrees, "agreement: LAS SHING (sa->shing)");
    CHECK(agree("SPONG", "CING") == A::Disagrees,
          "agreement: SPONG CING flagged (nga wants zhing)");
    CHECK(agree("BYA", "ZHES") == A::Agrees, "agreement: BYA ZHES (open->zhes)");
    CHECK(agree("THOB", "CES") == A::Agrees, "agreement: THOB CES (ba->ces)");
    CHECK(agree("YIN", "CES") == A::DaDragPossible,
          "agreement: YIN CES is the da-drag case");
    CHECK(agree("GANG", "SHES") == A::NotAParticleVariant,
          "agreement: SHES never diagnosed (verb 'to know' ambiguity)");
    CHECK(agree("RNAMS", "ZHES") == A::Agrees,
          "agreement: RNAMS ZHES accepted (dominant corpus convention)");

    // ---- suffix-echo final particles ----
    CHECK(allcore::isFinalParticle("SPANGS", "SO"),
          "echo: SPANGS SO is the final particle (s licenses so)");
    CHECK(allcore::isFinalParticle("YOD", "DO"), "echo: YOD DO");
    CHECK(allcore::isFinalParticle("BYA'", "'O"), "echo: BYA' 'O (vowel/achung)");
    CHECK(!allcore::isFinalParticle("BDEN", "SO"),
          "echo: BDEN SO rejected (n does not license so — lexical so)");
    CHECK(!allcore::isFinalParticle("DANG", "GO"),
          "echo: DANG GO rejected (final is ng, not bare g)");
    CHECK(allcore::isFinalParticle("SNANG", "NGO"), "echo: SNANG NGO");

    // ---- clause splitting ----
    // two clauses joined by zhing, closed by shad + echo final
    auto c1 = split("SEMS CAN LA PHAN PA BYED CING, SDUG BSNGAL SPANGS SO");
    CHECK(c1.size() == 2, "cing passage splits into 2 clauses");
    CHECK(c1.size() == 2 && c1[0].boundary == "cing",
          "clause 1 boundary is the coordinating cing");
    CHECK(c1.size() == 2 && c1[1].boundary == "so",
          "clause 2 boundary is the echo final so");

    // quotative closes the quote clause
    auto c2 = split("BDEN PA BZHI ZHES GSUNGS");
    CHECK(c2.size() == 2 && c2[0].boundary == "zhes",
          "quotative zhes ends the quoted clause");

    // conditional na
    auto c3 = split("KA BA YOD NA PHYI DRO MED");
    CHECK(c3.size() == 2 && c3[0].boundary == "na",
          "conditional na splits the clause");

    // barrier + connective at the same point yields ONE clause, not an empty
    auto c4 = split("MIG YOD STE, RNA BA MED");
    CHECK(c4.size() == 2 && c4[0].boundary == "ste",
          "ste followed by comma closes exactly one clause");

    // clause-initial connective form is NOT a boundary (i > beg guard):
    // NA at position 0 stays inside its clause
    auto c5 = split("NA RO YOD");
    CHECK(c5.size() == 1, "clause-initial NA does not split");

    // passage [598] sanity: comma barriers give 2 clauses, GYI untouched (v1)
    auto c6 = split(
        "SNGA DRO'I KA BA PHYI DRO MED PA DUS MA 'DRES PA'I DON YIN GYI, "
        "KA BA SNGA DRO YOD NA PHYI DRO MED DGOS PA DUS CHA MA 'DRES PA'I DON "
        "MA YIN LA,");
    bool sane = c6.size() >= 3;   // barrier, na, barrier
    CHECK(sane, "passage [598] splits without pathology");
    for (const auto& c : c6)
        CHECK(c.end > c.beg, "no empty clauses");

    // ---- chunks / verb / reading order (spine-backed) ----
    allcore::Spine spine(argv[1]);
    auto analyze = [&](const std::string& acip) {
        auto doc = allcore::buildOverlay(spine, acip);
        auto clauses = allcore::splitClauses(doc.tokens, doc.barrier_after);
        return std::make_pair(std::move(doc), std::move(clauses));
    };

    // four-truths phrase: genitive chunk attaches, la don chunk marked
    {
        auto [doc, cls] = analyze("'PHAGS PA'I BDEN PA BZHI LA");
        CHECK(cls.size() == 1, "four-truths phrase is one clause");
        auto chunks = allcore::chunkClause(doc, cls[0]);
        CHECK(chunks.size() == 2, "four-truths phrase cuts into 2 chunks");
        CHECK(chunks.size() == 2 && chunks[0].marker == "'i" &&
                  chunks[0].marker_fused,
              "chunk 1 carries the fused genitive (dictionary-anchored)");
        CHECK(chunks.size() == 2 && chunks[1].marker == "la",
              "chunk 2 carries the la don");
        auto plan = allcore::planReading(
            chunks, allcore::spotVerb(doc, chunks));
        CHECK(plan.size() == 2 && plan[0].order == 0,
              "genitive chunk gets no number (attaches forward)");
    }

    // agent + verb: sangs rgyas kyis chos bstan
    {
        auto [doc, cls] = analyze("SANGS RGYAS KYIS CHOS BSTAN");
        auto chunks = allcore::chunkClause(doc, cls[0]);
        CHECK(chunks.size() == 2 && chunks[0].marker == "kyis",
              "agent chunk marked by kyis");
        auto verb = allcore::spotVerb(doc, chunks);
        CHECK(verb.confident && verb.wylie == "bstan",
              "verb spotted: bstan, via HGM tense data");
        CHECK(verb.evidence.rfind("tenses:", 0) == 0,
              "verb evidence cites the banked tenses");
        auto plan = allcore::planReading(chunks, verb);
        CHECK(plan.size() == 2 && plan[1].order == 1 && plan[0].order == 2,
              "reading order: verb chunk first, agent second");
    }

    // echo-final clause: spangs so — verb via tenses, chunk excludes SO
    {
        auto [doc, cls] = analyze("SDUG BSNGAL SPANGS SO");
        CHECK(cls.size() == 1 && cls[0].boundary == "so",
              "spangs-so clause closed by the echo final");
        auto chunks = allcore::chunkClause(doc, cls[0]);
        CHECK(!chunks.empty() && chunks.back().end == 3,
              "the final particle is outside every chunk");
        auto verb = allcore::spotVerb(doc, chunks);
        CHECK(verb.confident && verb.wylie == "spangs",
              "verb spotted: spangs, via HGM tense data");
    }

    // no HGM verb evidence -> honestly unverified
    {
        auto [doc, cls] = analyze("BDEN PA BZHI");
        auto chunks = allcore::chunkClause(doc, cls[0]);
        auto verb = allcore::spotVerb(doc, chunks);
        CHECK(!verb.confident,
              "no verb evidence in 'bden pa bzhi' -> unverified, not guessed");
    }

    // ---- na disambiguation (refineClauses) ----
    {
        // verbal na: yod has "to be" gloss evidence -> conditional, split kept
        auto [doc, cls] = analyze("KA BA YOD NA PHYI DRO MED");
        auto ref = allcore::refineClauses(doc, cls);
        CHECK(ref.size() == 2 && ref[0].boundary == "na",
              "na after verb (yod) stays a conditional clause boundary");
    }
    {
        // nominal na: nags "forest" has no verb evidence -> locative, merged
        auto [doc, cls] = analyze("NAGS NA SENG GE YOD");
        CHECK(cls.size() == 2, "raw split still cuts at na");
        auto ref = allcore::refineClauses(doc, cls);
        CHECK(ref.size() == 1,
              "na after noun (nags) merges back into one clause");
        auto chunks = allcore::chunkClause(doc, ref[0]);
        CHECK(chunks.size() >= 2 && chunks[0].marker == "na",
              "merged locative na becomes a chunk marker (in the forest)");
    }
    {
        // debate formula: 'dod na = "if you assert" — conditional, kept
        auto [doc, cls] = analyze("GANG ZAG BDEN GRUB 'DOD NA SKYON YOD");
        auto ref = allcore::refineClauses(doc, cls);
        CHECK(ref.size() == 2 && ref[0].boundary == "na",
              "'dod na kept as conditional (debate formula)");
    }
    {
        // nominalizer + na: byas pa na = "when done" — temporal, kept
        auto [doc, cls] = analyze("LAS BYAS PA NA 'BRAS BU YOD");
        auto ref = allcore::refineClauses(doc, cls);
        CHECK(ref.size() == 2 && ref[0].boundary == "na",
              "pa na kept as the temporal clause (when ...)");
    }
    {
        // fixed phrase: dper na = "for example" — merged, not clausal
        auto [doc, cls] = analyze("DPER NA BUM PA LTA BU");
        auto ref = allcore::refineClauses(doc, cls);
        CHECK(ref.size() == 1, "dper na merges (fixed phrase, not a clause)");
    }

    // ---- Wilson verb classes ----
    CHECK(allcore::classifyVerb("stong") &&
              allcore::classifyVerb("stong")->cls == allcore::VerbClass::Absence,
          "verb class: stong is Class IV absence");
    CHECK(allcore::classifyVerbWithTenses("bltas", "blta lta bltas ltos") &&
              allcore::classifyVerbWithTenses("bltas", "blta lta bltas ltos")
                      ->cls == allcore::VerbClass::AgentiveObj,
          "verb class: bltas resolves through lta's tense row");
    CHECK(allcore::classifyVerb("bum pa") == nullptr,
          "verb class: nouns are not classified (no guessing)");
    CHECK(allcore::classifyVerb("'khrul") &&
              allcore::classifyVerb("'khrul")->cls ==
                  allcore::VerbClass::Attitude,
          "verb class: 'khrul is a Class II attitude verb (ch. 19)");
    CHECK(allcore::classifyVerb("mthong") &&
              allcore::classifyVerb("mthong")->cls ==
                  allcore::VerbClass::AgentiveNom,
          "verb class: mthong is agentive (cognitive verbs are not attitude)");
    {
        // the absence-verb insight: RANG BZHIN GYIS is NOT an agent here
        auto [doc, cls] =
            analyze("CHOS THAMS CAD RANG BZHIN GYIS STONG");
        auto chunks = allcore::chunkClause(doc, cls[0]);
        auto verb = allcore::spotVerb(doc, chunks);
        CHECK(verb.confident && verb.cls &&
                  verb.cls->cls == allcore::VerbClass::Absence,
              "stong spotted as verb via the Wilson tables");
        auto plan = allcore::planReading(chunks, verb);
        bool absenceNote = false;
        for (size_t i = 0; i < chunks.size(); ++i)
            if (chunks[i].marker == "gyis" &&
                std::string(plan[i].how).find("NOT an agent") !=
                    std::string::npos)
                absenceNote = true;
        CHECK(absenceNote,
              "gyis chunk under stong is labeled absence-qualifier, not agent");
    }
    {
        // disjunctive: the dang chunk completes the verb and gets a number
        auto [doc, cls] = analyze("BLO DE 'DOD CHAGS DANG BRAL");
        auto chunks = allcore::chunkClause(doc, cls[0]);
        auto verb = allcore::spotVerb(doc, chunks);
        CHECK(verb.confident && verb.cls &&
                  verb.cls->cls == allcore::VerbClass::Disjunctive,
              "bral spotted as Class IV disjunctive");
        auto plan = allcore::planReading(chunks, verb);
        bool dangNumbered = false;
        for (size_t i = 0; i < chunks.size(); ++i)
            if (chunks[i].marker == "dang" && plan[i].order > 0)
                dangNumbered = true;
        CHECK(dangNumbered,
              "dang chunk under bral gets its own number (verb qualifier)");
    }

    // ---- the full Wilson parse: per-unit designations, contextual cases ----
    {
        auto [doc, cls] = analyze("SANGS RGYAS KYIS CHOS BSTAN");
        auto parses = allcore::wilsonParse(spine, doc, cls);
        CHECK(parses.size() == 1, "parse: one clause");
        const auto& us = parses[0].units;
        auto find = [&](const std::string& text) -> const allcore::ParseUnit* {
            for (const auto& u : us)
                if (u.text == text) return &u;
            return nullptr;
        };
        auto* kyis = find("KYIS");
        CHECK(kyis && kyis->category.find("3rd (agentive") != std::string::npos,
              "parse: KYIS = 3rd case under an agentive verb");
        auto* bstan = find("BSTAN");
        CHECK(bstan && bstan->category.find("verb") == 0,
              "parse: BSTAN designated a verb with its class");
        auto* sr = find("SANGS RGYAS");
        CHECK(sr && sr->category == "dictionary term",
              "parse: sangs rgyas honestly a dictionary term (no POS guess)");
        // full coverage, no overlaps
        int covered = 0;
        bool overlap = false;
        int last = cls[0].beg;
        for (const auto& u : us) {
            overlap |= (u.beg < last);
            last = u.end;
            covered += u.end - u.beg;
        }
        CHECK(!overlap && covered == cls[0].end - cls[0].beg,
              "parse: units cover the clause exactly once");
    }
    {
        // contextual case number: same la-group particle, different verbs
        auto [d1, c1] = analyze("RGYA GAR DU 'GRO");
        auto p1 = allcore::wilsonParse(spine, d1, c1);
        bool second = false;
        for (const auto& u : p1[0].units)
            if (u.text == "DU" && u.category.find("2nd") != std::string::npos)
                second = true;
        CHECK(second, "parse: DU = 2nd case (destination) under motion 'gro");

        auto [d2, c2] = analyze("CHOS THAMS CAD RANG BZHIN GYIS STONG");
        auto p2 = allcore::wilsonParse(spine, d2, c2);
        bool notAgent = false;
        for (const auto& u : p2[0].units)
            if (u.text == "GYIS" &&
                u.category.find("NOT 3rd") != std::string::npos)
                notAgent = true;
        CHECK(notAgent,
              "parse: GYIS under stong is NOT the 3rd case (absence verb)");
    }
    {
        // negation: MI before a verb is the particle, not the noun "person"
        auto [doc, cls] = analyze("SDIG PA MI BYA");
        auto p = allcore::wilsonParse(spine, doc, cls);
        bool neg = false, aux = false;
        for (const auto& u : p[0].units)
            if (u.text == "MI" && u.category == "negation particle") neg = true;
        CHECK(neg, "parse: MI before bya = negation particle (context rule)");

        // auxiliary verb phrase: V + pa-r + auxiliary — using a phrase the
        // dictionary has NOT lexicalized (dgag par bya; lexicalized ones
        // like shes par bya rightly parse as their compound entry)
        auto [d2, c2] = analyze("DGAG PAR BYA");
        auto p2 = allcore::wilsonParse(spine, d2, c2);
        bool infMarker = false;
        for (const auto& u : p2[0].units) {
            if (u.text == "BYA" &&
                u.category.find("auxiliary") != std::string::npos)
                aux = true;
            if (u.text == "PAR" &&
                u.category.rfind("infinitive marker", 0) == 0)
                infMarker = true;
        }
        CHECK(aux && infMarker,
              "parse: dgag par bya = V + infinitive marker + auxiliary");
        // and a lexicalized verb phrase stays one compound unit (max info)
        auto [d4, c4] = analyze("SHES PAR BYA");
        auto p4 = allcore::wilsonParse(spine, d4, c4);
        CHECK(p4[0].units.size() == 1 &&
                  p4[0].units[0].category == "dictionary term",
              "parse: shes par bya stays the compound entry ('learn')");

        // postposition construction: genitive + nang
        auto [d3, c3] = analyze("KHANG PA'I NANG DU");
        auto p3 = allcore::wilsonParse(spine, d3, c3);
        bool post = false;
        for (const auto& u : p3[0].units)
            if (u.category == "postposition" &&
                u.detail.find("genitive") != std::string::npos)
                post = true;
        CHECK(post, "parse: nang after genitive = postposition construction");
    }
    {
        // the Science of the Dots — Wilson's own diagram for this sentence
        // is S, C 3, C 3(NOM side), NOM, VB
        auto [doc, cls] = analyze("SANGS RGYAS KYIS CHOS BSTAN");
        auto p = allcore::wilsonParse(spine, doc, cls);
        const auto& dots = p[0].dots;
        CHECK(dots.size() == 5, "dots: one designation per tsheg");
        CHECK(dots.size() == 5 && dots[0].label == "S",
              "dots: sangs·rgyas is an S dot (within the word)");
        CHECK(dots.size() == 5 && dots[1].label == "C 3" &&
                  dots[2].label == "C 3",
              "dots: both sides of kyis are C 3 dots (case number shown)");
        CHECK(dots.size() == 5 && dots[3].label == "NOM",
              "dots: chos carries the NOM dot before the verb");
        CHECK(dots.size() == 5 && dots[4].label == "VB",
              "dots: final verb carries the VB dot");
    }
    {
        // vocative: interjections carry VOC dots (Wilson's kyi ma kyi hud)
        auto [doc, cls] = analyze("KYI MA KYI HUD SEMS CAN RNAMS");
        auto p = allcore::wilsonParse(spine, doc, cls);
        int voc = 0, interjections = 0;
        for (const auto& u : p[0].units)
            interjections += (u.category == "interjection (vocative)");
        for (const auto& d : p[0].dots) voc += (d.label == "VOC");
        CHECK(interjections == 2 && voc == 2,
              "kyi ma + kyi hud parse as interjections with VOC dots");
    }

    std::printf("%s (%d failures)\n",
                failures ? "READER SMOKE FAILED" : "READER SMOKE OK", failures);
    return failures ? 1 : 0;
}
