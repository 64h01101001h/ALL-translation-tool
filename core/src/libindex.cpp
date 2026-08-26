#include "allcore/searchnorm.h"
#include "allcore/engines.h"
#include "allcore/libindex.h"

#include <sqlite3.h>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <functional>
#include <map>
#include <stdexcept>

#include "gofer_ast.h"

namespace allcore {

namespace {

struct Stmt {
    sqlite3_stmt* p = nullptr;
    Stmt(sqlite3* db, const char* sql) {
        if (sqlite3_prepare_v2(db, sql, -1, &p, nullptr) != SQLITE_OK)
            throw std::runtime_error(std::string("libindex sql: ") +
                                     sqlite3_errmsg(db));
    }
    ~Stmt() { sqlite3_finalize(p); }
};

void exec(sqlite3* db, const char* sql) {
    char* err = nullptr;
    if (sqlite3_exec(db, sql, nullptr, nullptr, &err) != SQLITE_OK) {
        std::string msg = err ? err : "exec failed";
        sqlite3_free(err);
        throw std::runtime_error("libindex exec: " + msg);
    }
}

bool eligible(const std::filesystem::path& p, uintmax_t size) {
    if (size > 10u * 1024 * 1024) return false;
    std::string ext = p.extension().string();
    for (auto& c : ext) c = (char)std::tolower((unsigned char)c);
    return ext == ".txt" || ext == ".acip" || ext == ".md" || ext == ".act" ||
           ext == ".inc" || ext == ".ace";
}

std::string ftsQuote(const std::string& term) {
    std::string q = "\"";
    for (char c : term) {
        if (c == '"') q += "\"\"";
        else q += c;
    }
    q += "\"";
    return q;
}

}  // namespace

LibraryIndex::LibraryIndex(const std::string& db_path) {
    if (sqlite3_open(db_path.c_str(), &db_) != SQLITE_OK) {
        std::string err = db_ ? sqlite3_errmsg(db_) : "open failed";
        sqlite3_close(db_);
        db_ = nullptr;
        throw std::runtime_error("cannot open library index '" + db_path +
                                 "': " + err);
    }
    // PERF-6: WAL - readers never block the writer, a crash mid-update
    // rolls back cleanly, and the fold rebuild can be watched by a
    // second connection. Converts an existing DELETE-mode index on
    // first touch; the -wal/-shm siblings live beside .index.db.
    exec(db_, "PRAGMA journal_mode=WAL");
    exec(db_,
         "CREATE TABLE IF NOT EXISTS files ("
         "  id INTEGER PRIMARY KEY,"
         "  path TEXT UNIQUE NOT NULL,"
         "  mtime INTEGER NOT NULL,"
         "  size INTEGER NOT NULL,"
         "  nlines INTEGER NOT NULL);"
         "CREATE TABLE IF NOT EXISTS lines ("
         "  id INTEGER PRIMARY KEY,"
         "  file_id INTEGER NOT NULL,"
         "  line_no INTEGER NOT NULL,"
         "  text TEXT NOT NULL,"
         "  text_norm TEXT NOT NULL DEFAULT '');"
         "CREATE INDEX IF NOT EXISTS lines_file ON lines(file_id, line_no);"
         "CREATE VIRTUAL TABLE IF NOT EXISTS lines_fts USING fts5("
         "  text, text_norm, content='lines', content_rowid='id',"
         "  tokenize=\"unicode61 tokenchars ''''\");");
    // schema migration: indexes built before the affix-normalized
    // column are derived data — drop and let update() rebuild
    bool hasNorm = false;
    {
        Stmt ti(db_, "PRAGMA table_info(lines)");
        while (sqlite3_step(ti.p) == SQLITE_ROW) {
            const auto* n = sqlite3_column_text(ti.p, 1);
            if (n && std::string((const char*)n) == "text_norm")
                hasNorm = true;
        }
    }
    if (!hasNorm) {
        exec(db_,
             "DROP TABLE IF EXISTS lines_fts;"
             "DROP TABLE IF EXISTS lines;"
             "DROP TABLE IF EXISTS files;");
        exec(db_,
             "CREATE TABLE files (path TEXT PRIMARY KEY,"
             "  mtime INTEGER NOT NULL, size INTEGER NOT NULL,"
             "  nlines INTEGER NOT NULL);"
             "CREATE TABLE lines (id INTEGER PRIMARY KEY,"
             "  file_id INTEGER NOT NULL, line_no INTEGER NOT NULL,"
             "  text TEXT NOT NULL, text_norm TEXT NOT NULL DEFAULT '');"
             "CREATE INDEX lines_file ON lines(file_id, line_no);"
             "CREATE VIRTUAL TABLE lines_fts USING fts5("
             "  text, text_norm, content='lines', content_rowid='id',"
             "  tokenize=\"unicode61 tokenchars ''''\");");
    }
    // schema v3: tokenizer aligned with corpus FTS + search fold
    // extended (pa/ba + verb lemma). Older indexes rebuild once.
    {
        long ver = 0;
        {
            Stmt v(db_, "PRAGMA user_version");
            if (sqlite3_step(v.p) == SQLITE_ROW)
                ver = sqlite3_column_int64(v.p, 0);
        }
        if (ver < 3) {
            exec(db_,
                 "DROP TABLE IF EXISTS lines_fts;"
                 "DELETE FROM lines; DELETE FROM files;");
            exec(db_,
                 "CREATE VIRTUAL TABLE lines_fts USING fts5("
                 "  text, text_norm, content='lines',"
                 "  content_rowid='id',"
                 "  tokenize=\"unicode61 tokenchars ''''\");");
            exec(db_, "PRAGMA user_version=3");
        }
    }
}

LibraryIndex::~LibraryIndex() { sqlite3_close(db_); }

LibraryIndex::UpdateStats LibraryIndex::update(
    const std::string& root, const UpdateProgress& progress) {
    namespace fs = std::filesystem;
    UpdateStats st;
    if (!fs::exists(root))
        throw std::runtime_error("no such directory: " + root);

    // current on-disk state
    std::map<std::string, std::pair<long long, long long>> disk;  // path → mtime,size
    for (auto it = fs::recursive_directory_iterator(
             root, fs::directory_options::skip_permission_denied);
         it != fs::recursive_directory_iterator(); ++it) {
        if (!it->is_regular_file()) continue;
        const auto size = it->file_size();
        if (!eligible(it->path(), size)) continue;
        const auto mt = (long long)fs::last_write_time(it->path())
                            .time_since_epoch()
                            .count();
        disk[fs::relative(it->path(), root).string()] = {mt, (long long)size};
        // SQA PERF-2: the walk alone is thousands of stat() calls on a
        // real collection. Report it (total 0 = still counting) so the
        // caller can paint and stay answerable. Stopping here has
        // written nothing at all — no transaction is open yet.
        if (progress && (disk.size() % 256) == 0 &&
            !progress((int)disk.size(), 0, "")) {
            st.canceled = true;
            return st;
        }
    }

    // fold-generation stamp: text_norm depends on whether the
    // verb-lemma fold was configured when a file was indexed. A
    // mismatch (an index built before the caller configured the
    // fold, or vice versa) forces ONE full reindex so every norm
    // heals; searches keep answering from the old norms until then.
    const int foldGen = globalLemmaFold() ? 1 : 0;
    int prevGen = 0;
    {
        Stmt g(db_, "PRAGMA application_id");
        if (sqlite3_step(g.p) == SQLITE_ROW)
            prevGen = sqlite3_column_int(g.p, 0);
    }
    const bool refold = prevGen != foldGen;
    st.refolded = refold;   // PERF-6: the caller must be able to say so

    exec(db_, "BEGIN");
    // remove vanished files
    {
        Stmt all(db_, "SELECT id, path FROM files");
        std::vector<std::pair<long long, std::string>> gone;
        while (sqlite3_step(all.p) == SQLITE_ROW) {
            const std::string p =
                reinterpret_cast<const char*>(sqlite3_column_text(all.p, 1));
            if (!disk.count(p))
                gone.push_back({sqlite3_column_int64(all.p, 0), p});
        }
        for (auto& [id, p] : gone) {
            Stmt d1(db_,
                    "INSERT INTO lines_fts(lines_fts, rowid, text, text_norm) "
                    "SELECT 'delete', id, text, text_norm FROM lines "
                    "WHERE file_id=?");
            sqlite3_bind_int64(d1.p, 1, id);
            sqlite3_step(d1.p);
            Stmt d2(db_, "DELETE FROM lines WHERE file_id=?");
            sqlite3_bind_int64(d2.p, 1, id);
            sqlite3_step(d2.p);
            Stmt d3(db_, "DELETE FROM files WHERE id=?");
            sqlite3_bind_int64(d3.p, 1, id);
            sqlite3_step(d3.p);
            ++st.removed;
        }
    }
    // add/update
    const int total = (int)disk.size();
    int done = 0;
    for (auto& [path, ms] : disk) {
        // SQA PERF-2: the cancellation point, and it sits HERE — before
        // the file is touched — on purpose. See UpdateProgress in the
        // header: a stop inside the line loop would bank a half-indexed
        // file under its real mtime+size and every later update would
        // skip it as unchanged.
        if (progress && !progress(done, total, path)) {
            st.canceled = true;
            break;
        }
        ++done;
        long long id = -1, mtime = 0, size = 0;
        {
            Stmt g(db_, "SELECT id, mtime, size FROM files WHERE path=?");
            sqlite3_bind_text(g.p, 1, path.c_str(), -1, SQLITE_TRANSIENT);
            if (sqlite3_step(g.p) == SQLITE_ROW) {
                id = sqlite3_column_int64(g.p, 0);
                mtime = sqlite3_column_int64(g.p, 1);
                size = sqlite3_column_int64(g.p, 2);
            }
        }
        if (!refold && id >= 0 && mtime == ms.first &&
            size == ms.second) {
            ++st.unchanged;
            continue;
        }
        if (id >= 0) {
            Stmt d1(db_,
                    "INSERT INTO lines_fts(lines_fts, rowid, text, text_norm) "
                    "SELECT 'delete', id, text, text_norm FROM lines "
                    "WHERE file_id=?");
            sqlite3_bind_int64(d1.p, 1, id);
            sqlite3_step(d1.p);
            Stmt d2(db_, "DELETE FROM lines WHERE file_id=?");
            sqlite3_bind_int64(d2.p, 1, id);
            sqlite3_step(d2.p);
            ++st.updated;
        } else {
            ++st.added;
        }
        std::ifstream f(std::filesystem::path(root) / path);
        std::vector<std::string> lines;
        std::string line;
        while (std::getline(f, line)) {
            if (!line.empty() && line.back() == '\r') line.pop_back();
            lines.push_back(line);
        }
        if (id < 0) {
            Stmt ins(db_,
                     "INSERT INTO files (path, mtime, size, nlines) "
                     "VALUES (?,?,?,?)");
            sqlite3_bind_text(ins.p, 1, path.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_int64(ins.p, 2, ms.first);
            sqlite3_bind_int64(ins.p, 3, ms.second);
            sqlite3_bind_int64(ins.p, 4, (long long)lines.size());
            sqlite3_step(ins.p);
            id = sqlite3_last_insert_rowid(db_);
        } else {
            Stmt up(db_,
                    "UPDATE files SET mtime=?, size=?, nlines=? WHERE id=?");
            sqlite3_bind_int64(up.p, 1, ms.first);
            sqlite3_bind_int64(up.p, 2, ms.second);
            sqlite3_bind_int64(up.p, 3, (long long)lines.size());
            sqlite3_bind_int64(up.p, 4, id);
            sqlite3_step(up.p);
        }
        Stmt li(db_,
                "INSERT INTO lines (file_id, line_no, text, text_norm) "
                "VALUES (?,?,?,?)");
        Stmt lf(db_,
                "INSERT INTO lines_fts(rowid, text, text_norm) "
                "VALUES (?,?,?)");
        for (size_t n = 0; n < lines.size(); ++n) {
            // affix-normalized shadow (same authority as the corpus
            // spine): per token ACIP -> ewts -> strip; stored only
            // when some token actually changed
            std::string norm;
            {
                std::string tok;
                bool changed = false;
                std::string built;
                for (char c : lines[n] + " ") {
                    if (c == ' ' || c == '\t' || c == ',' || c == '/') {
                        if (!tok.empty()) {
                            // ACIP is defined uppercase; a token with
                            // no uppercase letters is already wylie
                            // (e.g. the Release 6 wylie edition) and
                            // must not be re-converted
                            bool upper = false;
                            for (char ch : tok)
                                upper |= (ch >= 'A' && ch <= 'Z');
                            const std::string wy =
                                upper ? acipToEwts(tok) : tok;
                            const std::string st = searchFoldWylie(wy);
                            if (!built.empty()) built += ' ';
                            built += st;
                            changed |= (st != wy);
                            tok.clear();
                        }
                    } else {
                        tok += c;
                    }
                }
                if (changed) norm = built;
            }
            sqlite3_reset(li.p);
            sqlite3_bind_int64(li.p, 1, id);
            sqlite3_bind_int64(li.p, 2, (long long)n + 1);
            sqlite3_bind_text(li.p, 3, lines[n].c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(li.p, 4, norm.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_step(li.p);
            const long long rowid = sqlite3_last_insert_rowid(db_);
            sqlite3_reset(lf.p);
            sqlite3_bind_int64(lf.p, 1, rowid);
            sqlite3_bind_text(lf.p, 2, lines[n].c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(lf.p, 3, norm.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_step(lf.p);
        }
    }
    // The fold stamp asserts "every row in this index was normalised
    // under this fold". A cancelled pass has not finished proving that,
    // so it must NOT stamp: otherwise an interrupted refold never heals
    // and searches silently miss the rows that kept the old norms.
    if (!st.canceled) {
        exec(db_, foldGen ? "PRAGMA application_id=1"
                          : "PRAGMA application_id=0");
        // PERF-6: a completed full refold rewrites every FTS row, which
        // leaves the index maximally fragmented - the measured cold
        // "PA" cost 8.1 s partly for this reason. Merge the b-trees
        // once, now, while the cost is expected, instead of amortising
        // it into the first hundred searches.
        if (st.refolded)
            exec(db_, "INSERT INTO lines_fts(lines_fts) "
                      "VALUES('optimize')");
    }
    // Everything completed before the stop is committed — the index is
    // incremental, so a partial index is valid and resumable.
    exec(db_, "COMMIT");
    st.lines = lineCount();
    return st;
}

long long LibraryIndex::fileCount() const {
    Stmt s(db_, "SELECT COUNT(*) FROM files");
    return sqlite3_step(s.p) == SQLITE_ROW ? sqlite3_column_int64(s.p, 0) : 0;
}

long long LibraryIndex::lineCount() const {
    Stmt s(db_, "SELECT COUNT(*) FROM lines");
    return sqlite3_step(s.p) == SQLITE_ROW ? sqlite3_column_int64(s.p, 0) : 0;
}

std::vector<FileGoferHit> LibraryIndex::search(const std::string& query,
                                               int limit,
                                               bool* truncated,
                                               SearchStats* stats,
                                               const Pump& pump) const {
    // PERF-4: the progress handler is SQLite's own cooperative-abort
    // hook. Every kPumpEveryVmSteps VM instructions it calls the pump;
    // a false return makes the active statement fail with
    // SQLITE_INTERRUPT, which the row loops below treat as end-of-rows
    // and the stats record as an abort. Installed only when a pump is
    // supplied; removed on every exit path by the guard.
    struct PumpGuard {
        sqlite3* db;
        bool armed;
        bool aborted = false;
        static int trampoline(void* self) {
            auto* g = static_cast<PumpGuard*>(self);
            if ((*g->pump)()) return 0;
            g->aborted = true;
            return 1;
        }
        const Pump* pump = nullptr;
        PumpGuard(sqlite3* d, const Pump& p) : db(d), armed(bool(p)) {
            if (armed) {
                pump = &p;
                constexpr int kPumpEveryVmSteps = 2000;
                sqlite3_progress_handler(db, kPumpEveryVmSteps,
                                         &trampoline, this);
            }
        }
        ~PumpGuard() {
            if (armed) sqlite3_progress_handler(db, 0, nullptr, nullptr);
        }
    } pumpGuard(db_, pump);
    using namespace gofer_ast;
    bool cut = false;   // any node hit kScanCap
    auto toks = lex(query);
    Parser parser(toks);
    auto ast = parser.parseQuery();

    struct FWin { long long file = 0; int lo = 0; int hi = 0; };
    // PERF-3: a BARE term needs only `limit` windows - there is no
    // OR/NEAR join downstream that could consume more. Leaves inside a
    // compound query keep the full window (a join partner may sit
    // anywhere in it). "PA" alone: 7.85M matching lines, 1.9s warm, to
    // display 60 - now the scan stops at 61.
    const bool bareTerm = ast->kind == Node::TERM;
    const int scanLimit = bareTerm ? limit : kScanCap;
    std::function<std::vector<FWin>(const Node&)> eval =
        [&](const Node& n) -> std::vector<FWin> {
        switch (n.kind) {
            case Node::TERM: {
                std::vector<FWin> w;
                // The LIMIT is the fix: "PA" alone matches 7,854,758
                // of 14,077,690 lines, every one of which was pushed
                // into this vector to return 60.
                //
                // ORDER BY is not decoration - without it SQLite may
                // return a DIFFERENT window between runs, so a
                // translator re-checking a citation could find the hit
                // gone with nothing changed. But ordering by
                // (file_id, line_no) forced the join across every
                // matching row (7.85M for "PA") before the LIMIT could
                // act. f.rowid gives the same guarantee for less:
                // rowid == lines.id, an INTEGER PRIMARY KEY, stable
                // across VACUUM, monotone in (file, line) for any
                // freshly built index - and FTS5 can walk matches in
                // rowid order and STOP at the limit. The kept window is
                // "first N by id" rather than "first N by (file,line)";
                // identical on a fresh index, deterministic always, and
                // the display sort below orders whatever is kept.
                Stmt s(db_,
                       "SELECT l.file_id, l.line_no FROM lines_fts f "
                       "JOIN lines l ON l.id = f.rowid "
                       "WHERE lines_fts MATCH ? "
                       "ORDER BY f.rowid LIMIT ?");
                const std::string q = ftsQuote(n.term);
                sqlite3_bind_text(s.p, 1, q.c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_bind_int(s.p, 2, scanLimit + 1);
                while (sqlite3_step(s.p) == SQLITE_ROW) {
                    const int ln = sqlite3_column_int(s.p, 1) - 1;
                    w.push_back({sqlite3_column_int64(s.p, 0), ln, ln});
                }
                if (stats) {
                    stats->term_rows_visited += (long long)w.size();
                    if (pumpGuard.aborted) stats->aborted = true;
                }
                if ((int)w.size() > scanLimit) {
                    w.resize(scanLimit);
                    cut = true;
                }
                return w;
            }
            case Node::OR: {
                auto a = eval(*n.lhs), b = eval(*n.rhs);
                a.insert(a.end(), b.begin(), b.end());
                if ((int)a.size() > kScanCap) {   // two capped arms
                    a.resize(kScanCap);
                    cut = true;
                }
                return a;
            }
            case Node::NEAR: {
                auto a = eval(*n.lhs), b = eval(*n.rhs);
                std::map<long long, std::vector<const FWin*>> byFile;
                for (const auto& w : b) byFile[w.file].push_back(&w);
                std::vector<FWin> out;
                // The pane's "AND (same file)" compiles to
                // NEAR/1000000, so every same-file pair passes the gap
                // test and this loop is the full cross product: 18.0 GB
                // for three ordinary words. Bounded, and the cut is
                // reported so the reader is not shown a partial answer
                // as a whole one.
                for (const auto& wa : a) {
                    if ((int)out.size() >= kScanCap) { cut = true; break; }
                    auto it = byFile.find(wa.file);
                    if (it == byFile.end()) continue;
                    for (const auto* wb : it->second) {
                        if ((int)out.size() >= kScanCap) {
                            cut = true;
                            break;
                        }
                        const int gap =
                            std::max({wb->lo - wa.hi, wa.lo - wb->hi, 0});
                        if (gap <= n.near_n)
                            out.push_back({wa.file, std::min(wa.lo, wb->lo),
                                           std::max(wa.hi, wb->hi)});
                    }
                }
                return out;
            }
        }
        return {};
    };
    auto wins = eval(*ast);
    if (truncated) *truncated = cut;
    std::sort(wins.begin(), wins.end(), [](const FWin& a, const FWin& b) {
        if (a.file != b.file) return a.file < b.file;
        if (a.lo != b.lo) return a.lo < b.lo;
        return a.hi < b.hi;
    });
    wins.erase(std::unique(wins.begin(), wins.end(),
                           [](const FWin& a, const FWin& b) {
                               return a.file == b.file && a.lo == b.lo &&
                                      a.hi == b.hi;
                           }),
               wins.end());

    std::vector<FileGoferHit> hits;
    Stmt path(db_, "SELECT path FROM files WHERE id=?");
    Stmt seg(db_,
             "SELECT text FROM lines WHERE file_id=? AND line_no BETWEEN ? "
             "AND ? ORDER BY line_no");
    for (const auto& w : wins) {
        if ((int)hits.size() >= limit) break;
        FileGoferHit h;
        sqlite3_reset(path.p);
        sqlite3_bind_int64(path.p, 1, w.file);
        if (sqlite3_step(path.p) == SQLITE_ROW)
            h.file = reinterpret_cast<const char*>(
                sqlite3_column_text(path.p, 0));
        h.line_lo = w.lo + 1;
        h.line_hi = w.hi + 1;
        sqlite3_reset(seg.p);
        sqlite3_bind_int64(seg.p, 1, w.file);
        sqlite3_bind_int(seg.p, 2, h.line_lo);
        sqlite3_bind_int(seg.p, 3, h.line_hi);
        while (sqlite3_step(seg.p) == SQLITE_ROW)
            h.lines.push_back(reinterpret_cast<const char*>(
                sqlite3_column_text(seg.p, 0)));
        hits.push_back(std::move(h));
    }
    return hits;
}

}  // namespace allcore
