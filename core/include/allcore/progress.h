// allcore/progress.h — the local progress / spaced-repetition layer
// (docs/PEDAGOGY_ROADMAP.md, build-order item 2).
//
// One SQLite file of the learner's OWN data (the spine stays read-only
// release data): every word they click enters their vocabulary deck; drill
// results and reading peeks are logged; a simplified SM-2 schedule decides
// what is due. All local, offline, no accounts. Timestamps are passed in by
// the caller (seconds since epoch) so scheduling is fully testable.
#pragma once

#include <string>
#include <utility>
#include <vector>

struct sqlite3;

namespace allcore {

class Progress {
public:
    // Opens (or creates) the progress database and its schema.
    explicit Progress(const std::string& db_path);
    ~Progress();
    Progress(const Progress&) = delete;
    Progress& operator=(const Progress&) = delete;

    // -- vocabulary deck (self-building) --
    // The learner met this word (clicked it in the Overlay/Trainer). New
    // words enter the deck due immediately; known words just bump the view
    // count.
    void touchWord(const std::string& wylie, long long now);
    // SRS review result. knew_it=true grows the interval (ease-weighted);
    // false resets it to a short retry.
    void reviewWord(const std::string& wylie, bool knew_it, long long now);
    // Words due for review, oldest-due first.
    std::vector<std::string> dueWords(int limit, long long now) const;

    // -- drills and reading --
    void recordDrill(const std::string& kind, const std::string& key,
                     bool correct, long long now);
    // A parallel-reading segment: peeked=true when the English was revealed.
    void recordSegmentRead(long long segment_id, bool peeked, long long now);
    // Segments the learner peeked at (candidates for resurfacing), most
    // peeked first.
    std::vector<long long> peekedSegments(int limit) const;

    // -- stats and difficulty --
    struct Stats {
        long long words_tracked = 0;
        long long due_now = 0;
        long long drills_done = 0;
        long long drills_correct = 0;
        long long segments_read = 0;
        long long segments_peeked = 0;
    };
    Stats stats(long long now) const;

    // Fraction of the given words the learner has already met (drives the
    // "readable for YOU?" difficulty score).
    double coverage(const std::vector<std::string>& wylie_words) const;

    // Miss taxonomy: skills logged as kind "miss:<skill>", most frequent
    // first — the learner's weak spots, named.
    std::vector<std::pair<std::string, long long>> topMisses(int limit) const;

    // Most recent distinct event keys of one kind (newest first) — powers
    // e.g. the Library pane's recently-opened list (kind "openfile").
    std::vector<std::string> recentKeys(const std::string& kind,
                                        int limit) const;

private:
    sqlite3* db_ = nullptr;
};

}  // namespace allcore
