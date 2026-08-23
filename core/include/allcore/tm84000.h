// allcore/tm84000.h — the 84000 Translation Memory comparanda layer.
//
// Source: github.com/84000/data-translation-memory — 400k+ aligned
// Tibetan–English segments across 388 canonical texts, combined into
// data/84000/tm_84000.tsv.gz. License: CC BY 4.0 per 84000's own
// canonical Terms of Use table (github.com/84000/all-data/
// Terms_of_Use.md, rev. 2023-06-14: "Translation memory … CC-BY";
// re-verified live 2026-08-20). The TM repo README's BY-NC-ND line is
// the outdated blanket notice that itself links to those Terms.
//
// Tier: REFERENCE COMPARANDA ONLY — 84000's segments are shown beside
// the HGM corpus concordance, always labeled with the 84000
// attribution, and are NEVER presented as HGM material (rule 1).
//
// The TSV ships in git; the FTS5 database is generated on first use
// (data/extracted/, like every other derived cache) and rebuilt when
// the TSV changes.
#pragma once

#include <string>
#include <vector>

struct sqlite3;

namespace allcore {

struct TmSegment {
    std::string text_id;   // 84000 UT id (e.g. UT22084-029-001)
    std::string toh;       // Tohoku ref as the repo names it (toh10)
    std::string folio;     // Degé folio ref (F.175.b) when carried
    std::string method;    // alignment method (v1..v4 / creation tool)
    std::string tibetan;   // Tibetan unicode segment
    std::string english;   // 84000's published English segment
};

class Tm84000 {
public:
    // Build (or refresh) the FTS5 db from the TSV.gz. Rebuilds when the
    // db is missing or its recorded source size differs from the TSV's.
    // Returns the row count, or -1 on failure (nothing guessed: a bad
    // TSV line is skipped and counted, never repaired).
    static long long ensureBuilt(const std::string& tsv_gz_path,
                                 const std::string& db_path);

    // Opens read-only; throws std::runtime_error on failure or if the
    // file lacks the expected schema.
    explicit Tm84000(const std::string& db_path);
    ~Tm84000();
    Tm84000(const Tm84000&) = delete;
    Tm84000& operator=(const Tm84000&) = delete;

    long long rowCount() const;

    // FTS5 MATCH over the tibetan + english columns (caller quotes
    // phrases, same convention as Spine::corpusSearch). Ranked.
    std::vector<TmSegment> search(const std::string& fts_query,
                                  int limit = 50) const;

    // How many segments the same query matches, WITHOUT a cap — the
    // twin of Spine::corpusCount, and for the same reason (SQA DATA-3).
    // search() takes a limit, so its result size can never answer "how
    // many are there": the 84000 card re-queried with a literal 200 and
    // printed "5 of 200" for a term with 55,720 segments, which reads
    // as false scarcity in the published canon. A count is a different
    // question from a page of results and needs its own query.
    // Returns -1 if the query could not be run (never a guessed 0).
    long long matchCount(const std::string& fts_query) const;

private:
    sqlite3* db_ = nullptr;
};

}  // namespace allcore
