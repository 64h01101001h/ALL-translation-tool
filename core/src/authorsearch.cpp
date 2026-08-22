#include "allcore/authorsearch.h"

#include <algorithm>
#include <cctype>

#include "allcore/engines.h"
#include "allcore/unicode_wylie.h"

namespace allcore {
namespace {

bool hasAsciiUpper(const std::string& s) {
    for (unsigned char c : s)
        if (c >= 'A' && c <= 'Z') return true;
    return false;
}

bool hasTibetanUnicode(const std::string& s) {
    // U+0F00..U+0FFF encodes as EF B8..  no — three bytes starting 0xE0
    // 0xBC/0xBD. Cheap and sufficient: any 0xE0 lead byte followed by
    // 0xBC or 0xBD is Tibetan.
    for (size_t i = 0; i + 1 < s.size(); ++i)
        if ((unsigned char)s[i] == 0xE0) {
            unsigned char b = (unsigned char)s[i + 1];
            if (b == 0xBC || b == 0xBD) return true;
        }
    return false;
}

// Strip a trailing "(1357-1419)" style life-dates parenthetical. The
// banked per-text author strings carry them; a person typing the name
// will not.
std::string stripDates(const std::string& s) {
    const size_t open = s.rfind('(');
    if (open == std::string::npos) return s;
    if (s.find(')', open) == std::string::npos) return s;
    bool digity = false;
    for (size_t i = open + 1; i < s.size() && s[i] != ')'; ++i)
        if (std::isdigit((unsigned char)s[i])) digity = true;
    if (!digity) return s;
    return s.substr(0, open);
}

std::string collapseSpaces(const std::string& s) {
    std::string out;
    bool sp = true;  // leading spaces dropped
    for (char c : s) {
        const bool isSp = (c == ' ' || c == '\t' || c == '\n' || c == '\r');
        if (isSp) {
            if (!sp) out.push_back(' ');
            sp = true;
        } else {
            out.push_back(c);
            sp = false;
        }
    }
    while (!out.empty() && out.back() == ' ') out.pop_back();
    return out;
}

std::string despace(const std::string& s) {
    std::string out;
    for (char c : s)
        if (!std::isspace((unsigned char)c)) out.push_back(c);
    return out;
}

std::string lower(std::string s) {
    for (auto& c : s) c = (char)std::tolower((unsigned char)c);
    return s;
}

// Levenshtein with an early bail: anything past `cap` is reported as
// cap+1, so a long non-match costs little.
size_t editDistance(const std::string& a, const std::string& b, size_t cap) {
    const size_t n = a.size(), m = b.size();
    if (n > m + cap || m > n + cap) return cap + 1;
    std::vector<size_t> prev(m + 1), cur(m + 1);
    for (size_t j = 0; j <= m; ++j) prev[j] = j;
    for (size_t i = 1; i <= n; ++i) {
        cur[0] = i;
        size_t rowBest = cur[0];
        for (size_t j = 1; j <= m; ++j) {
            cur[j] = std::min({prev[j] + 1, cur[j - 1] + 1,
                               prev[j - 1] + (a[i - 1] == b[j - 1] ? 0 : 1)});
            rowBest = std::min(rowBest, cur[j]);
        }
        if (rowBest > cap) return cap + 1;
        prev.swap(cur);
    }
    return prev[m];
}

}  // namespace

std::string authorCanonical(const std::string& name) {
    std::string s = collapseSpaces(stripDates(name));
    if (s.empty()) return s;
    if (hasTibetanUnicode(s)) s = unicodeToWylie(s).wylie;
    // §8 duality: ACIP is defined uppercase, wylie lowercase.
    else if (hasAsciiUpper(s)) s = acipToEwts(s);
    return collapseSpaces(lower(s));
}

std::string authorPhoneticFold(const std::string& name) {
    const std::string ewts = authorCanonical(name);
    if (ewts.empty()) return {};
    std::string p = lower(pronounce(ewts));
    std::string out;
    for (char c : p)
        if (!std::isspace((unsigned char)c) && c != '-') out.push_back(c);
    return out;
}

std::vector<AuthorHit> matchAuthors(
    const std::string& query, const std::vector<std::string>& authors) {
    std::vector<AuthorHit> hits;
    const std::string q = authorCanonical(query);
    if (q.empty()) return hits;

    // Fold the query BOTH ways. Someone typing "tsongkapa" is spelling
    // a sound, not wylie; someone typing "tsong kha pa" is spelling
    // wylie. Running the first through pronounce() as if it were the
    // second produces nonsense, so both are offered and either counts.
    std::vector<std::string> qFolds;
    {
        std::string raw;
        for (char c : lower(collapseSpaces(stripDates(query))))
            if (std::isalnum((unsigned char)c)) raw.push_back(c);
        const std::string pron = authorPhoneticFold(query);
        // pronounce() is a WYLIE engine. Fed an English phonetic
        // spelling it does not fail loudly, it returns a stub:
        // "sakya pandita" came back "sapen", which then substring-
        // matched an unrelated Shing-za Pandita. A pronunciation that
        // collapsed to under half the typed length is that stub, not a
        // reading, so it is dropped rather than trusted.
        const bool pronUsable =
            pron.size() >= kMinPhoneticFold && pron.size() * 2 >= raw.size();
        for (const std::string& f :
             {raw, pronUsable ? pron : std::string()})
            if (f.size() >= kMinPhoneticFold &&
                std::find(qFolds.begin(), qFolds.end(), f) == qFolds.end())
                qFolds.push_back(f);
    }

    for (const auto& a : authors) {
        const std::string ac = authorCanonical(a);
        if (ac.empty()) continue;
        if (ac == q) {
            hits.push_back({a, AuthorTier::Exact, "exact name match"});
            continue;
        }
        if (q.size() >= kMinContainsLen &&
            ac.find(q) != std::string::npos) {
            hits.push_back(
                {a, AuthorTier::Contains, "contains \"" + q + "\""});
            continue;
        }
        // Spacing: same letters, different syllable breaks. Purely
        // orthographic, so it outranks anything phonetic.
        const std::string qd = despace(q), ad = despace(ac);
        if (qd.size() >= kMinContainsLen &&
            (ad == qd || ad.find(qd) != std::string::npos)) {
            hits.push_back({a, AuthorTier::Spacing,
                            "same letters, different syllable spacing"});
            continue;
        }
        if (qFolds.empty()) continue;
        const std::string ap = authorPhoneticFold(a);
        if (ap.size() < kMinPhoneticFold) continue;

        bool placed = false;
        for (const auto& qf : qFolds) {
            if (ap == qf || ap.find(qf) != std::string::npos) {
                hits.push_back({a, AuthorTier::Phonetic,
                                "sounds like \"" + qf + "\" (" + ap + ")"});
                placed = true;
                break;
            }
        }
        if (placed) continue;

        // Bounded near-match: allow roughly one slip per six characters,
        // measured against the SHORTER fold so a long name cannot buy
        // itself a large budget. Never a free-for-all.
        size_t best = std::string::npos;
        std::string bestQf;
        for (const auto& qf : qFolds) {
            const size_t budget =
                std::max<size_t>(1, std::min(qf.size(), ap.size()) / 6);
            const size_t d = editDistance(qf, ap, budget);
            if (d <= budget && (best == std::string::npos || d < best)) {
                best = d;
                bestQf = qf;
            }
        }
        if (best != std::string::npos)
            hits.push_back({a, AuthorTier::PhoneticNear,
                            "sounds close to \"" + bestQf + "\" (" + ap +
                                ", " + std::to_string(best) +
                                " sound(s) different)"});
    }

    std::stable_sort(hits.begin(), hits.end(),
                     [](const AuthorHit& x, const AuthorHit& y) {
                         if (x.tier != y.tier) return x.tier < y.tier;
                         return x.author < y.author;
                     });
    return hits;
}

}  // namespace allcore
