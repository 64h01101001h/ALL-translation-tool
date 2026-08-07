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
    std::string part;           // trailing part/volume designation
    std::string language;       // from the extension (Tibetan/English/…)
    bool incomplete = false;    // INC extension or I status
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
    int paragraphs = 0;
};
TranslationPrep formatForTranslation(const std::string& acip_document);

}  // namespace allcore
