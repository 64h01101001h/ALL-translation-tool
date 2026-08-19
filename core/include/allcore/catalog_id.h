// allcore/catalog_id.h — the identity PROPOSAL lane for uncataloged files
// (TODO 9g step 1). A file whose NAME the ACIP decoder cannot read may still
// announce its own identity in its TEXT: the canonical title page. This
// module reads that title and offers ranked, evidence-carrying candidates
// from a bank of known titles.
//
// THE CHARTER STANDS: this SUGGESTS. Nothing here writes the catalog, and
// every candidate carries the extracted title, the matched title, and the
// shared syllables so a cataloger can judge the claim rather than trust it.
//
// MEASURED by catalog_id_smoke over the installed library (kangyur + tengyur
// + sungbum, 2026-08-19; the battery re-measures on every ctest run):
//   · 240 of 414 sampled files carry an extractable title page; the rest are
//     fragments that begin mid-text and honestly yield nothing.
//   · of those 240: 194 top-1 correct (80.8%), 216 correct within the top 5
//     (90.0%), 11 wrong (4.6%), 13 no match (5.4%).
// Wrong answers are mostly GENUINE ambiguity — two Tengyur works can carry
// the same title — which is exactly why a human approves.
//
// TWO THINGS TRIED AND SETTLED BY MEASUREMENT, not by taste:
//   · an edit-distance-1 "near syllable" tier, meant to absorb spelling
//     variants (TSAD/TSHAD, DGONGS/DGONS), LOWERED top-1 from 81.8% to 80.4%
//     while converting only 4 no-matches. Not shipped. Do not re-add it
//     without re-measuring.
//   · the 0.5 floor beat a looser 0.35 on the trade that matters here: it
//     costs 2 correct answers and removes 4 confidently wrong ones. Every
//     candidate spends a cataloger's attention, so the tighter floor wins.
#pragma once

#include <string>
#include <vector>

