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

namespace allcore {

std::string witnessSearchQuery(const std::string& citation) {
    // the span after "Tib: " (or "(Tib: ") up to a delimiter that ends
    // the title: a comma followed by a capitalized tag ("Tibetan
    // translation", "ACIP"), a closing paren, or end
    // The dominant shape (41 of the 50 missing works, measured):
    // the Tibetan title is the parenthesized span ENCLOSING the ACIP
    // number — "(Lam-ngan mun-sel sgron-ma, ACIP S05500-46)". Walk
    // back from "ACIP" to its opening paren. The Tib:-style spans
    // ("(Tib: dKon-mchog…, Tibetan translation at ACIP …)") resolve
    // through the same walk, with the "Tib: " prefix stripped.
    size_t p = std::string::npos, e = std::string::npos;
    const size_t acip = citation.find("ACIP ");
    if (acip != std::string::npos) {
        const size_t open = citation.rfind('(', acip);
        if (open != std::string::npos && acip - open < 400) {
            p = open + 1;
            e = acip;
            // trim the connective before "ACIP": ", " / ", at " /
            // ", Tibetan translation at "
            while (e > p && (citation[e - 1] == ' ')) --e;
            const size_t comma = citation.rfind(',', e);
            if (comma != std::string::npos && comma > p) e = comma;
        }
    }
    if (p == std::string::npos) {
        // fallback: the LAST "Tib:" span (the first is routinely the
        // AUTHOR's Tibetan name)
        size_t t = citation.rfind("Tib: ");
        if (t == std::string::npos) t = citation.rfind("Tib.: ");
        if (t == std::string::npos) return "";
        p = citation.find(' ', t);
        if (p == std::string::npos) return "";
        ++p;
        e = citation.size();
        for (const char* stop :
             {", Tibetan", ", ACIP", ")", ", at ACIP"}) {
            const size_t q = citation.find(stop, p);
            if (q != std::string::npos && q < e) e = q;
        }
    }
    std::string span = citation.substr(p, e - p);
    // strip a leading "Tib: " tag when the paren span carried one
    if (span.rfind("Tib: ", 0) == 0) span.erase(0, 5);
    // a span that swallowed a "Tibetan translation" tail keeps only
    // the title before it
    const size_t tt = span.find(", Tibetan");
    if (tt != std::string::npos) span.erase(tt);
    // clean to plain Wylie: hyphens between syllables become spaces,
    // diacritics fold via the filename sanitizer's rules, case lowered
    std::string out;
    for (size_t i = 0; i < span.size(); ++i) {
        unsigned char c = static_cast<unsigned char>(span[i]);
        if (c == '-') {
            out.push_back(' ');
        } else if (c == 0xE2 && i + 2 < span.size()) {
            // typographic apostrophe U+2019 -> '
            out.push_back('\'');
            i += 2;
        } else if (c < 0x80) {
            out.push_back(static_cast<char>(
                std::tolower(static_cast<unsigned char>(c))));
        }
        // other non-ASCII bytes dropped, never guessed
    }
    // collapse whitespace
    std::string q;
    bool sp = true;
    for (char c : out) {
        if (c == ' ' || c == '\n' || c == '\r' || c == '\t') {
            if (!sp) { q.push_back(' '); sp = true; }
        } else {
            q.push_back(c);
            sp = false;
        }
    }
    while (!q.empty() && (q.back() == ' ' || q.back() == ','))
        q.pop_back();
    // a one-syllable query finds everything; refuse it
    int spaces = 0;
    for (char c : q)
        if (c == ' ') ++spaces;
    return spaces >= 1 ? q : "";
}

}  // namespace allcore
