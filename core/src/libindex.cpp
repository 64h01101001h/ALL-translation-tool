#include "allcore/affixnorm.h"
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
         "  text, text_norm, content='lines', content_rowid='id');");
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
             "  text, text_norm, content='lines', content_rowid='id');");
    }
}

LibraryIndex::~LibraryIndex() { sqlite3_close(db_); }

LibraryIndex::UpdateStats LibraryIndex::update(const std::string& root) {
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
    }

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
    for (auto& [path, ms] : disk) {
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
        if (id >= 0 && mtime == ms.first && size == ms.second) {
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
                            const std::string wy = acipToEwts(tok);
                            const std::string st =
                                stripAffixedParticlesWylie(wy);
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
                                               int limit) const {
    using namespace gofer_ast;
    auto toks = lex(query);
    Parser parser(toks);
    auto ast = parser.parseQuery();

    struct FWin { long long file = 0; int lo = 0; int hi = 0; };
    std::function<std::vector<FWin>(const Node&)> eval =
        [&](const Node& n) -> std::vector<FWin> {
        switch (n.kind) {
            case Node::TERM: {
                std::vector<FWin> w;
                Stmt s(db_,
                       "SELECT l.file_id, l.line_no FROM lines_fts f "
                       "JOIN lines l ON l.id = f.rowid "
                       "WHERE lines_fts MATCH ?");
                const std::string q = ftsQuote(n.term);
                sqlite3_bind_text(s.p, 1, q.c_str(), -1, SQLITE_TRANSIENT);
                while (sqlite3_step(s.p) == SQLITE_ROW) {
                    const int ln = sqlite3_column_int(s.p, 1) - 1;
                    w.push_back({sqlite3_column_int64(s.p, 0), ln, ln});
                }
                return w;
            }
            case Node::OR: {
                auto a = eval(*n.lhs), b = eval(*n.rhs);
                a.insert(a.end(), b.begin(), b.end());
                return a;
            }
            case Node::NEAR: {
                auto a = eval(*n.lhs), b = eval(*n.rhs);
                std::map<long long, std::vector<const FWin*>> byFile;
                for (const auto& w : b) byFile[w.file].push_back(&w);
                std::vector<FWin> out;
                for (const auto& wa : a) {
                    auto it = byFile.find(wa.file);
                    if (it == byFile.end()) continue;
                    for (const auto* wb : it->second) {
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
