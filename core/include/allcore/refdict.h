// allcore/refdict.h — read-only access to the LOCAL reference dictionaries
// (build/reference.db: TibetanDictionary.dic + THL bundle extractions).
//
// LICENSING (Adam's decision 2026-08-06): these are unlicensed compilations
// for personal in-house lookup only. Callers MUST present entries as
// "reference only" and never export them into release data.
#pragma once

#include <string>
#include <vector>

struct sqlite3;

namespace allcore {

struct RefEntry {
    std::string layer;        // 'TD' or THL dict tag(s), '+'-joined
    std::string wylie;
    std::string definition;
};

class RefDict {
public:
    explicit RefDict(const std::string& db_path);   // throws on failure
    ~RefDict();
    RefDict(const RefDict&) = delete;
    RefDict& operator=(const RefDict&) = delete;

    // exact headword lookup, TD first then THL tags in stored order
    std::vector<RefEntry> lookup(const std::string& wylie, int limit = 40) const;

private:
    sqlite3* db_ = nullptr;
};

}  // namespace allcore
