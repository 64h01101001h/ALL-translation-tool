#include "allcore/refdict.h"

#include <sqlite3.h>

#include <stdexcept>

namespace allcore {

RefDict::RefDict(const std::string& db_path) {
    if (sqlite3_open_v2(db_path.c_str(), &db_, SQLITE_OPEN_READONLY, nullptr) !=
        SQLITE_OK)
        throw std::runtime_error("cannot open reference db: " + db_path);
}

RefDict::~RefDict() {
    if (db_) sqlite3_close(db_);
}

std::vector<RefEntry> RefDict::lookup(const std::string& wylie, int limit) const {
    std::vector<RefEntry> out;
    sqlite3_stmt* s = nullptr;
    sqlite3_prepare_v2(db_,
                       "SELECT layer, wylie, definition FROM ref_entries "
                       "WHERE wylie=? ORDER BY id LIMIT ?",
                       -1, &s, nullptr);
    sqlite3_bind_text(s, 1, wylie.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(s, 2, limit);
    while (sqlite3_step(s) == SQLITE_ROW) {
        auto txt = [&](int c) {
            const unsigned char* t = sqlite3_column_text(s, c);
            return t ? std::string(reinterpret_cast<const char*>(t)) : "";
        };
        out.push_back({txt(0), txt(1), txt(2)});
    }
    sqlite3_finalize(s);
    return out;
}

}  // namespace allcore
