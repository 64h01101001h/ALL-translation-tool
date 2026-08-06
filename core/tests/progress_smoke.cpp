// progress_smoke — the local SRS/progress layer (in-memory db, fixed clock).
#include <cstdio>

#include "allcore/progress.h"

static int failures = 0;
#define CHECK(cond, msg)                                        \
    do {                                                        \
        if (cond) std::printf("  [PASS] %s\n", msg);            \
        else { std::printf("  [FAIL] %s\n", msg); ++failures; } \
    } while (0)

int main() {
    allcore::Progress p(":memory:");
    const long long t0 = 1'000'000;

    // new words are due immediately
    p.touchWord("bden pa", t0);
    p.touchWord("'phags pa", t0 + 1);
    auto due = p.dueWords(10, t0 + 2);
    CHECK(due.size() == 2, "two touched words are due immediately");
    CHECK(!due.empty() && due[0] == "bden pa", "oldest-due first");

    // touching again bumps views, does not duplicate
    p.touchWord("bden pa", t0 + 3);
    CHECK(p.stats(t0 + 3).words_tracked == 2, "no duplicate deck entries");

    // correct review pushes the word out ~1 day; it is no longer due
    p.reviewWord("bden pa", true, t0 + 10);
    due = p.dueWords(10, t0 + 20);
    CHECK(due.size() == 1 && due[0] == "'phags pa",
          "reviewed word leaves the due queue");
    CHECK(p.dueWords(10, t0 + 10 + 86400 + 5).size() == 2,
          "reviewed word comes back after its interval");

    // wrong review retries within minutes
    p.reviewWord("'phags pa", false, t0 + 30);
    CHECK(p.dueWords(10, t0 + 31).empty() ||
              p.dueWords(10, t0 + 31)[0] != "'phags pa",
          "missed word is not due immediately");
    CHECK(!p.dueWords(10, t0 + 30 + 700).empty(),
          "missed word retries within minutes");

    // growing intervals: two successes stretch beyond two days
    p.reviewWord("bden pa", true, t0 + 86400 * 2);
    auto d3 = p.dueWords(10, t0 + 86400 * 3);
    bool bdenDue = false;
    for (auto& w : d3) bdenDue |= (w == "bden pa");
    CHECK(!bdenDue, "second success stretches the interval past a day");

    // drills + segments + stats
    p.recordDrill("order", "seg:598", true, t0 + 40);
    p.recordDrill("cloze", "seg:12", false, t0 + 41);
    p.recordSegmentRead(598, true, t0 + 42);
    p.recordSegmentRead(598, false, t0 + 43);
    p.recordSegmentRead(12, false, t0 + 44);
    auto st = p.stats(t0 + 50);
    CHECK(st.words_tracked == 2, "stats: words tracked");
    CHECK(st.drills_done >= 2 && st.drills_correct >= 1,
          "stats: drills logged with correctness");
    CHECK(st.segments_read == 3 && st.segments_peeked == 1,
          "stats: segment reads and peeks");
    auto peeked = p.peekedSegments(5);
    CHECK(peeked.size() == 1 && peeked[0] == 598,
          "peeked segments are resurfacing candidates");

    // coverage
    const double cov = p.coverage({"bden pa", "'phags pa", "nam mkha'"});
    CHECK(cov > 0.6 && cov < 0.7, "coverage: 2 of 3 words known");

    // miss taxonomy
    p.recordDrill("miss:order:verb-position", "seg:1", false, t0 + 60);
    p.recordDrill("miss:order:verb-position", "seg:2", false, t0 + 61);
    p.recordDrill("miss:particle:gi-family", "seg:3", false, t0 + 62);
    auto misses = p.topMisses(5);
    CHECK(misses.size() == 2 && misses[0].first == "miss:order:verb-position" &&
              misses[0].second == 2,
          "topMisses groups and ranks the named skills");

    std::printf("%s (%d failures)\n",
                failures ? "PROGRESS SMOKE FAILED" : "PROGRESS SMOKE OK",
                failures);
    return failures ? 1 : 0;
}
