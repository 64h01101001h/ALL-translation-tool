// allcore/spine.h — read-only access to the compiled HGM spine database.
//
// The spine is built by tools/build_spine.py from a versioned HGM release;
// the app imports a release, never owns the data. hgm_gloss is the binding
// layer (rule 1); sanskrit/hopkins fields are reference comparanda only and
// are exposed under names that say so.
#pragma once

#include <optional>
#include <map>
#include <string>
#include <vector>

struct sqlite3;

namespace allcore {

struct Entry {
    long long id = 0;
    std::string wylie;
    std::string tibetan;
    std::string tibetan_source;   // empty = source-attested; else generated-*
    std::string acip;
    std::string pronunciation;
    bool pronunciation_card_attested = false;
    std::string sanskrit_reference;
    std::string hopkins_reference;
    std::vector<std::string> hgm_gloss;   // binding HGM equivalents
    std::string tier;                     // curated | glossary | auto-aligned | ""
    std::string status;
    std::vector<std::string> flags;
    std::vector<std::string> variants;
    std::string raw_json;                 // complete source entry, verbatim
    std::string tenses;                   // verb tense forms ("spang spong
                                          // spangs spongs"); empty = none banked
    bool provisional() const { return tier == "auto-aligned"; }
};

struct CorpusSegment {
    long long id = 0;
    std::string course;
    int seq = 0;                          // position within course (line unit)
    std::string wylie;
    std::string english;
    std::string acip;
};

struct ReverseHit {
    std::string wylie;
    std::string pronunciation;
    std::string tier;
};

class Spine {
public:
    // Opens read-only; throws std::runtime_error on failure or if the file
    // lacks the expected schema/FTS5.
    explicit Spine(const std::string& db_path);
    ~Spine();
    Spine(const Spine&) = delete;
    Spine& operator=(const Spine&) = delete;

    std::string metaValue(const std::string& key) const;

    // Exact headword lookup; tries wylie, then wylie variants, then tibetan
    // (trailing tsheg/shad ignored), then ACIP.
    std::vector<Entry> lookup(const std::string& headword) const;
    // F4 tier arithmetic: totals recomputed from the database
    int entryCount() const;
    std::map<std::string, int> tierCensus() const;
    // stratified whole-dictionary sample (F1 honesty sweep)
    std::vector<Entry> sampleEntries(int stride, int cap = 5000) const;

    // FTS5 match over wylie/variants/tibetan/acip/pronunciation.
    std::vector<Entry> headwordSearch(const std::string& fts_query,
                                      int limit = 20) const;

    // English reverse lookup: release reverse index, exact phrase.
    std::vector<ReverseHit> reverseIndex(const std::string& english) const;

    // FTS5 over the English side. binding_only=true restricts the match to
    // the hgm_gloss column (rule 1: binding layer separable from reference).
    std::vector<Entry> englishSearch(const std::string& fts_query,
                                     bool binding_only = true,
                                     int limit = 20) const;

    // FTS5 over corpus (wylie/english/acip); optional course filter.
    std::vector<CorpusSegment> corpusSearch(const std::string& fts_query,
                                            const std::string& course = "",
                                            int limit = 20) const;

    // All segments of one course with seq in [lo, hi], in order (line window).
    std::vector<CorpusSegment> corpusWindow(const std::string& course, int lo,
                                            int hi) const;

    // Distinct course ids, sorted.
    std::vector<std::string> corpusCourses() const;

    // pronunciation search (GMR convention): the query is folded to
    // lowercase letters and matched exactly, then by prefix —
    // "jangchub" or "jang chub" finds byang chub. Deterministic fold,
    // no phonetic guessing.
    std::vector<Entry> lookupByPronunciation(const std::string& query,
                                             int limit = 10) const;

    // Single corpus segment by id (id 0 on miss) and the highest segment id —
    // together they give seedable random access for the drill generator.
    CorpusSegment corpusSegmentById(long long id) const;
    long long corpusMaxId() const;

    // Single entry by rowid (empty optional-like: id 0 on miss).
    Entry entryById(long long id) const;

    // (id, acip) for every entry with a non-empty ACIP headword — feeds the
    // overlay's in-memory first-syllable index.
    std::vector<std::pair<long long, std::string>> allAcipHeadwords() const;

private:
    sqlite3* db_ = nullptr;
    Entry entryFromRow(void* stmt) const;
};

}  // namespace allcore
