// allcore/tibexport.h — print export: a raw ACIP input file → digital Tibetan
// Unicode, following the ACIP Release IV file conventions (banked from the
// organization's own manual, docs/standards/):
//   · @NNN[A/B] page/folio markers — kept as ⟪NNN⟫ annotations or dropped
//   · [bracketed material] = input-operator corrections and queries — stripped
//     by default (the manual's own "Text only" convention), optionally kept
//   · , ; = shad punctuation → ། ༎ with the print rules (no tsheg before
//     shad except after final ང)
//   · syllables convert through the battery-proven ACIP→EWTS→Unicode chain;
//     anything the engines cannot legally convert renders as ⟨wylie⟩ and is
//     counted — flagged, never guessed (rule 3)
#pragma once

#include <string>
#include <vector>

namespace allcore {

struct TibetanExportOptions {
    bool keep_page_markers = true;   // ⟪012A⟫ annotations in the output
    bool keep_brackets = false;      // keep [bracketed] material verbatim
};

struct TibetanExportResult {
    std::string unicode;             // the converted document
    int syllables = 0;
    int failures = 0;                // ⟨wylie⟩ markers emitted
    std::vector<std::string> failed; // distinct syllables that failed
};

TibetanExportResult exportTibetanUnicode(
    const std::string& acip_document,
    const TibetanExportOptions& opt = TibetanExportOptions{});

// ---- ACIP file nomenclature (Release IV manual pp. 168–172) ----------------
struct AcipFileInfo {
    bool recognized = false;
    std::string collection;     // "Kangyur (Lhasa edition)", "Sungbum", …
    std::string number;         // the 4-digit Tohoku/ACIP catalog core
    std::string status;         // human reading of the verification letter
    std::string part;           // trailing part/volume designation (raw)
    std::string subNumber;      // decoded sub-number when the part
                                // encodes one: plain digits 1-99, or the
                                // toolchain's letter-hundreds scheme
                                // (A=100 … T=290, +ones digit —
                                // CatalogNumber.m, ACIPMaintenance)
    std::string language;       // from the extension (Tibetan/English/…)
    bool incomplete = false;    // INC/INE extension or I status
    bool partial = false;       // trailing P = purposely partial input
};

// Decode an ACIP file name like "S5977MA1.ACT" or "KD0001E2.INC".
AcipFileInfo decodeAcipFilename(const std::string& filename);

// STD-002 (docs/standards/HGM_TRANSLATION_STANDARDS.md): the published
// volumes cite Wylie as "(technical spelling nyams-mgur)" — syllables
// hyphenated in pairs, space between pairs, exactly as in every observed
// example (nyams-mgur · Lam-rim bsdus-don · Blo-bzang grags-pa · dgag-bya).
// This renders a wylie headword in that apparatus form.
std::string hgmTechnicalSpelling(const std::string& wylie);

// The BDRC scans page for a decoded catalog file, or "" when the collection
// has no deterministic mapping. Kangyur/Tengyur ACIP numbers are Tohoku
// numbers, and BUDA's Derge/Lhasa outline IDs encode them directly
// (verified live 2026-08-06: bdr:MW23703_4210 = tshad ma rnam 'grel =
// Tohoku 4210; docs/BDRC_LINKING_DESIGN.md):
//   KD → bdr:MW22084_<n>   (Derge Kangyur)
//   KL → bdr:MW26071_<n>   (Lhasa Kangyur)
//   TD → bdr:MW23703_<n>   (Derge Tengyur)
// Pure string mapping — no network; the link opens in the user's browser.
std::string bdrcScanUrl(const AcipFileInfo& info);

// STD-007 house bibliography entry, assembled exactly as published:
//   (Epithets) Author, dates. English Title (Tibetan title in technical
//   spelling, ACIP S#####), ff. 1a-11a.
// Pure assembly: every field is used verbatim (house technical spelling
// is entered, not derived — lexical hyphenation is not guessable).
struct BibliographyFields {
    std::string epithets;             // without parentheses; optional
    std::string author;               // technical spelling as published
    std::string dates;                // "1675-1748"; optional
    std::string english_title;        // full title with subtitles
    std::string tibetan_title;        // technical spelling; optional
    std::string acip_number;          // "S00184"; optional
    std::string folios;               // "1a-11a"; optional
};
std::string composeBibliographyEntry(const BibliographyFields& f);

// Sanskrit canonical work — the DCC style guide's full template
// (S1 Dharmakīrti example): translation location down to volume,
// section, collection, and edition.
struct SanskritBibFields {
    std::string author_skt;      // Dharmakīrti
    std::string author_tib;      // Chos kyi grags-pa
    std::string dates;           // c. 650ad
    std::string english_title;
    std::string sanskrit_title;  // Pramāṇavārtika
    std::string tibetan_title;
    std::string acip_number;     // TD04210
    std::string folios;          // 94a-151a (en-dashed on output)
    std::string vol_num;         // 1
    std::string vol_letter;      // Ce
    std::string section_en;      // Logical & Perceptual Theory
    std::string section_skt;     // Pramāṇa
    std::string section_tib;     // Tsad-ma
    std::string collection;      // bsTan-'gyur
    std::string edition;         // sDe-dge
};
std::string composeSanskritBibEntry(const SanskritBibFields& f);

// ---- Mixed Nuts preliminary formatting (GMR's demonstrated workflow) -------
// Prepares a raw ACIP input file for the translation team, replicating the
// steps Geshe Michael demonstrated on "Destroying the Darkness" (9/26/20
// video): flow the input lines into running text; re-paragraph at double
// shads (,,); turn every [bracketed correction/query] into a numbered note
// carrying the folio it sits on; turn @page markers into inline [f. NNN]
// references. Verse lineation and house style stay with the human editor
// (Mixed Nuts style guide pending).
struct TranslationPrep {
    std::string text;                  // formatted ACIP, [n] note markers
    std::vector<std::string> notes;    // note n-1: "folio NNN: content"
    // Parses that could not be completed. A malformed bracket is
    // REPORTED, never guessed at: measured on 598 real ACIP files,
    // 2 carry an unterminated bracket and 1 carries nested brackets,
    // so this is a live condition, not a theoretical one.
    std::vector<std::string> flags;
    int paragraphs = 0;
};
TranslationPrep formatForTranslation(const std::string& acip_document);

// The Mixed Nuts house setup, as Geshe Michael performs it in Word at
// the end of the same recording: Palatino Linotype 12, full
// justification, and a page number centred at the foot of every page
// except the first. A .txt cannot carry any of that, so the prep can
// also be written as RTF - which Word opens natively and saves as
// .docx in one step. Paragraph breaks become a blank line, verse line
// breaks a single one, exactly as the ^p / ^p^p passes leave them.
std::string translationPrepToRtf(const TranslationPrep& prep,
                                 const std::string& title_en = "");

}  // namespace allcore
