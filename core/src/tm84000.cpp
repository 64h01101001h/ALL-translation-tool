// tm84000.cpp — see allcore/tm84000.h.
#include "allcore/tm84000.h"

#include <sqlite3.h>
#include <zlib.h>

#include <cstring>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace allcore {
namespace {

std::string columnText(sqlite3_stmt* s, int col) {
    const unsigned char* t = sqlite3_column_text(s, col);
    return t ? reinterpret_cast<const char*>(t) : "";
}

bool readAll(const std::string& path, std::string& out) {
    std::ifstream f(path, std::ios::binary);
    if (!f) return false;
    std::ostringstream ss;
    ss << f.rdbuf();
    out = ss.str();
    return true;
}

// full gzip decompress (same shape as stardict.cpp's — the TSV is a
// plain single-member gzip stream)
bool gunzipAll(const std::string& in, std::string& out) {
    z_stream zs;
    std::memset(&zs, 0, sizeof zs);
    if (inflateInit2(&zs, 15 + 32) != Z_OK) return false;
    zs.next_in = (Bytef*)in.data();
    zs.avail_in = (uInt)in.size();
    char buf[1 << 16];
    int rc = Z_OK;
    while (rc != Z_STREAM_END) {
        zs.next_out = (Bytef*)buf;
        zs.avail_out = sizeof buf;
        rc = inflate(&zs, Z_NO_FLUSH);
        if (rc != Z_OK && rc != Z_STREAM_END) {
            inflateEnd(&zs);
            return false;
        }
        out.append(buf, sizeof buf - zs.avail_out);
    }
    inflateEnd(&zs);
    return true;
}

std::string metaOf(sqlite3* db, const std::string& key) {
    sqlite3_stmt* st = nullptr;
    if (sqlite3_prepare_v2(db, "SELECT value FROM tm_meta WHERE key=?1",
                           -1, &st, nullptr) != SQLITE_OK)
        return "";
    sqlite3_bind_text(st, 1, key.c_str(), -1, SQLITE_TRANSIENT);
    std::string v;
    if (sqlite3_step(st) == SQLITE_ROW) v = columnText(st, 0);
    sqlite3_finalize(st);
    return v;
}

}  // namespace

