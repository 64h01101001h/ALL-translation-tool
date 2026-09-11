// progress_smoke — the local SRS/progress layer (in-memory db, fixed clock).
#include <cstdio>

#include <sqlite3.h>

#include <cstdio>
#include <string>

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
    {   // WP-15 (SQA re-measurement 2026-08-26): a drill store that
        // silently drops writes eats the translator's review history.
        // The page cap makes SQLITE_FULL reachable; the store must
        // COUNT what it could not keep (rule 3).
        const std::string fp = "/tmp/all_progress_wp15.db";
        std::remove(fp.c_str());
        setenv("ALL_PROGRESS_MAX_PAGES", "20", 1);
        allcore::Progress pc(fp);
        for (int i = 0; i < 4000; ++i)
            pc.touchWord("tshig" + std::to_string(i), 1000 + i);
        unsetenv("ALL_PROGRESS_MAX_PAGES");
        CHECK(pc.writeFailures() > 0,
              "a full progress db is COUNTED, never silent (WP-15)");
        allcore::Progress ok(fp + "2");
        ok.touchWord("bden pa", 1);
        CHECK(ok.writeFailures() == 0,
              "a healthy store counts zero write failures (WP-15)");
        std::remove(fp.c_str());
        std::remove((fp + "2").c_str());
    }
    {   // 2026-09-11: the schema migration crashed the app on the SECOND
        // open of any deck. ALTER TABLE throws on "duplicate column name",
        // exec() propagates, and nothing caught it — so every learner would
        // have lost the app on their next launch. Every suite passed anyway,
        // because test databases happen to be fresh. This is the check that
        // would have caught it.
        const std::string fp = "/tmp/all_progress_reopen.db";
        std::remove(fp.c_str());
        { allcore::Progress a(fp); a.touchWord("bden pa", 7, 100); }
        bool reopened = true;
        try {
            allcore::Progress b(fp);
            b.touchWord("chos", 8, 200);
        } catch (...) {
            reopened = false;
        }
        CHECK(reopened,
              "a deck opens a SECOND time without throwing (the migration is "
              "idempotent)");
        std::remove(fp.c_str());
    }
    // ---- schema versioning, and the work it protects ----------------------
    // progress.db is the only file in the product holding something a user
    // cannot get back. These are the three ways a migration loses it.
    {
        auto userVersion = [](const std::string& path) {
            sqlite3* db = nullptr;
            int v = -1;
            if (sqlite3_open(path.c_str(), &db) == SQLITE_OK) {
                sqlite3_stmt* st = nullptr;
                if (sqlite3_prepare_v2(db, "PRAGMA user_version", -1, &st,
                                       nullptr) == SQLITE_OK &&
                    sqlite3_step(st) == SQLITE_ROW)
                    v = sqlite3_column_int(st, 0);
                sqlite3_finalize(st);
            }
            sqlite3_close(db);
            return v;
        };

        const std::string fp = "/tmp/all_progress_schema.db";
        std::remove(fp.c_str());
        { allcore::Progress a(fp); a.touchWord("bden pa", 7, 100); }
        CHECK(userVersion(fp) == 1,
              "schema: a new deck records which format it is in");

        // 1. AN OLD DECK UPGRADES WITHOUT LOSING WORK. This is the case that
        //    matters on the day v28 ships: a deck written before the columns
        //    existed, carrying a year of study.
        const std::string old_ = "/tmp/all_progress_v0.db";
        std::remove(old_.c_str());
        {
            sqlite3* db = nullptr;
            sqlite3_open(old_.c_str(), &db);
            // the ORIGINAL shape: no first_segment/transfer_segment/stage,
            // no user_version
            sqlite3_exec(db,
                         "CREATE TABLE vocab (wylie TEXT PRIMARY KEY,"
                         " first_seen INTEGER NOT NULL,"
                         " last_seen INTEGER NOT NULL,"
                         " views INTEGER NOT NULL DEFAULT 1,"
                         " ease REAL NOT NULL,"
                         " interval_days REAL NOT NULL DEFAULT 0,"
                         " due INTEGER NOT NULL);"
                         "CREATE TABLE events (id INTEGER PRIMARY KEY,"
                         " ts INTEGER NOT NULL, kind TEXT NOT NULL,"
                         " key TEXT, correct INTEGER);"
                         "CREATE TABLE segments (segment_id INTEGER PRIMARY "
                         "KEY, reads INTEGER NOT NULL DEFAULT 0,"
                         " peeks INTEGER NOT NULL DEFAULT 0,"
                         " last_ts INTEGER NOT NULL);"
                         "INSERT INTO vocab VALUES ('sems can', 10, 20, 9,"
                         " 2.5, 4.0, 999);",
                         nullptr, nullptr, nullptr);
            sqlite3_close(db);
        }
        CHECK(userVersion(old_) == 0, "schema: the old deck starts unversioned");
        bool upgraded = true;
        try { allcore::Progress up(old_); } catch (...) { upgraded = false; }
        CHECK(upgraded, "schema: an unversioned deck opens and upgrades");
        CHECK(userVersion(old_) == 1,
              "schema: the upgraded deck records its new format");
        {   // the row that was already there is STILL there, unchanged
            sqlite3* db = nullptr;
            sqlite3_open(old_.c_str(), &db);
            sqlite3_stmt* st = nullptr;
            int views = -1;
            double ease = -1;
            if (sqlite3_prepare_v2(db,
                                   "SELECT views, ease FROM vocab WHERE "
                                   "wylie='sems can'",
                                   -1, &st, nullptr) == SQLITE_OK &&
                sqlite3_step(st) == SQLITE_ROW) {
                views = sqlite3_column_int(st, 0);
                ease = sqlite3_column_double(st, 1);
            }
            sqlite3_finalize(st);
            sqlite3_close(db);
            CHECK(views == 9 && ease > 2.4 && ease < 2.6,
                  "schema: the upgrade carries the learner's existing work "
                  "across untouched");
        }

        // 2. A DECK FROM THE FUTURE IS REFUSED, NOT WRITTEN TO. An older
        //    build cannot know what a newer one's columns mean, and writing
        //    anyway can destroy work the newer build could still read.
        const std::string fut = "/tmp/all_progress_future.db";
        std::remove(fut.c_str());
        { allcore::Progress a(fut); a.touchWord("chos", 1, 100); }
        {
            sqlite3* db = nullptr;
            sqlite3_open(fut.c_str(), &db);
            sqlite3_exec(db, "PRAGMA user_version=99", nullptr, nullptr,
                         nullptr);
            sqlite3_close(db);
        }
        bool refused = false;
        std::string why;
        try {
            allcore::Progress f(fut);
        } catch (const std::exception& e) {
            refused = true;
            why = e.what();
        }
        CHECK(refused, "schema: a deck from a NEWER release is refused");
        CHECK(why.find("newer version") != std::string::npos,
              "schema: and the refusal says so in words a person can act on");

        std::remove(fp.c_str());
        std::remove(old_.c_str());
        std::remove(fut.c_str());
    }

    std::printf("%s (%d failures)\n",
                failures ? "PROGRESS SMOKE FAILED" : "PROGRESS SMOKE OK",
                failures);
    return failures ? 1 : 0;
}
