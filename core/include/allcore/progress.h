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
    // WP-15 (2026-08-26): writes this store could not land (e.g.
    // disk full). The store never throws for them - drills must not
    // crash mid-session - but it never hides them either; a pane
    // that shows progress should disclose a nonzero count (rule 3).
    int writeFailures() const { return write_failures_; }
    ~Progress();
    Progress(const Progress&) = delete;
    Progress& operator=(const Progress&) = delete;

    // -- vocabulary deck (self-building) --
    // The learner met this word (clicked it in the Overlay/Trainer). New
    // words enter the deck due immediately; known words just bump the view
    // count.
    void touchWord(const std::string& wylie, long long now);
    // The same, remembering WHERE the word was met, so its first review
    // can re-present it in that segment rather than bare.
    void touchWord(const std::string& wylie, long long segment_id,
                   long long now);
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

    // ---- Known here / known anywhere (docs/LEARN_TAB_VISION.md) ----
    //
    // A word recognised only where it was met is the weakest form of knowing,
    // and the one a bare flashcard measures. These two readouts separate it
    // from the real thing.
    struct VocabItem {
        std::string wylie;
        long long first_segment = 0;    // where it was met, 0 if unrecorded
        long long transfer_segment = 0; // where it was recognised again
        int stage = 0;                  // 0 met · 1 known here · 2 anywhere
        long long views = 0;
    };

    // Words due for review, oldest due first. `stage` says which kind of
    // review is owed: in its own segment, or in a different one.
    std::vector<VocabItem> dueVocab(long long now, int limit) const;

    // Record a review. `in_segment` is where the learner just saw it, so a
    // correct answer in a DIFFERENT segment from the origin is what promotes
    // a word to "known anywhere" — being right in the same sentence again
    // proves only that the sentence is familiar.
    void reviewWordInContext(const std::string& wylie, bool knew_it,
                             long long in_segment, long long now);

    struct VocabStanding {
        long long met = 0;        // in the deck, not yet reviewed
        long long here = 0;       // recognised in its own segment
        long long anywhere = 0;   // recognised somewhere else
    };
    VocabStanding vocabStanding() const;

    // Per-skill accuracy, from events logged as kind "skill:<skill>" with
    // their correct flag. Misses alone can only ever grow a counter; a
    // denominator is what lets the tab say "right 7 of 9" and, later,
    // whether a weakness actually closed.
    //
    // `min_attempts` is a FLOOR, not a filter for tidiness: below it the
    // caller must say "too few to call a trend" rather than draw one. The
    // Learn tab's fourth binding condition (docs/LEARN_TAB_VISION.md,
    // "Train this") turns on this being enforced here rather than remembered
    // at each call site.
    struct SkillScore {
        std::string skill;      // e.g. "cloze-role:agent/instrument"
        long long attempts = 0;
        long long right = 0;
        bool enough = false;    // attempts >= min_attempts
    };
    std::vector<SkillScore> skillScores(int min_attempts) const;

    // Most recent distinct event keys of one kind (newest first) — powers
    // e.g. the Library pane's recently-opened list (kind "openfile").
    std::vector<std::string> recentKeys(const std::string& kind,
                                        int limit) const;

private:
    sqlite3* db_ = nullptr;
    int write_failures_ = 0;   // WP-15
};

}  // namespace allcore