long long Tm84000::ensureBuilt(const std::string& tsv_gz_path,
                               const std::string& db_path) {
    std::string gz;
    if (!readAll(tsv_gz_path, gz)) return -1;
    const std::string src_size = std::to_string(gz.size());

    // fresh already?
    {
        sqlite3* db = nullptr;
        if (sqlite3_open_v2(db_path.c_str(), &db, SQLITE_OPEN_READONLY,
                            nullptr) == SQLITE_OK) {
            const std::string have = metaOf(db, "source_size");
            const std::string rows = metaOf(db, "rows");
            sqlite3_close(db);
            if (have == src_size && !rows.empty())
                return std::stoll(rows);
        } else if (db) {
            sqlite3_close(db);
        }
    }

    std::string tsv;
    if (!gunzipAll(gz, tsv)) return -1;
    gz.clear();

    std::remove(db_path.c_str());
    sqlite3* db = nullptr;
    if (sqlite3_open(db_path.c_str(), &db) != SQLITE_OK) {
        if (db) sqlite3_close(db);
        return -1;
    }
    const char* ddl =
        "PRAGMA journal_mode=OFF; PRAGMA synchronous=OFF;"
        "CREATE VIRTUAL TABLE tm USING fts5(tibetan, english, "
        "  text_id UNINDEXED, toh UNINDEXED, folio UNINDEXED, "
        "  method UNINDEXED);"
        "CREATE TABLE tm_meta(key TEXT PRIMARY KEY, value TEXT);";
    if (sqlite3_exec(db, ddl, nullptr, nullptr, nullptr) != SQLITE_OK) {
        sqlite3_close(db);
        return -1;
    }
    sqlite3_exec(db, "BEGIN", nullptr, nullptr, nullptr);
    sqlite3_stmt* ins = nullptr;
    sqlite3_prepare_v2(db,
                       "INSERT INTO tm(text_id,toh,folio,method,tibetan,"
                       "english) VALUES(?1,?2,?3,?4,?5,?6)",
                       -1, &ins, nullptr);
    long long rows = 0, skipped = 0;
    size_t pos = 0;
    bool header_seen = false;
    while (pos < tsv.size()) {
        size_t nl = tsv.find('\n', pos);
        if (nl == std::string::npos) nl = tsv.size();
        const std::string line = tsv.substr(pos, nl - pos);
        pos = nl + 1;
        if (line.empty() || line[0] == '#') continue;
        if (!header_seen) {   // the column-name line
            header_seen = true;
            continue;
        }
        // split into exactly 6 fields
        std::string f[6];
        size_t start = 0;
        int fi = 0;
        bool ok = true;
        for (; fi < 5; ++fi) {
            const size_t tab = line.find('\t', start);
            if (tab == std::string::npos) { ok = false; break; }
            f[fi] = line.substr(start, tab - start);
            start = tab + 1;
        }
        if (!ok || line.find('\t', start) != std::string::npos) {
            ++skipped;   // malformed line: skipped, never repaired
            continue;
        }
        f[5] = line.substr(start);
        sqlite3_bind_text(ins, 1, f[0].c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(ins, 2, f[1].c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(ins, 3, f[2].c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(ins, 4, f[3].c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(ins, 5, f[4].c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(ins, 6, f[5].c_str(), -1, SQLITE_TRANSIENT);
        if (sqlite3_step(ins) == SQLITE_DONE) ++rows;
        sqlite3_reset(ins);
    }
    sqlite3_finalize(ins);
    char meta[256];
    std::snprintf(meta, sizeof meta,
                  "INSERT INTO tm_meta VALUES('source_size','%s'),"
                  "('rows','%lld'),('skipped','%lld'),"
                  "('license','CC BY 4.0 - 84000: Translating the "
                  "Words of the Buddha');",
                  src_size.c_str(), rows, skipped);
    sqlite3_exec(db, meta, nullptr, nullptr, nullptr);
    sqlite3_exec(db, "COMMIT", nullptr, nullptr, nullptr);
    sqlite3_close(db);
    return rows;
}

Tm84000::Tm84000(const std::string& db_path) {
    if (sqlite3_open_v2(db_path.c_str(), &db_, SQLITE_OPEN_READONLY,
                        nullptr) != SQLITE_OK) {
        const std::string err =
            db_ ? sqlite3_errmsg(db_) : "sqlite3_open_v2 failed";
        if (db_) sqlite3_close(db_);
        db_ = nullptr;
        throw std::runtime_error("tm84000: cannot open " + db_path +
                                 ": " + err);
    }
    if (metaOf(db_, "rows").empty()) {
        sqlite3_close(db_);
        db_ = nullptr;
        throw std::runtime_error("tm84000: " + db_path +
                                 " lacks the tm_meta schema");
    }
}

Tm84000::~Tm84000() {
    if (db_) sqlite3_close(db_);
}

long long Tm84000::rowCount() const {
    const std::string r = metaOf(db_, "rows");
    return r.empty() ? 0 : std::stoll(r);
}

long long Tm84000::matchCount(const std::string& fts_query) const {
    // SELECT COUNT(*) ... WHERE tm MATCH ? — the uncapped question.
    // -1 means "could not measure", so the caller says so rather than
    // printing a number it did not measure (rule 3).
    sqlite3_stmt* st = nullptr;
    if (sqlite3_prepare_v2(db_, "SELECT COUNT(*) FROM tm WHERE tm MATCH ?1",
                           -1, &st, nullptr) != SQLITE_OK)
        return -1;
    sqlite3_bind_text(st, 1, fts_query.c_str(), -1, SQLITE_TRANSIENT);
    long long n = -1;
    if (sqlite3_step(st) == SQLITE_ROW) n = sqlite3_column_int64(st, 0);
    sqlite3_finalize(st);
    return n;
}

std::vector<TmSegment> Tm84000::search(const std::string& fts_query,
                                       int limit) const {
    std::vector<TmSegment> out;
    sqlite3_stmt* st = nullptr;
    if (sqlite3_prepare_v2(db_,
                           "SELECT text_id,toh,folio,method,tibetan,"
                           "english FROM tm WHERE tm MATCH ?1 "
                           "ORDER BY rank LIMIT ?2",
                           -1, &st, nullptr) != SQLITE_OK)
        return out;
    sqlite3_bind_text(st, 1, fts_query.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(st, 2, limit);
    while (sqlite3_step(st) == SQLITE_ROW) {
        TmSegment s;
        s.text_id = columnText(st, 0);
        s.toh = columnText(st, 1);
        s.folio = columnText(st, 2);
        s.method = columnText(st, 3);
        s.tibetan = columnText(st, 4);
        s.english = columnText(st, 5);
        out.push_back(std::move(s));
    }
    sqlite3_finalize(st);
    return out;
}

}  // namespace allcore
