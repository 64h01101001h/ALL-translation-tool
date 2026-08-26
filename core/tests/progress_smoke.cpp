// progress_smoke — the local SRS/progress layer (in-memory db, fixed clock).
#include <cstdio>

#include <sqlite3.h>

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

    // recent keys (newest first, distinct)
    p.recordDrill("openfile", "/a.txt", true, t0 + 70);
    p.recordDrill("openfile", "/b.txt", true, t0 + 71);
    p.recordDrill("openfile", "/a.txt", true, t0 + 72);
    auto rec = p.recentKeys("openfile", 5);
    CHECK(rec.size() == 2 && rec[0] == "/a.txt" && rec[1] == "/b.txt",
          "recentKeys: distinct, newest first");

    // miss taxonomy
    p.recordDrill("miss:order:verb-position", "seg:1", false, t0 + 60);
    p.recordDrill("miss:order:verb-position", "seg:2", false, t0 + 61);
    p.recordDrill("miss:particle:gi-family", "seg:3", false, t0 + 62);
    auto misses = p.topMisses(5);
    CHECK(misses.size() == 2 && misses[0].first == "miss:order:verb-position" &&
              misses[0].second == 2,
          "topMisses groups and ranks the named skills");

    {
        // MEM-3: a NULL in a text column must not SEGV the readers.
        // sqlite3_column_text returns NULL for a NULL cell, and both
        // dueWords and recentKeys built a std::string straight from
        // it. NULLs are plantable by any external tool touching the
        // same file, and this db lives in the user's own folder.
        const std::string fp = "/tmp/progress_mem3_probe.db";
        std::remove(fp.c_str());
        {
            allcore::Progress p2(fp);
            p2.touchWord("real word", 100);
            p2.recordDrill("card", "real key", true, 100);
        }
        sqlite3* raw = nullptr;
        CHECK(sqlite3_open(fp.c_str(), &raw) == SQLITE_OK,
              "mem3: probe db reopens raw");
        char* err = nullptr;
        sqlite3_exec(raw,
            "INSERT INTO vocab (wylie, first_seen, last_seen, views, "
            "ease, interval_days, due) VALUES (NULL, 1,1,1,2.5,1,1);"
            "INSERT INTO events (ts, kind, key, correct) "
            "VALUES (2,'card',NULL,1);", nullptr, nullptr, &err);
        CHECK(err == nullptr, "mem3: NULL rows planted");
        sqlite3_close(raw);
        allcore::Progress p3(fp);
        auto due = p3.dueWords(10, 1000);        // used to SEGV here
        bool fabricated = false;
        for (auto& w : due) if (w.empty()) fabricated = true;
        CHECK(!fabricated && due.size() == 1,
              "mem3: the NULL vocab row is skipped, not fabricated as "
              "an empty word");
        auto rec = p3.recentKeys("card", 10);
        bool fab2 = false;
        for (auto& k : rec) if (k.empty()) fab2 = true;
        CHECK(!fab2 && rec.size() == 1,
              "mem3: the NULL event key is skipped, not fabricated");
        std::remove(fp.c_str());
    }
    std::printf("%s (%d failures)\n",
                failures ? "PROGRESS SMOKE FAILED" : "PROGRESS SMOKE OK",
                failures);
    return failures ? 1 : 0;
}
