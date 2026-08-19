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
        size_t p = i;
        while (p < text.size() && isUpper(text[p]) && p - i < 2) ++p;
        const size_t letters = p - i;
        size_t d = p;
        while (d < text.size() && isDigit(text[d]) && d - p < 5) ++d;
        const size_t digits = d - p;
        if (letters >= 1 && letters <= 2 && digits >= 3 && digits <= 5 &&
            (d == text.size() || !isDigit(text[d]))) {
            AcipCitation c;
            c.number = text.substr(i, d - i);
            c.offset = i;
            out.push_back(std::move(c));
        }
        pos = d > pos + cue.size() ? d : pos + cue.size();
    }
    return out;
}

std::pair<std::string, int> normalizeCatalogNumber(const std::string& num) {
    size_t i = 0;
    while (i < num.size() && isUpper(num[i])) ++i;
    if (i == 0 || i > 2) return {num, -1};
    size_t d = i;
    long v = 0;
    while (d < num.size() && isDigit(num[d])) {
        v = v * 10 + (num[d] - '0');
        ++d;
    }
    if (d == i || d - i > 5) return {num, -1};
    // the numeric part must end the number (a trailing letter suffix —
    // language/part codes like S06850E1 — is fine and ignored here only
    // if the caller stripped it; for cited numbers there is none)
    return {num.substr(0, i), static_cast<int>(v)};
}

std::set<std::pair<std::string, int>> collectLibraryNumbers(
    const std::string& root) {
    namespace fs = std::filesystem;
    std::set<std::pair<std::string, int>> have;
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
        size_t i = 0;
        while (i < name.size() && isUpper(name[i]) && i < 2) ++i;
        if (i == 0) continue;
        size_t d = i;
        long v = 0;
        while (d < name.size() && isDigit(name[d]) && d - i < 5) {
            v = v * 10 + (name[d] - '0');
            ++d;
        }
        if (d - i < 3) continue;
        have.insert({name.substr(0, i), static_cast<int>(v)});
    }
    return have;
}

AuditResult auditPresence(
    const std::vector<AcipCitation>& cited,
    const std::set<std::pair<std::string, int>>& have) {
    // distinct by normalized form; keep the first spelling + count
    std::map<std::pair<std::string, int>, AuditEntry> byNorm;
    for (const auto& c : cited) {
        const auto n = normalizeCatalogNumber(c.number);
        if (n.second < 0) continue;
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
