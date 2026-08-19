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

}  // namespace allcore
