#include "allcore/progress.h"

#include <sqlite3.h>

#include <stdexcept>

namespace allcore {

namespace {

struct Stmt {
    sqlite3_stmt* p = nullptr;
    Stmt(sqlite3* db, const char* sql) {
        if (sqlite3_prepare_v2(db, sql, -1, &p, nullptr) != SQLITE_OK)
            throw std::runtime_error(std::string("progress sql: ") +
                                     sqlite3_errmsg(db));
    }
    ~Stmt() { sqlite3_finalize(p); }
};

void exec(sqlite3* db, const char* sql) {
    char* err = nullptr;
    if (sqlite3_exec(db, sql, nullptr, nullptr, &err) != SQLITE_OK) {
        std::string msg = err ? err : "exec failed";
        sqlite3_free(err);
        throw std::runtime_error("progress exec: " + msg);
    }
}

// simplified SM-2: ease in [1.3, 3.0], interval in days
constexpr double kEaseStart = 2.5, kEaseMin = 1.3, kEaseMax = 3.0;
constexpr double kEaseUp = 0.1, kEaseDown = 0.3;
constexpr long long kDay = 86400;
constexpr long long kRetry = 600;   // wrong answer: retry in 10 minutes

}  // namespace

Progress::Progress(const std::string& db_path) {
    if (sqlite3_open(db_path.c_str(), &db_) != SQLITE_OK) {
        std::string err = db_ ? sqlite3_errmsg(db_) : "open failed";
        sqlite3_close(db_);
        db_ = nullptr;
        throw std::runtime_error("cannot open progress db '" + db_path +
                                 "': " + err);
    }
    exec(db_,
         "CREATE TABLE IF NOT EXISTS vocab ("
         "  wylie TEXT PRIMARY KEY,"
         "  first_seen INTEGER NOT NULL,"
         "  last_seen INTEGER NOT NULL,"
         "  views INTEGER NOT NULL DEFAULT 1,"
         "  ease REAL NOT NULL,"
         "  interval_days REAL NOT NULL DEFAULT 0,"
         "  due INTEGER NOT NULL);"
         "CREATE TABLE IF NOT EXISTS events ("
         "  id INTEGER PRIMARY KEY,"
         "  ts INTEGER NOT NULL,"
         "  kind TEXT NOT NULL,"
         "  key TEXT,"
         "  correct INTEGER);"
         "CREATE TABLE IF NOT EXISTS segments ("
         "  segment_id INTEGER PRIMARY KEY,"
         "  reads INTEGER NOT NULL DEFAULT 0,"
         "  peeks INTEGER NOT NULL DEFAULT 0,"
         "  last_ts INTEGER NOT NULL);"
         "CREATE INDEX IF NOT EXISTS vocab_due ON vocab(due);");
}

Progress::~Progress() { sqlite3_close(db_); }

void Progress::touchWord(const std::string& wylie, long long now) {
    if (wylie.empty()) return;
    Stmt s(db_,
           "INSERT INTO vocab (wylie, first_seen, last_seen, views, ease, "
           "interval_days, due) VALUES (?,?,?,1,?,0,?) "
           "ON CONFLICT(wylie) DO UPDATE SET views=views+1, last_seen=?");
    sqlite3_bind_text(s.p, 1, wylie.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(s.p, 2, now);
    sqlite3_bind_int64(s.p, 3, now);
    sqlite3_bind_double(s.p, 4, kEaseStart);
    sqlite3_bind_int64(s.p, 5, now);   // new words are due immediately
    sqlite3_bind_int64(s.p, 6, now);
    sqlite3_step(s.p);
}

void Progress::reviewWord(const std::string& wylie, bool knew_it,
                          long long now) {
    Stmt g(db_, "SELECT ease, interval_days FROM vocab WHERE wylie=?");
    sqlite3_bind_text(g.p, 1, wylie.c_str(), -1, SQLITE_TRANSIENT);
    if (sqlite3_step(g.p) != SQLITE_ROW) return;
    double ease = sqlite3_column_double(g.p, 0);
    double interval = sqlite3_column_double(g.p, 1);
    long long due;
    if (knew_it) {
        ease = std::min(kEaseMax, ease + kEaseUp);
        interval = interval <= 0 ? 1.0 : interval * ease;
        due = now + (long long)(interval * kDay);
    } else {
        ease = std::max(kEaseMin, ease - kEaseDown);
        interval = 0;
        due = now + kRetry;
    }
    Stmt u(db_,
           "UPDATE vocab SET ease=?, interval_days=?, due=?, last_seen=? "
           "WHERE wylie=?");
    sqlite3_bind_double(u.p, 1, ease);
    sqlite3_bind_double(u.p, 2, interval);
    sqlite3_bind_int64(u.p, 3, due);
    sqlite3_bind_int64(u.p, 4, now);
    sqlite3_bind_text(u.p, 5, wylie.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_step(u.p);
    recordDrill("vocab", wylie, knew_it, now);
}

std::vector<std::string> Progress::dueWords(int limit, long long now) const {
    std::vector<std::string> out;
    Stmt s(db_,
           "SELECT wylie FROM vocab WHERE due<=? ORDER BY due LIMIT ?");
    sqlite3_bind_int64(s.p, 1, now);
    sqlite3_bind_int(s.p, 2, limit);
    while (sqlite3_step(s.p) == SQLITE_ROW)
        out.push_back(reinterpret_cast<const char*>(sqlite3_column_text(s.p, 0)));
    return out;
}

void Progress::recordDrill(const std::string& kind, const std::string& key,
                           bool correct, long long now) {
    Stmt s(db_, "INSERT INTO events (ts, kind, key, correct) VALUES (?,?,?,?)");
    sqlite3_bind_int64(s.p, 1, now);
    sqlite3_bind_text(s.p, 2, kind.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(s.p, 3, key.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(s.p, 4, correct ? 1 : 0);
    sqlite3_step(s.p);
}

void Progress::recordSegmentRead(long long segment_id, bool peeked,
                                 long long now) {
    Stmt s(db_,
           "INSERT INTO segments (segment_id, reads, peeks, last_ts) "
           "VALUES (?,1,?,?) ON CONFLICT(segment_id) DO UPDATE SET "
           "reads=reads+1, peeks=peeks+?, last_ts=?");
    sqlite3_bind_int64(s.p, 1, segment_id);
    sqlite3_bind_int(s.p, 2, peeked ? 1 : 0);
    sqlite3_bind_int64(s.p, 3, now);
    sqlite3_bind_int(s.p, 4, peeked ? 1 : 0);
    sqlite3_bind_int64(s.p, 5, now);
    sqlite3_step(s.p);
}

std::vector<long long> Progress::peekedSegments(int limit) const {
    std::vector<long long> out;
    Stmt s(db_,
           "SELECT segment_id FROM segments WHERE peeks>0 "
           "ORDER BY peeks DESC, last_ts ASC LIMIT ?");
    sqlite3_bind_int(s.p, 1, limit);
    while (sqlite3_step(s.p) == SQLITE_ROW)
        out.push_back(sqlite3_column_int64(s.p, 0));
    return out;
}

Progress::Stats Progress::stats(long long now) const {
    Stats st;
    {
        Stmt s(db_, "SELECT COUNT(*) FROM vocab");
        if (sqlite3_step(s.p) == SQLITE_ROW)
            st.words_tracked = sqlite3_column_int64(s.p, 0);
    }
    {
        Stmt s(db_, "SELECT COUNT(*) FROM vocab WHERE due<=?");
        sqlite3_bind_int64(s.p, 1, now);
        if (sqlite3_step(s.p) == SQLITE_ROW)
            st.due_now = sqlite3_column_int64(s.p, 0);
    }
    {
        Stmt s(db_,
               "SELECT COUNT(*), COALESCE(SUM(correct),0) FROM events "
               "WHERE kind!='touch'");
        if (sqlite3_step(s.p) == SQLITE_ROW) {
            st.drills_done = sqlite3_column_int64(s.p, 0);
            st.drills_correct = sqlite3_column_int64(s.p, 1);
        }
    }
    {
        Stmt s(db_,
               "SELECT COALESCE(SUM(reads),0), COALESCE(SUM(peeks),0) "
               "FROM segments");
        if (sqlite3_step(s.p) == SQLITE_ROW) {
            st.segments_read = sqlite3_column_int64(s.p, 0);
            st.segments_peeked = sqlite3_column_int64(s.p, 1);
        }
    }
    return st;
}

std::vector<std::pair<std::string, long long>> Progress::topMisses(
    int limit) const {
    std::vector<std::pair<std::string, long long>> out;
    Stmt s(db_,
           "SELECT kind, COUNT(*) FROM events WHERE kind LIKE 'miss:%' "
           "GROUP BY kind ORDER BY COUNT(*) DESC LIMIT ?");
    sqlite3_bind_int(s.p, 1, limit);
    while (sqlite3_step(s.p) == SQLITE_ROW)
        out.emplace_back(
            reinterpret_cast<const char*>(sqlite3_column_text(s.p, 0)),
            sqlite3_column_int64(s.p, 1));
    return out;
}

double Progress::coverage(const std::vector<std::string>& wylie_words) const {
    if (wylie_words.empty()) return 0.0;
    long known = 0;
    Stmt s(db_, "SELECT 1 FROM vocab WHERE wylie=?");
    for (const auto& w : wylie_words) {
        sqlite3_reset(s.p);
        sqlite3_bind_text(s.p, 1, w.c_str(), -1, SQLITE_TRANSIENT);
        if (sqlite3_step(s.p) == SQLITE_ROW) ++known;
    }
    return (double)known / (double)wylie_words.size();
}

}  // namespace allcore
