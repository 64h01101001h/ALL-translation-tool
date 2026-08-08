#include "allcore/spine.h"

#include <sqlite3.h>

#include <stdexcept>

namespace allcore {
namespace {

std::string columnText(sqlite3_stmt* s, int col) {
    const unsigned char* t = sqlite3_column_text(s, col);
    return t ? reinterpret_cast<const char*>(t) : "";
}

// Minimal JSON string-array parser for the columns build_spine.py writes
// (json.dumps of a list of strings). Not a general JSON parser.
std::vector<std::string> jsonStringArray(const std::string& j) {
    std::vector<std::string> out;
    size_t i = 0;
    while (i < j.size()) {
        if (j[i] != '"') { ++i; continue; }
        std::string s;
        for (++i; i < j.size() && j[i] != '"'; ++i) {
            char c = j[i];
            if (c == '\\' && i + 1 < j.size()) {
                char n = j[++i];
                switch (n) {
                    case 'n': s += '\n'; break;
                    case 't': s += '\t'; break;
                    case 'u': {
                        if (i + 4 < j.size()) {
                            unsigned cp = std::stoul(j.substr(i + 1, 4), nullptr, 16);
                            i += 4;
                            // encode BMP code point as UTF-8 (fields are BMP)
                            if (cp < 0x80) s += static_cast<char>(cp);
                            else if (cp < 0x800) {
                                s += static_cast<char>(0xC0 | (cp >> 6));
                                s += static_cast<char>(0x80 | (cp & 0x3F));
                            } else {
                                s += static_cast<char>(0xE0 | (cp >> 12));
                                s += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
                                s += static_cast<char>(0x80 | (cp & 0x3F));
                            }
                        }
                        break;
                    }
                    default: s += n;
                }
            } else {
                s += c;
            }
        }
        ++i;
        out.push_back(std::move(s));
    }
    return out;
}

std::string rstripTibetanPunct(std::string s) {
    // strip trailing tsheg (U+0F0B: E0 BC 8B) and shad (U+0F0D: E0 BC 8D)
    while (s.size() >= 3) {
        const auto n = s.size();
        unsigned char a = s[n - 3], b = s[n - 2], c = s[n - 1];
        if (a == 0xE0 && b == 0xBC && (c == 0x8B || c == 0x8D)) s.resize(n - 3);
        else break;
    }
    return s;
}

struct Stmt {
    sqlite3_stmt* p = nullptr;
    Stmt(sqlite3* db, const char* sql) {
        if (sqlite3_prepare_v2(db, sql, -1, &p, nullptr) != SQLITE_OK)
            throw std::runtime_error(std::string("spine prepare failed: ") +
                                     sqlite3_errmsg(db));
    }
    ~Stmt() { sqlite3_finalize(p); }
};

constexpr const char* kEntryCols =
    "id, wylie, tibetan, tibetan_source, acip, pronunciation, "
    "pronunciation_source, sanskrit, hopkins_english, hgm_gloss, tier, "
    "status, flags, raw, tenses";

}  // namespace

Spine::Spine(const std::string& db_path) {
    if (sqlite3_open_v2(db_path.c_str(), &db_, SQLITE_OPEN_READONLY, nullptr) !=
        SQLITE_OK) {
        std::string err = db_ ? sqlite3_errmsg(db_) : "open failed";
        sqlite3_close(db_);
        db_ = nullptr;
        throw std::runtime_error("cannot open spine db '" + db_path + "': " + err);
    }
    // sanity: schema + FTS5 present
    try {
        Stmt s(db_, "SELECT 1 FROM entries_fts WHERE entries_fts MATCH 'ka' LIMIT 1");
        sqlite3_step(s.p);
    } catch (...) {
        sqlite3_close(db_);
        db_ = nullptr;
        throw;
    }
}

Spine::~Spine() { sqlite3_close(db_); }

Entry Spine::entryFromRow(void* stmt) const {
    auto* s = static_cast<sqlite3_stmt*>(stmt);
    Entry e;
    e.id = sqlite3_column_int64(s, 0);
    e.wylie = columnText(s, 1);
    e.tibetan = columnText(s, 2);
    e.tibetan_source = columnText(s, 3);
    e.acip = columnText(s, 4);
    e.pronunciation = columnText(s, 5);
    e.pronunciation_card_attested = !columnText(s, 6).empty();
    e.sanskrit_reference = columnText(s, 7);
    e.hopkins_reference = columnText(s, 8);
    e.hgm_gloss = jsonStringArray(columnText(s, 9));
    e.tier = columnText(s, 10);
    e.status = columnText(s, 11);
    e.flags = jsonStringArray(columnText(s, 12));
    e.raw_json = columnText(s, 13);
    e.tenses = columnText(s, 14);
    if (e.tenses == "null" || e.tenses == "[]") e.tenses.clear();

    Stmt v(db_, "SELECT wylie FROM entry_variants WHERE entry_id=?");
    sqlite3_bind_int64(v.p, 1, e.id);
    while (sqlite3_step(v.p) == SQLITE_ROW) {
        auto w = columnText(v.p, 0);
        if (w != e.wylie) e.variants.push_back(std::move(w));
    }
    return e;
}

std::string Spine::metaValue(const std::string& key) const {
    Stmt s(db_, "SELECT value FROM meta WHERE key=?");
    sqlite3_bind_text(s.p, 1, key.c_str(), -1, SQLITE_TRANSIENT);
    return sqlite3_step(s.p) == SQLITE_ROW ? columnText(s.p, 0) : "";
}

std::vector<Entry> Spine::lookup(const std::string& headword) const {
    std::vector<Entry> out;
    auto collect = [&](const char* sql, const std::string& val) {
        std::string q = std::string("SELECT ") + kEntryCols + " FROM entries " + sql;
        Stmt s(db_, q.c_str());
        sqlite3_bind_text(s.p, 1, val.c_str(), -1, SQLITE_TRANSIENT);
        while (sqlite3_step(s.p) == SQLITE_ROW)
            out.push_back(entryFromRow(s.p));
    };
    collect("WHERE wylie=?", headword);
    if (out.empty()) {
        std::string q = std::string("SELECT ") + kEntryCols +
            " FROM entries WHERE id IN "
            "(SELECT entry_id FROM entry_variants WHERE wylie=?)";
        Stmt s(db_, q.c_str());
        sqlite3_bind_text(s.p, 1, headword.c_str(), -1, SQLITE_TRANSIENT);
        while (sqlite3_step(s.p) == SQLITE_ROW)
            out.push_back(entryFromRow(s.p));
    }
    if (out.empty()) collect("WHERE tibetan=?", rstripTibetanPunct(headword));
    if (out.empty()) collect("WHERE acip=?", headword);
    return out;
}

std::vector<Entry> Spine::headwordSearch(const std::string& fts_query,
                                         int limit) const {
    std::vector<Entry> out;
    std::string q = std::string("SELECT ") + kEntryCols +
        " FROM entries WHERE id IN (SELECT rowid FROM entries_fts "
        "WHERE entries_fts MATCH ? ORDER BY rank LIMIT ?)";
    Stmt s(db_, q.c_str());
    sqlite3_bind_text(s.p, 1, fts_query.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(s.p, 2, limit);
    while (sqlite3_step(s.p) == SQLITE_ROW) out.push_back(entryFromRow(s.p));
    return out;
}

std::vector<ReverseHit> Spine::reverseIndex(const std::string& english) const {
    std::vector<ReverseHit> out;
    Stmt s(db_,
           "SELECT wylie, pronunciation, tier FROM reverse_index WHERE english=? "
           "ORDER BY CASE tier WHEN 'curated' THEN 0 WHEN 'glossary' THEN 1 "
           "ELSE 2 END");
    sqlite3_bind_text(s.p, 1, english.c_str(), -1, SQLITE_TRANSIENT);
    while (sqlite3_step(s.p) == SQLITE_ROW)
        out.push_back({columnText(s.p, 0), columnText(s.p, 1), columnText(s.p, 2)});
    return out;
}

std::vector<Entry> Spine::englishSearch(const std::string& fts_query,
                                        bool binding_only, int limit) const {
    std::vector<Entry> out;
    std::string match = binding_only ? "hgm_gloss:(" + fts_query + ")" : fts_query;
    std::string q = std::string("SELECT ") + kEntryCols +
        " FROM entries WHERE id IN (SELECT rowid FROM english_fts "
        "WHERE english_fts MATCH ? ORDER BY rank LIMIT ?)";
    Stmt s(db_, q.c_str());
    sqlite3_bind_text(s.p, 1, match.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(s.p, 2, limit);
    while (sqlite3_step(s.p) == SQLITE_ROW) out.push_back(entryFromRow(s.p));
    return out;
}

std::vector<CorpusSegment> Spine::corpusSearch(const std::string& fts_query,
                                               const std::string& course,
                                               int limit) const {
    std::vector<CorpusSegment> out;
    std::string q =
        "SELECT s.id, s.course, s.seq, s.wylie, s.english, s.acip "
        "FROM corpus_fts f JOIN corpus_segments s ON s.id=f.rowid "
        "WHERE corpus_fts MATCH ?";
    if (!course.empty()) q += " AND s.course=?";
    q += " ORDER BY rank LIMIT ?";
    Stmt s(db_, q.c_str());
    int i = 1;
    sqlite3_bind_text(s.p, i++, fts_query.c_str(), -1, SQLITE_TRANSIENT);
    if (!course.empty())
        sqlite3_bind_text(s.p, i++, course.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(s.p, i, limit);
    while (sqlite3_step(s.p) == SQLITE_ROW) {
        CorpusSegment seg;
        seg.id = sqlite3_column_int64(s.p, 0);
        seg.course = columnText(s.p, 1);
        seg.seq = sqlite3_column_int(s.p, 2);
        seg.wylie = columnText(s.p, 3);
        seg.english = columnText(s.p, 4);
        seg.acip = columnText(s.p, 5);
        out.push_back(std::move(seg));
    }
    return out;
}

std::vector<CorpusSegment> Spine::corpusWindow(const std::string& course, int lo,
                                               int hi) const {
    std::vector<CorpusSegment> out;
    Stmt s(db_,
           "SELECT id, course, seq, wylie, english, acip FROM corpus_segments "
           "WHERE course=? AND seq BETWEEN ? AND ? ORDER BY seq");
    sqlite3_bind_text(s.p, 1, course.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(s.p, 2, lo);
    sqlite3_bind_int(s.p, 3, hi);
    while (sqlite3_step(s.p) == SQLITE_ROW) {
        CorpusSegment seg;
        seg.id = sqlite3_column_int64(s.p, 0);
        seg.course = columnText(s.p, 1);
        seg.seq = sqlite3_column_int(s.p, 2);
        seg.wylie = columnText(s.p, 3);
        seg.english = columnText(s.p, 4);
        seg.acip = columnText(s.p, 5);
        out.push_back(std::move(seg));
    }
    return out;
}

CorpusSegment Spine::corpusSegmentById(long long id) const {
    CorpusSegment seg;
    Stmt s(db_,
           "SELECT id, course, seq, wylie, english, acip FROM corpus_segments "
           "WHERE id=?");
    sqlite3_bind_int64(s.p, 1, id);
    if (sqlite3_step(s.p) == SQLITE_ROW) {
        seg.id = sqlite3_column_int64(s.p, 0);
        seg.course = columnText(s.p, 1);
        seg.seq = sqlite3_column_int(s.p, 2);
        seg.wylie = columnText(s.p, 3);
        seg.english = columnText(s.p, 4);
        seg.acip = columnText(s.p, 5);
    }
    return seg;
}

long long Spine::corpusMaxId() const {
    Stmt s(db_, "SELECT MAX(id) FROM corpus_segments");
    if (sqlite3_step(s.p) == SQLITE_ROW) return sqlite3_column_int64(s.p, 0);
    return 0;
}

std::vector<Entry> Spine::lookupByPronunciation(const std::string& query,
                                                int limit) const {
    std::string fold;
    for (char c : query) {
        if (c >= 'a' && c <= 'z') fold += c;
        else if (c >= 'A' && c <= 'Z') fold += (char)(c - 'A' + 'a');
    }
    std::vector<Entry> out;
    if (fold.empty()) return out;
    auto run = [&](const char* sql, const std::string& arg) {
        Stmt q(db_, sql);
        sqlite3_bind_text(q.p, 1, arg.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(q.p, 2, limit - (int)out.size());
        while (sqlite3_step(q.p) == SQLITE_ROW && (int)out.size() < limit) {
            auto e = entryById(sqlite3_column_int64(q.p, 0));
            bool dup = false;
            for (auto& x : out) dup |= (x.id == e.id);
            if (!dup && e.id) out.push_back(std::move(e));
        }
    };
    run("SELECT entry_id FROM pron_index WHERE fold=? LIMIT ?", fold);
    if ((int)out.size() < limit)
        run("SELECT entry_id FROM pron_index WHERE fold LIKE ? "
            "ORDER BY length(fold) LIMIT ?",
            fold + "%");
    return out;
}

std::vector<std::string> Spine::corpusCourses() const {
    std::vector<std::string> out;
    Stmt s(db_, "SELECT DISTINCT course FROM corpus_segments ORDER BY course");
    while (sqlite3_step(s.p) == SQLITE_ROW) out.push_back(columnText(s.p, 0));
    return out;
}

Entry Spine::entryById(long long id) const {
    // kEntryCols, NOT '*': the physical table order interleaves hgm_source and
    // tenses, so '*' silently misaligned status/flags/tenses in entryFromRow
    Stmt s(db_, (std::string("SELECT ") + kEntryCols +
                 " FROM entries WHERE id=?").c_str());
    sqlite3_bind_int64(s.p, 1, id);
    if (sqlite3_step(s.p) == SQLITE_ROW) return entryFromRow(s.p);
    return Entry{};
}

std::vector<std::pair<long long, std::string>> Spine::allAcipHeadwords() const {
    std::vector<std::pair<long long, std::string>> out;
    out.reserve(110000);
    Stmt s(db_, "SELECT id, acip FROM entries WHERE acip IS NOT NULL AND acip != ''");
    while (sqlite3_step(s.p) == SQLITE_ROW)
        out.emplace_back(sqlite3_column_int64(s.p, 0), columnText(s.p, 1));
    return out;
}

}  // namespace allcore