namespace allcore {

// Title syllables, uppercased, page markers/bracketed notes/parenthetical
// glosses and punctuation removed, whitespace collapsed. Both sides of every
// comparison pass through this.
std::string normalizeAcipTitle(const std::string& text);

struct TitleExtraction {
    bool found = false;
    std::string title;      // normalized — what the matcher compares
    std::string raw;        // the span verbatim, for the evidence panel
    std::string rule;       // "bod-skad-du" | "bzhugs-so"
    std::string sanskrit;   // the RGYA GAR SKAD DU form, when the page has one
    // Alternative readings of the same title page, normalized. Currently the
    // tail after a "… LAS" clause ("from the collected works of X, THE WORK"),
    // which is how Sungbum extracts name themselves.
    std::vector<std::string> variants;
};

// Read the title page out of the head of an ACIP document. Two rules, in
// order of authority:
//   bod-skad-du — the canonical bilingual head "RGYA GAR SKAD DU, <skt>, BOD
//                 SKAD DU, <title>," (Kangyur/Tengyur, and Chinese-source
//                 works with RGYA'I SKAD DU).
//   bzhugs-so   — the title block that closes with "… BZHUGS SO" (Sungbum).
// A file that begins mid-text returns found=false rather than a guess.
TitleExtraction extractAcipTitle(const std::string& text);

struct TitleBankEntry {
    std::string key;     // catalog number as filed, e.g. "TD02022", "S702"
    std::string raw;     // the title as filed
    std::string title;   // normalized
    std::string eng;     // English title when the source carries one
    std::string source;  // "library filename" | "catalog_works.json" | …
};

class TitleBank {
public:
    // Adds one entry; normalizes raw and skips titles under 3 syllables
    // (too short to identify anything). Returns whether it was kept.
    bool add(const std::string& key, const std::string& raw_title,
             const std::string& source, const std::string& eng = "");
    // Walks an installed collection and banks every "NUMBER_TIBETAN TITLE_…"
    // filename it finds (.txt/.act/.inc/.ace; META files skipped). Returns
    // the number of titles banked.
    int addLibraryTree(const std::string& root);
    size_t size() const { return entries_.size(); }
    const std::vector<TitleBankEntry>& entries() const { return entries_; }

private:
    std::vector<TitleBankEntry> entries_;
};

struct IdentityCandidate {
    std::string key, raw, eng, source;
    double score = 0.0;          // shared syllables / longer title's length
    int shared = 0;              // shared syllable count
    int extracted_syllables = 0;
    int bank_syllables = 0;
    std::string basis;           // "exact title" | "syllable overlap"
    std::string matched_variant; // which reading of the title page matched
};

// Ranked candidates, best first. floor_score is the honesty gate: below it a
// title is not offered at all (an empty result means "needs a cataloger",
// which is a real answer — see the header note on why the default is 0.5).
// Never returns more than limit.
std::vector<IdentityCandidate> suggestIdentity(const TitleExtraction& t,
                                               const TitleBank& bank,
                                               int limit = 5,
                                               double floor_score = 0.5);

// ---- volume splitting ("chop assist") --------------------------------------
// The team's #1 manual activity (sessions 4-8 and every continuation
// session): a typed volume holds many works; somebody finds each boundary
// and cuts. This SUGGESTS the boundaries — with evidence — and cuts
// nothing. House rules honored: candidates carry the rule and the raw
// evidence; a "title" that looks like a chapter or a part of one work
// (LE'U etc.) gets a WARNING, not a cut, per "don't split what the author
// didn't split"; the mother copy is never touched by this module at all
// (it only reads).
struct SplitCandidate {
    size_t offset = 0;        // byte offset where the new text likely begins
    std::string rule;         // "bod-skad-du" | "bzhugs-so"
    std::string title;        // normalized title at the boundary
    std::string raw;          // the evidence span, verbatim
    std::string folio;        // nearest preceding page marker, e.g. "213B"
    bool closing_before = false;  // a closing formula (RDZOGS SO / DGE'O /
                                  // MANGGA LAM / BKRA SHIS) shortly before —
                                  // corroborates a boundary
    bool warn = false;        // looks like a chapter/part, not a new work
    std::string warn_reason;
};

// Scan a whole ACIP document for candidate text boundaries. The first
// candidate is usually the volume's own opening (offset near 0) — callers
// display it as segment 1, not as a cut.
std::vector<SplitCandidate> suggestVolumeSplits(const std::string& doc);

// ---- colophon finder --------------------------------------------------------
// The author-determination step from the sessions: "It is our duty to
// find the author" — from the COLOPHON, never assumed from the
// collection, never copied from a catalog without verification; and the
// translator's credit at the very end is NOT the author (session 8).
// This locates candidate colophon clauses near a text's end and labels
// what each one is evidence OF. It identifies nobody itself.
struct ColophonSpan {
    size_t offset = 0;      // byte offset in the document
    std::string text;       // the clause, verbatim (bounded)
    std::string kind;       // "composition" | "translation-credit"
    std::string cue;        // the word that triggered it (SBYAR, BSGYUR…)
};

// Scan the tail of a document for clauses carrying composition verbs
// (SBYAR / MDZAD / BRIS / BRTZAMS / BKOD) or translation credits
// (BSGYUR / LO TS'A). Nearest the end first.
std::vector<ColophonSpan> findColophonCandidates(const std::string& doc,
                                                 int max_spans = 4);

// ---- cleanup + provenance scan ---------------------------------------------
// Session Aug 4: "somebody put a slash between each line [of Jamyang
// Shepa's Abhidharma commentary]… you can't search across the lines
// because there's an extra character there… I'd like to clean it up."
// Plus the standing provenance signals: western-style pagination (page
// marks with no folio A/B side = a typed book = suspect), and lowercase
// runs ("somebody typed in lowercase letters illegally").
struct CleanupScan {
    int total_lines = 0;
    int slash_terminated = 0;     // lines ending in '/'
    bool slash_corruption = false;   // pervasive (>=40% of real lines)
    int lowercase_runs = 0;       // runs of 20+ lowercase letters
    int folio_marks = 0;          // @NNN[AB]
    int western_page_marks = 0;   // @NNN with no A/B side
};
CleanupScan scanAcipCleanup(const std::string& body);

// Remove the line-terminating slashes (only sensible when the scan says
// the corruption is pervasive). Returns the cleaned text and how many
// slashes were removed. NEVER writes anything — the caller decides
// where a cleaned COPY goes; the mother copy is untouched by design.
std::pair<std::string, int> stripLineSlashes(const std::string& body);

}  // namespace allcore
