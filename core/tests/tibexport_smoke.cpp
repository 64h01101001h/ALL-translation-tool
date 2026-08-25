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
    // toolchain grammar upgrades (ACIPMaintenance CatalogNumber.m):
    CHECK(f1.subNumber == "101",
          "S5977MA1: letter-hundreds sub-number A1 = 101");
    auto f5 = allcore::decodeAcipFilename("S0300E12.ACT");
    CHECK(f5.recognized && f5.subNumber == "12",
          "plain-digit sub-number: 12");
    auto f6 = allcore::decodeAcipFilename("R0002KHA.ACT");
    CHECK(f6.recognized &&
              f6.collection == "Reference — The Great Dictionary" &&
              f6.subNumber.find("KHA") != std::string::npos,
          "R0002KHA: the Great Dictionary, alphabet volume KHA");
    auto f7 = allcore::decodeAcipFilename("S0107E.INE");
    CHECK(f7.recognized && f7.incomplete &&
              f7.language.find("English") != std::string::npos,
          ".INE = incomplete English");
    auto f8 = allcore::decodeAcipFilename("KL0009E.AT1");
    CHECK(f8.recognized &&
              f8.language.find("Tibetan") != std::string::npos,
          ".AT1 = Tibetan working file (toolchain extension table)");

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
        // pinned to the DCC style guide (2023-08-25), which
        // supersedes the earlier single-space pin: en dashes in
        // ranges, two spaces after the dates period
        CHECK(allcore::composeBibliographyEntry(f) ==
                  "(Co-ne bla-ma) Grags-pa bshad-sgrub, "
                  "1675\u20131748.  "
                  "A Brief Clarification of Heart: A Word-by-Word "
                  "Commentary to \u201cAn Abbreviated Presentation of "
                  "the Steps to the Path\u201d (Lam-rim bsdus-don gyi "
                  "tsig-'grel snying-po mdor-bsdus gsal-ba, "
                  "ACIP S00184), ff. 1a\u201311a.",
              "composer follows the DCC guide (en-dash ranges, "
              "house sentence spacing)");
        allcore::BibliographyFields g;
        g.author = "N\u0101g\u0101rjuna (Tib: Klu-sgrub)";
        g.dates = "c. 200AD";
        g.english_title = "The String of Precious Jewels";
        g.acip_number = "TD04158";
        CHECK(allcore::composeBibliographyEntry(g) ==
                  "N\u0101g\u0101rjuna (Tib: Klu-sgrub), c. 200AD.  "
                  "The String of Precious Jewels (ACIP TD04158).",
              "composer: optional fields omitted cleanly");
        {   // the guide's own S1 example, reproduced structurally
            allcore::SanskritBibFields sk;
            sk.author_skt = "Dharmak\u012brti";
            sk.author_tib = "Chos kyi grags-pa";
            sk.dates = "c. 650ad";
            sk.english_title =
                "A Detailed Commentary on Accurate Perception";
            sk.sanskrit_title = "Pram\u0101\u1e47v\u0101rtika";
            sk.tibetan_title =
                "rGyas-pa'i bstan-bcos tsad-ma rnam-'grel";
            sk.acip_number = "TD04210";
            sk.folios = "94a-151a";
            sk.vol_num = "1";
            sk.vol_letter = "Ce";
            sk.section_en = "Logical & Perceptual Theory";
            sk.section_skt = "Pram\u0101\u1e47a";
            sk.section_tib = "Tsad-ma";
            sk.collection = "bsTan-'gyur";
            sk.edition = "sDe-dge";
            CHECK(allcore::composeSanskritBibEntry(sk) ==
                      "Dharmak\u012brti (Tib: Chos kyi grags-pa), "
                      "c. 650ad.  A Detailed Commentary on Accurate "
                      "Perception (Pram\u0101\u1e47v\u0101rtika) "
                      "(Tib: rGyas-pa'i bstan-bcos tsad-ma "
                      "rnam-'grel, Tibetan translation at ACIP "
                      "TD04210, ff. 94a\u2013151a of Vol. 1 [Ce] in "
                      "the Logical & Perceptual Theory Section "
                      "[Pram\u0101\u1e47a, Tsad-ma] of the "
                      "bsTan-'gyur [sDe-dge edition]).",
                  "Sanskrit composer reproduces the guide's S1 "
                  "example");
        }
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
        CHECK(contains(p.text, "[f. 1a]") && contains(p.text, "[f. 1b]"),
              "prep: page markers become inline [f. Na] references");
        CHECK(contains(p.text, "TSAD MA SDE BDUN"),
              "prep: input line breaks flow into running text");
        CHECK(contains(p.text, ",,\n\n"),
              "prep: double shad breaks the paragraph");
        CHECK(p.notes.size() == 1 &&
                  p.notes[0].find("folio 1b") != std::string::npos &&
                  p.notes[0].find("DD") != std::string::npos,
              "prep: bracketed correction becomes a folio-tagged note");
        CHECK(contains(p.text, "[1]"),
              "prep: note marker numbered in the text");
        CHECK(p.paragraphs >= 2, "prep: paragraphs counted");
    }
    {
        // D1: a double shad split across an input line break is still a
        // double shad. ACIP input files wrap at a fixed width, so a nyis
        // shad straddling the wrap is ordinary - and each one is a
        // passage boundary the translator would otherwise never see.
        // Exact equality: a contains() pin here would pass on the very
        // output this test exists to reject, which already contains ",,".
        auto p = allcore::formatForTranslation("BZHUGS SO,\n,BBB");
        CHECK(p.text == "BZHUGS SO,,\n\nBBB",
              "prep: double shad split by a line break still breaks the paragraph");
        CHECK(p.paragraphs == 2,
              "prep: a line-split double shad counts as a paragraph boundary");
        // the same split WITHOUT a final O is a verse line, not a paragraph
        auto v = allcore::formatForTranslation("BZHIN,\n,GANG");
        CHECK(v.text == "BZHIN,\n,GANG",
              "prep: a line-split pair without O is still a verse break");
    }
    {
        // S1: GMR's rule, read off the recording. A paragraph ends at
        // "O, ," - a syllable closing in capital O - and there were 1,737
        // of those. The REMAINING ", ," (679) are lines of quoted verse
        // and take a line break, not a paragraph break. Treating both as
        // paragraphs turns the opening homage verses into prose.
        auto para = allcore::formatForTranslation("BZHUGS SO, ,NEXT");
        CHECK(para.text == "BZHUGS SO,,\n\nNEXT",
              "prep: O + double shad ends a PARAGRAPH");

        auto verse = allcore::formatForTranslation("BZHIN, ,GANG");
        CHECK(verse.text == "BZHIN,\n,GANG",
              "prep: a double shad without a final O is a VERSE line break");
        CHECK(verse.paragraphs == 1,
              "prep: a verse line break is not a new paragraph");

        // capital O only - he warns against confusing it with a zero,
        // and searches with ignore-case OFF.
        auto lower = allcore::formatForTranslation("blo, ,GANG");
        CHECK(lower.text == "blo,\n,GANG",
              "prep: lowercase o does not end a paragraph");

        // the bum shad: ";" becomes a line break PLUS a shad (429 on the
        // demonstrated text). The narration says "a comma"; the pane
        // says ^p, - the screen wins.
        {
            // The house setup, from the tail of the recording: Palatino
            // Linotype 12, full justification, page number centred at
            // the foot, none on the first page.
            auto pr = allcore::formatForTranslation("@001A *, ,AAA BB SO,, CCC");
            const std::string r = allcore::translationPrepToRtf(pr, "Destroying the Darkness");
            auto has=[&](const char* n){ return r.find(n)!=std::string::npos; };
            CHECK(r.rfind("{\\rtf1",0)==0 && r.back()=='}',
                  "rtf: well-formed envelope");
            CHECK(has("Palatino Linotype"), "rtf: Palatino Linotype in the font table");
            CHECK(has("\\fs24"), "rtf: 12pt body (24 half-points)");
            CHECK(has("\\qj"),   "rtf: full justification");
            CHECK(has("\\chpgn"),"rtf: a page-number field in the footer");
            CHECK(has("\\titlepg"),"rtf: first page is treated separately, so it carries no number");
            CHECK(has("Destroying the Darkness"), "rtf: the English title is carried through");
            CHECK(has("[f. 1a]"), "rtf: folio references survive into the document");

            // RTF's own metacharacters must be escaped or Word mis-parses
            allcore::TranslationPrep braces;
            braces.text = "A{B}C\\D";
            const std::string e = allcore::translationPrepToRtf(braces);
            CHECK(e.find("A\\{B\\}C\\\\D")!=std::string::npos,
                  "rtf: braces and backslashes in the text are escaped");
        }

        {
            // D2: an unterminated bracket must NOT silently swallow the
            // rest of the document into a note. 2 of 598 real files.
            auto u = allcore::formatForTranslation("AAA [OOPS never closed");
            CHECK(u.flags.size() == 1,
                  "prep: an unterminated bracket is flagged");
            CHECK(u.text.find("OOPS never closed") != std::string::npos,
                  "prep: the unterminated tail stays in the text, not a note");
            CHECK(u.notes.empty(),
                  "prep: no note is fabricated from a broken bracket");

            // D3: nested brackets must not truncate the note and strand
            // a ']' in the running text. 1 of 598 real files.
            auto n = allcore::formatForTranslation("AAA [outer [inner] tail] BBB");
            CHECK(n.flags.size() == 1, "prep: a nested bracket is flagged");
            // The contract is "left as written", so the outer bracket
            // survives verbatim - removing it would be the guessing this
            // exists to avoid. What must NOT happen is the old behaviour:
            // a note truncated at the inner ']' reading "outer [inner".
            CHECK(n.text.find("outer") != std::string::npos,
                  "prep: the nested bracket's text is left as written");
            for (const auto& note : n.notes)
                CHECK(note.find("outer [inner") == std::string::npos,
                      "prep: no truncated note is fabricated from nesting");

            // a well-formed bracket still works and raises no flag
            auto ok = allcore::formatForTranslation("@1A AAA [DD] BBB");
            CHECK(ok.flags.empty() && ok.notes.size() == 1,
                  "prep: a well-formed bracket still becomes a note, unflagged");
        }

        // Shad RUNS. Measured on the real text (S05501M, 559 KB):
        // 2,168 pairs are spaced ", ,", 9 are tight ",," - and every
        // tight one follows a final O, i.e. a paragraph end where the
        // input operator omitted the space. 26 runs are FOUR commas
        // ",, ,,", every one after a final O: a doubled nyis shad
        // marking a major section break. A four-run must not be split
        // into two pairs - that leaves an orphan ",," on a line of its
        // own, which is the one shape the standard forbids.
        auto four = allcore::formatForTranslation("ZIN TO,, ,,THAR LAM");
        CHECK(four.text == "ZIN TO,,\n\n,,THAR LAM",
              "prep: ,, ,, is one section break, both shads kept, no orphan");
        // ...and a four-run that does NOT follow an O is still a
        // section break: all three real cases are followed by "DA NI".
        auto four2 = allcore::formatForTranslation("NUS SAM,, ,,DA NI");
        CHECK(four2.text == "NUS SAM,,\n\n,,DA NI",
              "prep: a doubled nyis shad breaks regardless of the final O");
        auto tight = allcore::formatForTranslation("BZHUGS SO,, NEXT");
        CHECK(tight.text == "BZHUGS SO,,\n\nNEXT",
              "prep: a tight ,, after O is still a paragraph end");
        // no ",," may ever sit in running text without its break
        for (const char* src : {"ZIN TO,, ,,THAR", "BZHUGS SO,, NEXT",
                                "BZHIN, ,GANG", "AAA, ,BBB, ,CCC"}) {
            auto r = allcore::formatForTranslation(src);
            size_t k = 0; bool orphan = false;
            while ((k = r.text.find(",,", k)) != std::string::npos) {
                if (r.text.compare(k, 4, ",,\n\n") != 0 &&
                    !(k + 2 < r.text.size() && r.text[k + 2] == ',') &&
                    !(k >= 1 && r.text[k - 1] == '\n'))
                    orphan = true;
                k += 2;
            }
            CHECK(!orphan, "prep: no orphan ,, left in running text");
        }

        {
            // Recto ornamentation. Verified at 21:05: find "A *,^p,",
            // replace "a] " - 192 matches. The A-side of a folio carries
            // a traditional ornament coded "*, ,", and it is deleted.
            // Measured on the real text: 186 of 187 recto markers are
            // followed by "*, "; NO verso marker is. Asterisk only and
            // recto only, exactly as his pattern has it - a "#" is left
            // alone because he never touches one.
            auto r = allcore::formatForTranslation("@001A *, ,TSAD MA");
            CHECK(r.text == "[f. 1a] TSAD MA",
                  "prep: recto ornamentation is stripped with the folio");
            auto v = allcore::formatForTranslation("@001B #, ,RGYA GAR");
            CHECK(v.text.find("#") != std::string::npos,
                  "prep: a # is not ornamentation and is left as written");
            auto rh = allcore::formatForTranslation("@001A #, ,X");
            CHECK(rh.text.find("#") != std::string::npos,
                  "prep: even on the recto, only an asterisk is stripped");
        }

        // S3: the stipulated folio form is [f. 1a] - leading zeros
        // stripped (he replaces "[f. 00" with "[f. ") and the side
        // letter lowercased ("A]" -> "a]", ignore-case OFF).
        auto fol = allcore::formatForTranslation("@001A TEXT");
        CHECK(fol.text.find("[f. 1a]") != std::string::npos,
              "prep: folio renders as [f. 1a], zeros stripped and side lowercased");
        CHECK(fol.text.find("001A") == std::string::npos,
              "prep: the raw padded folio does not survive");
        auto fol2 = allcore::formatForTranslation("@012B TEXT");
        CHECK(fol2.text.find("[f. 12b]") != std::string::npos,
              "prep: verso folio too - 12b, not 012B");

        auto bum = allcore::formatForTranslation("AAA;BBB");
        CHECK(bum.text == "AAA\n,BBB",
              "prep: bum shad becomes a line break and a leading shad");
    }

    std::printf("%s (%d failures)\n",
                failures ? "TIBEXPORT SMOKE FAILED" : "TIBEXPORT SMOKE OK",
                failures);
    return failures ? 1 : 0;
}
