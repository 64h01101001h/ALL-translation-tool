// catalog_audit.cpp — see allcore/catalog_audit.h. Measured 2026-08-19
// against the extracted bibliography banks + installed library: 347
// distinct cited numbers, 296 present, 51 missing (the battery re-runs
// this; a data update legitimately moves the numbers WITH the data).
#include "allcore/catalog_audit.h"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <map>

namespace allcore {
namespace {

bool isUpper(char c) { return c >= 'A' && c <= 'Z'; }
bool isDigit(char c) { return c >= '0' && c <= '9'; }

}  // namespace

namespace {

// prefix + unpadded digits + optional "-sub"; "" when not a number.
// No upper digit cap (Adam's ruling, 2026-08-19); minimum 3 digits so a
// stray "ACIP 12" never reads as a citation.
std::string normalizeAt(const std::string& s, size_t i, size_t* end_out) {
    size_t p = i;
    while (p < s.size() && isUpper(s[p]) && p - i < 2) ++p;
    const size_t letters = p - i;
    if (letters < 1 || letters > 2) return "";
    size_t d = p;
    while (d < s.size() && isDigit(s[d])) ++d;
    if (d - p < 3) return "";
    size_t z = p;
    while (z < d && s[z] == '0') ++z;
    std::string key = s.substr(i, letters);
    key += z == d ? "0" : s.substr(z, d - z);
    // dashed sub-number: S05002-1 is a distinct work
    if (d < s.size() && s[d] == '-' && d + 1 < s.size() &&
        isDigit(s[d + 1])) {
        size_t e = d + 1;
        while (e < s.size() && isDigit(s[e])) ++e;
        key += "-" + s.substr(d + 1, e - d - 1);
        d = e;
    }
    if (end_out) *end_out = d;
    return key;
}

}  // namespace

std::vector<AcipCitation> extractAcipCitations(const std::string& text) {
    std::vector<AcipCitation> out;
    const std::string cue = "ACIP";
    size_t pos = 0;
    while ((pos = text.find(cue, pos)) != std::string::npos) {
        size_t i = pos + cue.size();
        // whitespace (at least one) between the cue and the number
        size_t ws = i;
        while (i < text.size() &&
               (text[i] == ' ' || text[i] == '\n' || text[i] == '\r' ||
                text[i] == '\t'))
            ++i;
        if (i == ws) { pos += cue.size(); continue; }
        size_t end = i;
        const std::string key = normalizeAt(text, i, &end);
        if (!key.empty()) {
            AcipCitation c;
            c.number = text.substr(i, end - i);
            c.offset = i;
            out.push_back(std::move(c));
            pos = end;
        } else {
            pos += cue.size();
        }
    }
    return out;
}

std::string normalizeCatalogKey(const std::string& num) {
    size_t end = 0;
    const std::string key = normalizeAt(num, 0, &end);
    return key;
}

std::string baseCatalogKey(const std::string& num) {
    std::string key = normalizeCatalogKey(num);
    const size_t d = key.find('-');
    if (d != std::string::npos) key.erase(d);
    return key;
}

std::pair<std::string, int> normalizeCatalogNumber(const std::string& num) {
    const std::string key = normalizeCatalogKey(num);
    if (key.empty()) return {num, -1};
    size_t i = 0;
    while (i < key.size() && isUpper(key[i])) ++i;
    long v = 0;
    size_t d = i;
    while (d < key.size() && isDigit(key[d])) {
        v = v * 10 + (key[d] - '0');
        ++d;
    }
    return {key.substr(0, i), static_cast<int>(v)};
}

std::set<std::string> collectLibraryNumbers(const std::string& root) {
    namespace fs = std::filesystem;
    std::set<std::string> have;
    std::error_code ec;
    if (!fs::exists(root, ec)) return have;
    for (fs::recursive_directory_iterator it(
             root, fs::directory_options::skip_permission_denied, ec), end;
         it != end; it.increment(ec)) {
        if (ec) break;
        if (!it->is_regular_file(ec)) continue;
        std::string name = it->path().filename().string();
        for (char& c : name)
            c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
        const std::string key = baseCatalogKey(name);
        if (!key.empty()) have.insert(key);
    }
    return have;
}

AuditResult auditPresence(const std::vector<AcipCitation>& cited,
                          const std::set<std::string>& have) {
    // distinct by normalized form; keep the first spelling + count
    std::map<std::string, AuditEntry> byNorm;
    for (const auto& c : cited) {
        const auto n = baseCatalogKey(c.number);
        if (n.empty()) continue;
        auto& e = byNorm[n];
        if (e.citations == 0) {
            e.number = c.number;
            e.present = have.count(n) > 0;
        }
        ++e.citations;
    }
    AuditResult r;
    r.cited_distinct = static_cast<int>(byNorm.size());
    for (auto& [k, e] : byNorm) {
        if (e.present) ++r.present;
        else ++r.missing;
        r.entries.push_back(e);
    }
    std::sort(r.entries.begin(), r.entries.end(),
              [](const AuditEntry& a, const AuditEntry& b) {
                  if (a.present != b.present) return !a.present;
                  return a.number < b.number;
              });
    return r;
}

}  // namespace allcore
