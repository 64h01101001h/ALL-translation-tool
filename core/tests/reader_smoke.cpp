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

    // Preston p.xviii, verbatim: "gangs ri mig shes la sngon por snang" —
    // snow mountains appear blue to the eye consciousness. The splitter used
    // to cut this at SHES, reading the quotative particle, and hand back
    // "gangs ri mig" as a clause whose verb was mig. mig shes is a dictionary
    // entry, "visual consciousness", and no clause ends in the middle of a
    // word. Found by running the book's own example through the reader.
    {
        auto [doc, cls] = analyze("GANGS RI MIG SHES LA SNGON POR SNANG");
        // splitClauses sees only tokens and barriers, so it DOES cut here —
        // that is the two-stage design, not a bug, and asserting otherwise
        // was my mistake when I first wrote this gate. The lattice lives one
        // stage later.
        CHECK(cls.size() == 2,
              "splitClauses, which has no lattice, still cuts at shes");
        auto ref = allcore::refineClauses(doc, cls);
        CHECK(ref.size() == 1,
              "and refineClauses puts it back, because mig shes is a word "
              "and no clause ends inside one (Preston p.xviii)");
        auto chunks = allcore::chunkClause(doc, ref[0]);
        auto verb = allcore::spotVerb(doc, chunks);
        CHECK(verb.confident && verb.wylie == "snang",
              "the clause verb is snang, not the mig left behind by a split "
              "through the middle of a word");
    }
    // and the quotative must still split where it really is one: a shes that
    // is NOT inside a glossed word keeps its boundary.
    {
        auto [doc, cls] = analyze("SANGS RGYAS KYIS CHOS BSTAN CES GSUNGS");
        auto ref = allcore::refineClauses(doc, cls);
        CHECK(ref.size() >= 2,
              "a real quotative ces still ends its clause \u2014 the anchor "
              "only refuses boundaries that fall inside a word");
    }

    // A chunk boundary may not fall inside a word either — Preston vol.2
    // p.60, whose "dang po gnyis la" the chunker returned as DANG + "PO
    // GNYIS LA". dang po is a dictionary entry, "the first".
    //
    // The three checks below are one fix and its two guard rails. Suppressing
    // every straddled boundary is the obvious move and it is wrong: dang bral
    // and rang bzhin gyis stong straddle theirs too, and there the particle
    // is doing real work. What separates them is whether the glossed span
    // runs to the END of the clause — a span that does contains the
    // predicate and is a verb idiom; one that stops short is a word.
    {
        auto [doc, cls] = analyze("DANG PO GNYIS LA DON SMRA BA ZER");
        auto ref = allcore::refineClauses(doc, cls);
        auto chunks = allcore::chunkClause(doc, ref[0]);
        bool splitDangPo = false;
        for (const auto& ch : chunks)
            if (ch.end - ch.beg == 1 && ch.marker == "dang") splitDangPo = true;
        CHECK(!splitDangPo,
              "dang po is not cut in half by a chunk boundary (Preston "
              "vol.2 p.60)");
    }
    {   // guard rail one: the disjunctive dang still gets its own chunk,
        // because dang bral runs to the end of the clause.
        auto [doc, cls] = analyze("CHOS DANG BRAL");
        auto ref = allcore::refineClauses(doc, cls);
        auto chunks = allcore::chunkClause(doc, ref[0]);
        bool dangChunk = false;
        for (const auto& ch : chunks) if (ch.marker == "dang") dangChunk = true;
        CHECK(dangChunk,
              "and the dang of a disjunctive verb still closes its own chunk "
              "\u2014 the anchor does not swallow a working particle");
    }
    {   // guard rail two: same for the absence verb's gis-family chunk.
        auto [doc, cls] = analyze("CHOS THAMS CAD RANG BZHIN GYIS STONG");
        auto ref = allcore::refineClauses(doc, cls);
        auto chunks = allcore::chunkClause(doc, ref[0]);
        bool gyisChunk = false;
        for (const auto& ch : chunks) if (ch.marker == "gyis") gyisChunk = true;
        CHECK(gyisChunk,
              "and so does the gyis of an absence verb");
    }

    // THE BENEDICTIVE shog, AND ITS HOMOGRAPH.
    //
    // Hannah (1912) gives shog as the imperative of 'ong ba, "to come". Our
    // spine holds 'ong with tense forms and omits the imperative, so
    // "...par shog" had no verb at all — one of the two commonest endings
    // among corpus clauses where none could be found. shog is also a noun,
    // a sheet of paper, and the two are told apart by POSITION: the
    // benedictive is the clause's last token, the noun is followed by what
    // it modifies.
    {
        auto [doc, cls] = analyze("SANGS RGYAS 'GRUB PAR SHOG");
        auto chunks = allcore::chunkClause(doc, cls[0]);
        auto v = allcore::spotVerb(doc, chunks);
        CHECK(v.confident && v.wylie == "shog",
              "benedictive shog is a verb \u2014 ...par shog, may it be "
              "(Hannah 1912: the imperative of 'ong ba)");
    }
    {
        auto [doc, cls] = analyze("BSTAN PA GSAL BYED SHOG");
        auto chunks = allcore::chunkClause(doc, cls[0]);
        auto v = allcore::spotVerb(doc, chunks);
        CHECK(v.confident && v.wylie == "shog",
              "and with no par before it, since the position is what counts");
    }
    {   // THE guard rail. shog bu is paper, and it is not clause-final.
        auto [doc, cls] = analyze("SHOG BU GCIG");
        auto chunks = allcore::chunkClause(doc, cls[0]);
        auto v = allcore::spotVerb(doc, chunks);
        CHECK(!(v.confident && v.wylie == "shog"),
              "but shog bu \u2014 paper \u2014 is NOT a verb: the noun is "
              "followed by what it modifies, so it is never the last token");
    }

    // NO PREDICATE SLOT, NO CONFIDENT VERB.
    //
    // chunkClause gives the final chunk the role "predicate slot" only when
    // it carries no marker. A clause ending at a case particle has no
    // predicate position at all, so anything verb-shaped in that last chunk
    // is inside a marked phrase. spotVerb searched it anyway: measured over
    // 115 clauses of corpus prose, 32 (27%) have no predicate slot and
    // spotVerb returned CONFIDENT on 15 of them (46%).
    {
        // "bstan pa la" and not "bshad pa'i phyir": phyir is NOT a role
        // marker, so that clause DOES have a predicate slot and its verb was
        // being suppressed by the gloss-shape guard instead. The fixture
        // assertion below caught that — it exists for exactly this.
        //
        // Here rule 1 fires on bstan's tense forms and the predicate-slot
        // check is what removes the confidence, which is the rule under test.
        auto [doc, cls] = analyze("BSTAN PA LA");
        auto chunks = allcore::chunkClause(doc, cls[0]);
        auto v = allcore::spotVerb(doc, chunks);
        CHECK(!chunks.empty() && !chunks.back().marker.empty(),
              "no predicate slot: the fixture really does end at a marker \u2014 "
              "without this the check below could pass by never meeting the "
              "case it was written for");
        CHECK(v.evidence.find("tenses:") == 0,
              "no predicate slot: and rule 1 really did fire, so it is the "
              "slot check being tested and not some other guard");
        CHECK(!v.confident,
              "no predicate slot: no verb is reported CONFIDENT, because a "
              "word inside a marked phrase is not in predicate position");
        CHECK(v.chunk >= 0,
              "no predicate slot: the candidate is still named \u2014 the "
              "position is wrong, not the word unknown");
        CHECK(v.evidence.find("no predicate slot") != std::string::npos,
              "no predicate slot: and the evidence line says why");
    }
    {   // The guard must not cost a clause that HAS a predicate slot.
        auto [doc, cls] = analyze("SANGS RGYAS KYIS CHOS BSTAN");
        auto chunks = allcore::chunkClause(doc, cls[0]);
        CHECK(chunks.back().marker.empty(),
              "predicate slot: this clause has one");
        auto v = allcore::spotVerb(doc, chunks);
        CHECK(v.confident && v.wylie == "bstan",
              "predicate slot: and its verb is still found confidently");
    }

    // A CASE PARTICLE IS NOT A VERB.
    //
    // spotVerb's weakest rule accepted any entry with a gloss beginning
    // "to ". la's entry carries 108 glosses, among them "to ... to" — a
    // pattern entry showing the particle in a construction, not a definition.
    // Measured over 3,000 corpus segments, that rule fired on 5.8% of all
    // returns and la and phyir were the two most-returned "verbs" in the
    // sample. la is the commonest particle in Tibetan.
    {
        auto [doc, cls] = analyze("CHOS RNAMS LA");
        auto chunks = allcore::chunkClause(doc, cls[0]);
        auto v = allcore::spotVerb(doc, chunks);
        CHECK(!(v.confident && v.wylie == "la"),
              "la is never a CONFIDENT verb \u2014 a case particle is not a "
              "verb, whatever its gloss list happens to contain");
    }
    {
        auto [doc, cls] = analyze("SANGS RGYAS KYI PHYIR");
        auto chunks = allcore::chunkClause(doc, cls[0]);
        auto v = allcore::spotVerb(doc, chunks);
        CHECK(!(v.confident && v.wylie == "phyir"),
              "and neither is phyir \u2014 its \"to ... to\" is a usage "
              "pattern with no verb after the to");
    }
    {   // The guard must not cost a real verb. bstan has tense forms, so
        // rule 1 fires long before the gloss rule is reached.
        auto [doc, cls] = analyze("SANGS RGYAS KYIS CHOS BSTAN");
        auto chunks = allcore::chunkClause(doc, cls[0]);
        auto v = allcore::spotVerb(doc, chunks);
        CHECK(v.confident && v.wylie == "bstan",
              "and a real verb is still found confidently \u2014 the guard "
              "narrows only the gloss-shaped guess, not the tense evidence");
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
        // a PARTICLE before na is not evidence of a verb. `du` carries a
        // gloss beginning "to " — as do la, phyir, su and tu — and until
        // 2026-09-13 verbEvidenceAt took that at face value, exactly the
        // defect spotVerb had fixed that morning and this function had not.
        // So DE NYID DU NA, "in suchness itself", was cut in two. Real line,
        // C12:265 and C18:358.
        auto [doc, cls] = analyze("DE NYID DU NA STONG PA NYID SGOM PA");
        CHECK(cls.size() == 2, "raw split still cuts at the du-na");
        auto ref = allcore::refineClauses(doc, cls);
        CHECK(ref.size() == 1,
              "na after the particle du merges back \u2014 a particle's "
              "\"to \" gloss is not evidence of a verb");
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
    // allomorph licensing (TRAINER_DESIGN item, closed 2026-08-14):
    // cing/ces after g d b; zhing/zhes after ng n m ' r l/vowels;
    // shing/shes after s — mismatches name the expected form
    CHECK(allcore::connectiveAllomorphCheck("DAG", "CING") == 0,
          "cing licensed after final g");
    CHECK(allcore::connectiveAllomorphCheck("YIN", "ZHING") == 0,
          "zhing licensed after final n");
    CHECK(allcore::connectiveAllomorphCheck("LUS", "SHING") == 0,
          "shing licensed after final s");
    CHECK(allcore::connectiveAllomorphCheck("DAG", "ZHES") == 1 &&
              allcore::expectedAllomorph("DAG", "ZHES") == "CES",
          "zhes after final g flagged; expected ces");
    CHECK(allcore::connectiveAllomorphCheck("BA", "ZHES") == 0,
          "zhes licensed after a vowel");
    CHECK(allcore::connectiveAllomorphCheck("LUS", "CES") == 1 &&
              allcore::expectedAllomorph("LUS", "CES") == "SHES",
          "ces after final s flagged; expected shes");
    CHECK(allcore::connectiveAllomorphCheck("LUS", "KYANG") == -1,
          "non-family token returns -1");

    
    return failures ? 1 : 0;
}
