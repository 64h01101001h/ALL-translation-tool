#pragma once

#include <sqlite3.h>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

namespace drillpack {
struct EvidenceHead {
    std::string wylie;
    std::vector<std::pair<std::string, int>> renderings;
};

// Decode the evidence object, retaining source head/array order. SQLite is
// already linked by allcore; JSON support is checked by statement preparation.
// This connection is memory-only and never opens the source spine or a file.
inline std::vector<EvidenceHead> parseEvidence(const std::string& json) {
    if (json.size() > static_cast<size_t>(std::numeric_limits<int>::max()) ||
        json.find('\0') != std::string::npos)
        throw std::runtime_error("alignment evidence: invalid JSON input length/NUL");
    sqlite3* raw = nullptr;
    const int opened = sqlite3_open(":memory:", &raw);
    std::unique_ptr<sqlite3, decltype(&sqlite3_close)> db(raw, sqlite3_close);
    if (opened != SQLITE_OK)
        throw std::runtime_error("alignment evidence: cannot open memory JSON parser");
    using Statement = std::unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize)>;
    auto fail = [&]() -> void {
        throw std::runtime_error(std::string("alignment evidence JSON: ") + sqlite3_errmsg(db.get()));
    };
    auto prepare = [&](const char* sql) -> Statement {
        sqlite3_stmt* stmt = nullptr;
        const int rc = sqlite3_prepare_v2(db.get(), sql, -1, &stmt, nullptr);
        Statement result(stmt, sqlite3_finalize);
        if (rc != SQLITE_OK) fail();
        return result;
    };
    auto bind = [&](sqlite3_stmt* stmt, const std::string& value) {
        if (sqlite3_bind_text(stmt, 1, value.data(), static_cast<int>(value.size()), SQLITE_TRANSIENT) != SQLITE_OK) fail();
    };
    auto text = [](sqlite3_stmt* stmt, int column) {
        const auto* value = sqlite3_column_text(stmt, column);
        const int size = sqlite3_column_bytes(stmt, column);
        if (!value) throw std::runtime_error("alignment evidence: missing text field");
        return std::string(reinterpret_cast<const char*>(value), static_cast<size_t>(size));
    };
    auto valid = prepare("SELECT json_valid(?1)");
    bind(valid.get(), json);
    if (sqlite3_step(valid.get()) != SQLITE_ROW || sqlite3_column_int(valid.get(), 0) != 1)
        throw std::runtime_error("alignment evidence: malformed JSON");
    auto shape = prepare("SELECT json_type(?1), json_type(?1,'$.pairs'), "
                         "(SELECT count(*) FROM json_each(?1) WHERE key='pairs')");
    bind(shape.get(), json);
    if (sqlite3_step(shape.get()) != SQLITE_ROW || text(shape.get(), 0) != "object" ||
        sqlite3_column_type(shape.get(), 1) != SQLITE_TEXT || text(shape.get(), 1) != "object" ||
        sqlite3_column_int(shape.get(), 2) != 1)
        throw std::runtime_error("alignment evidence: expected one pairs object");
    auto heads = prepare("SELECT key,value,type FROM json_each(?1,'$.pairs') ORDER BY id");
    auto rows = prepare("SELECT r.type, json_extract(r.value,'$.eng'), json_extract(r.value,'$.n'), "
                        "json_type(r.value,'$.eng'), json_type(r.value,'$.n'), "
                        "(SELECT count(*) FROM json_each(r.value) WHERE key='eng'), "
                        "(SELECT count(*) FROM json_each(r.value) WHERE key='n') "
                        "FROM json_each(?1) AS r ORDER BY CAST(r.key AS INTEGER)");
    bind(heads.get(), json);
    std::vector<EvidenceHead> result;
    std::unordered_set<std::string> seen;
    int rc;
    while ((rc = sqlite3_step(heads.get())) == SQLITE_ROW) {
        if (text(heads.get(), 2) != "array")
            throw std::runtime_error("alignment evidence: headword value is not an array");
        EvidenceHead head{text(heads.get(), 0), {}};
        if (!seen.insert(head.wylie).second)
            throw std::runtime_error("alignment evidence: duplicate headword");
        bind(rows.get(), text(heads.get(), 1));
        int rr;
        while ((rr = sqlite3_step(rows.get())) == SQLITE_ROW) {
            if (text(rows.get(), 0) != "object" ||
                sqlite3_column_type(rows.get(), 3) != SQLITE_TEXT || text(rows.get(), 3) != "text" ||
                sqlite3_column_type(rows.get(), 4) != SQLITE_TEXT || text(rows.get(), 4) != "integer" ||
                sqlite3_column_type(rows.get(), 2) != SQLITE_INTEGER ||
                sqlite3_column_int(rows.get(), 5) != 1 || sqlite3_column_int(rows.get(), 6) != 1)
                throw std::runtime_error("alignment evidence: expected unique string eng and integer n");
            const sqlite3_int64 n = sqlite3_column_int64(rows.get(), 2);
            if (n < 0 || n > std::numeric_limits<int>::max())
                throw std::runtime_error("alignment evidence: n outside supported integer range");
            head.renderings.emplace_back(text(rows.get(), 1), static_cast<int>(n));
        }
        if (rr != SQLITE_DONE) fail();
        if (sqlite3_reset(rows.get()) != SQLITE_OK || sqlite3_clear_bindings(rows.get()) != SQLITE_OK) fail();
        result.push_back(std::move(head));
    }
    if (rc != SQLITE_DONE) fail();
    return result;
}
}  // namespace drillpack
