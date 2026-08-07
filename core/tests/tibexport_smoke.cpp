// tibexport_smoke — ACIP → print-Tibetan export + file nomenclature decoding.
#include <cstdio>
#include <string>

#include "allcore/tibexport.h"

static int failures = 0;
#define CHECK(cond, msg)                                        \
    do {                                                        \
        if (cond) std::printf("  [PASS] %s\n", msg);            \
        else { std::printf("  [FAIL] %s\n", msg); ++failures; } \
    } while (0)

static bool contains(const std::string& hay, const char* needle) {
    return hay.find(needle) != std::string::npos;
}

int main() {
    setvbuf(stdout, nullptr, _IONBF, 0);

    // basic conversion with page marker, bracket stripping, shad rules
    auto r = allcore::exportTibetanUnicode(
        "@012A BSOD NAMS [BP] KA BA, DBANG , ZZZQX ,");
    CHECK(contains(r.unicode, "\xE2\x9F\xAA" "012A" "\xE2\x9F\xAB"),
          "page marker kept as annotation");
    CHECK(contains(r.unicode, "བསོད་ནམས"), "syllables convert with tsheg");
    CHECK(!contains(r.unicode, "BP"), "bracketed corrections stripped (Text only)");
    CHECK(contains(r.unicode, "བ།"), "no tsheg before shad after plain final");
    CHECK(contains(r.unicode, "ང་།"), "tsheg kept before shad after final nga");
    CHECK(r.failures == 1 && r.failed.size() == 1 && r.failed[0] == "ZZZQX",
          "illegal syllable flagged, never guessed");
    CHECK(contains(r.unicode, "\xE2\x9F\xA8"), "failed syllable marked ⟨…⟩");

    // options: drop markers, keep brackets
    allcore::TibetanExportOptions o;
    o.keep_page_markers = false;
    o.keep_brackets = true;
    auto r2 = allcore::exportTibetanUnicode("@5B KA [SIC] BA", o);
    CHECK(!contains(r2.unicode, "5B"), "page markers dropped on request");
    CHECK(contains(r2.unicode, "[SIC]"), "brackets kept on request");

    // double shad
    auto r3 = allcore::exportTibetanUnicode("KA BA;");
    CHECK(contains(r3.unicode, "༎"), "; renders as nyis shad");

    // ---- file nomenclature ----
    auto f1 = allcore::decodeAcipFilename("S5977MA1.ACT");
    CHECK(f1.recognized && f1.collection == "Sungbum Collection" &&
              f1.number == "5977",
          "S5977MA1: Sungbum 5977");
    CHECK(f1.status.find("automated typo checks") != std::string::npos,
          "S5977MA1: status M decoded");
    CHECK(f1.language.find("Tibetan") != std::string::npos,
          "S5977MA1: .ACT = Tibetan approved");
    auto f2 = allcore::decodeAcipFilename("KD0001E2.INC");
    CHECK(f2.recognized && f2.collection == "Kangyur (Derge edition)" &&
              f2.incomplete,
          "KD0001E2.INC: Derge Kangyur, incomplete");
    auto f3 = allcore::decodeAcipFilename("TD4211L.ACT");
    CHECK(f3.recognized && f3.collection == "Tengyur (Derge edition)" &&
              f3.status.find("statistical checks") != std::string::npos,
          "TD4211L: Derge Tengyur, L status");
    auto f4 = allcore::decodeAcipFilename("notes.txt");
    CHECK(!f4.recognized, "non-ACIP names are not force-decoded");

    // ---- BDRC scan links (deterministic Tohoku mapping) ----
    CHECK(allcore::bdrcScanUrl(allcore::decodeAcipFilename("TD4210E.ACT")) ==
              "https://library.bdrc.io/show/bdr:MW23703_4210",
          "TD4210 links to the Derge Tengyur scans (Pramanavarttika)");
    CHECK(allcore::bdrcScanUrl(allcore::decodeAcipFilename("KD0001A.ACT")) ==
              "https://library.bdrc.io/show/bdr:MW22084_0001",
          "KD0001 links to the Derge Kangyur, zero-padded");
    CHECK(allcore::bdrcScanUrl(allcore::decodeAcipFilename("KL0001A.ACT")) ==
              "https://library.bdrc.io/show/bdr:MW26071_0001",
          "KL0001 links to the Lhasa Kangyur");
    CHECK(allcore::bdrcScanUrl(f1).empty(),
          "Sungbum has no Tohoku mapping — honest empty link");

    // ---- STD-002 technical-spelling apparatus form ----
    CHECK(allcore::hgmTechnicalSpelling("nyams mgur") == "nyams-mgur",
          "technical spelling: pair hyphenated (nyams-mgur)");
    CHECK(allcore::hgmTechnicalSpelling("lam rim bsdus don") ==
              "lam-rim bsdus-don",
          "technical spelling: pairs separated by spaces");
    CHECK(allcore::hgmTechnicalSpelling("blo bzang grags pa") ==
              "blo-bzang grags-pa",
          "technical spelling: four-syllable name");
    CHECK(allcore::hgmTechnicalSpelling("chos") == "chos",
          "technical spelling: single syllable unchanged");
    CHECK(allcore::hgmTechnicalSpelling("bden pa bzhi") == "bden-pa bzhi",
          "technical spelling: odd trailing syllable stands alone");

    // ---- STD-007 bibliography entry composer ----
    {
        allcore::BibliographyFields f;
        f.epithets = "Co-ne bla-ma";
        f.author = "Grags-pa bshad-sgrub";
        f.dates = "1675-1748";
        f.english_title =
            "A Brief Clarification of Heart: A Word-by-Word Commentary "
            "to \u201cAn Abbreviated Presentation of the Steps to the "
            "Path\u201d";
        f.tibetan_title =
            "Lam-rim bsdus-don gyi tsig-'grel snying-po mdor-bsdus "
            "gsal-ba";
        f.acip_number = "S00184";
        f.folios = "1a-11a";
        CHECK(allcore::composeBibliographyEntry(f) ==
                  "(Co-ne bla-ma) Grags-pa bshad-sgrub, 1675-1748. "
                  "A Brief Clarification of Heart: A Word-by-Word "
                  "Commentary to \u201cAn Abbreviated Presentation of "
                  "the Steps to the Path\u201d (Lam-rim bsdus-don gyi "
                  "tsig-'grel snying-po mdor-bsdus gsal-ba, "
                  "ACIP S00184), ff. 1a-11a.",
              "composer reproduces published B3 exactly");
        allcore::BibliographyFields g;
        g.author = "N\u0101g\u0101rjuna (Tib: Klu-sgrub)";
        g.dates = "c. 200AD";
        g.english_title = "The String of Precious Jewels";
        g.acip_number = "TD04158";
        CHECK(allcore::composeBibliographyEntry(g) ==
                  "N\u0101g\u0101rjuna (Tib: Klu-sgrub), c. 200AD. "
                  "The String of Precious Jewels (ACIP TD04158).",
              "composer: optional fields omitted cleanly");
    }
    // five-digit catalog refs normalize to BUDA's four digits
    CHECK(allcore::bdrcScanUrl(allcore::decodeAcipFilename("TD04158.ACT")) ==
              "https://library.bdrc.io/show/bdr:MW23703_4158",
          "TD04158 (five-digit catalog ref) links as Tohoku 4158");

    // ---- Mixed Nuts preliminary formatting ----
    {
        auto p = allcore::formatForTranslation(
            "@001A *, ,TSAD MA\nSDE BDUN, GYI RGYAN,\n,BZHUGS SO, ,\n"
            "@001B RGYA GAR [DD] SKAD DU,, DE NAS");
        CHECK(contains(p.text, "[f. 001A]") && contains(p.text, "[f. 001B]"),
              "prep: page markers become inline [f. NNN] references");
        CHECK(contains(p.text, "TSAD MA SDE BDUN"),
              "prep: input line breaks flow into running text");
        CHECK(contains(p.text, ",,\n\n"),
              "prep: double shad breaks the paragraph");
        CHECK(p.notes.size() == 1 &&
                  p.notes[0].find("folio 001B") != std::string::npos &&
                  p.notes[0].find("DD") != std::string::npos,
              "prep: bracketed correction becomes a folio-tagged note");
        CHECK(contains(p.text, "[1]"),
              "prep: note marker numbered in the text");
        CHECK(p.paragraphs >= 2, "prep: paragraphs counted");
    }

    std::printf("%s (%d failures)\n",
                failures ? "TIBEXPORT SMOKE FAILED" : "TIBEXPORT SMOKE OK",
                failures);
    return failures ? 1 : 0;
}
