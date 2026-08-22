// subjects_smoke — the catalogued subject layer, and the line it must
// not cross.
//
// The layer states what a text is ABOUT. It must never be mistaken for
// a statement about how words are READ, because the binding data does
// not contain that: 32 of 12,004 HGM-glossed entries carry any
// parenthetical and about five are register marks. This battery pins
// the facts the layer may assert and the partiality it must confess.
#include <cstdio>
#include <string>

#include "allcore/subjects.h"

static int failures = 0;
#define CHECK(cond, msg)                                        \
    do {                                                        \
        if (cond) std::printf("  [PASS] %s\n", msg);            \
        else { std::printf("  [FAIL] %s\n", msg); ++failures; } \
    } while (0)

int main(int argc, char** argv) {
    if (argc < 2) {
        std::fprintf(stderr, "usage: subjects_smoke <work_subjects.tsv>\n");
        return 2;
    }
    allcore::Subjects s;
    CHECK(s.load(argv[1]), "the subject table loads");
    if (!s.rows()) {
        std::printf("SUBJECTS SMOKE: table empty, refusing to pass\n");
        return 1;
    }
    CHECK(s.rows() > 3000 && s.works() > 1800,
          "the table carries the catalogued corpus, not a stub");

    // ── Vinaya: Adam's ruling, exact-match only ──────────────────────
    CHECK(s.isVinaya("KD1"),
          "the Kangyur Vinaya is flagged as Vinaya");
    {
        bool named = false;
        for (const auto& w : s.forWork("KD1"))
            if (w.vinaya && w.english == "Vowed Morality" &&
                w.tibetan == "'DUL BA")
                named = true;
        CHECK(named,
              "the Vinaya flag rides a heading that names itself in "
              "both languages");
    }
    {
        // The trap, tested against the REAL table rather than against
        // a string this test made up. S419 is catalogued
        // "Works_On_Good-Heart_Teachings_For_Barbarian_Lands--
        // MTHA'_'KHOB_'DUL_BA'I_BLO_SBYONG" — it contains 'DUL_BA and
        // is a lojong text. A substring test would have called it
        // Vinaya. (An earlier version of this check asserted a
        // property of its own literal and proved nothing.)
        bool carriesDulBa = false;
        for (const auto& w : s.forWork("S419"))
            if (w.raw.find("'DUL_BA") != std::string::npos)
                carriesDulBa = true;
        CHECK(carriesDulBa,
              "S419's catalogued heading really does contain 'DUL_BA");
        CHECK(!s.isVinaya("S419"),
              "a lojong text whose heading contains 'DUL_BA is NOT "
              "flagged Vinaya (why the rule is exact-match)");
    }

    // ── partiality is confessed, not hidden ─────────────────────────
    CHECK(s.forWork("ZZ99999").empty(),
          "an uncatalogued work returns NO subject rather than a guess");
    CHECK(!s.isVinaya("ZZ99999"),
          "an unknown work is not silently called Vinaya");
    CHECK(s.forWork("").empty(),
          "an empty work key yields nothing");

    // ── both banks are present and distinguishable ──────────────────
    {
        bool r6 = false, sung = false;
        for (const auto& w : s.forWork("KD1"))
            if (w.source == "r6") r6 = true;
        for (const auto& w : s.forWork("S12065"))
            if (w.source == "sungbum") sung = true;
        CHECK(r6, "R6 headings are labelled with their source");
        CHECK(sung, "Sungbum headings are labelled with their source");
    }
    {
        // a Sungbum row has no English side, and must not pretend to
        bool honest = true;
        for (const auto& w : s.forWork("S12065"))
            if (w.source == "sungbum" && !w.english.empty()) honest = false;
        CHECK(honest,
              "a Sungbum heading leaves English blank rather than "
              "inventing a translation of it");
    }

    std::printf(failures ? "SUBJECTS SMOKE: %d failure(s)\n"
                         : "SUBJECTS SMOKE OK (%d failures)\n",
                failures);
    return failures ? 1 : 0;
}
