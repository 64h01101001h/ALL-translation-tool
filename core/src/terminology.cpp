#include "allcore/terminology.h"

#include <algorithm>
#include <cctype>
#include <map>
#include <set>

#include "allcore/particles.h"

namespace allcore {

namespace {

std::string lowered(const std::string& s) {
    std::string out = s;
    for (auto& c : out) c = (char)std::tolower((unsigned char)c);
    return out;
}

std::string stripped(const std::string& s) {
    size_t b = 0, e = s.size();
    while (b < e && std::isspace((unsigned char)s[b])) ++b;
    while (e > b && std::isspace((unsigned char)s[e - 1])) --e;
    return s.substr(b, e - b);
}

// gloss → matchable alternatives: split on '/', strip parentheticals
std::vector<std::string> glossAlternatives(const std::string& gloss) {
    std::string noParens;
    int depth = 0;
    for (char c : gloss) {
        if (c == '(' || c == '[') ++depth;
        else if (c == ')' || c == ']') { if (depth > 0) --depth; }
        else if (depth == 0) noParens += c;
    }
    std::vector<std::string> out;
    std::string cur;
    for (char c : noParens + "/") {
        if (c == '/') {
            std::string alt = stripped(lowered(cur));
            if (alt.size() >= 3) out.push_back(std::move(alt));
            cur.clear();
        } else {
            cur += c;
        }
    }
    return out;
}

}  // namespace

bool glossMatches(const std::string& gloss, const std::string& draft_lower) {
    for (const auto& alt : glossAlternatives(gloss))
        if (draft_lower.find(alt) != std::string::npos) return true;
    return false;
}

TermReport checkTerminology(const Spine& spine, const HeadwordIndex& index,
                            const std::string& source_acip,
                            const std::string& english_draft) {
    TermReport rep;
    auto doc = buildOverlay(spine, index, source_acip);
    const std::string draft = lowered(english_draft);

    // terms = maximal GLOSSED spans: spans with binding glosses that are not
    // contained in another glossed span (an unglossed container like a
    // clitic-matched "sangs rgyas kyi" must not swallow "sangs rgyas");
    // freestanding particles are grammar, not terminology
    std::map<long long, TermUse> byEntry;
    for (const auto& s : doc.spans) {
        const auto& e = doc.entries[s.entry_ix];
        if (e.hgm_gloss.empty()) continue;
        if (s.end - s.beg == 1) {
            std::string up = e.wylie;
            for (auto& c : up)
                if (c >= 'a' && c <= 'z') c = (char)(c - 'a' + 'A');
            if (classifyParticle(up)) continue;
        }
        bool containedInGlossed = false;
        for (const auto& t : doc.spans) {
            if (&t == &s) continue;
            if (doc.entries[t.entry_ix].hgm_gloss.empty()) continue;
            if (t.beg <= s.beg && s.end <= t.end &&
                (t.end - t.beg) > (s.end - s.beg)) {
                containedInGlossed = true;
                break;
            }
        }
        if (containedInGlossed) continue;
        auto it = byEntry.find(e.id);
        if (it != byEntry.end()) {
            ++it->second.occurrences;
            continue;
        }
        TermUse u;
        u.entry_id = e.id;
        u.wylie = e.wylie;
        u.tier = e.tier;
        u.provisional = e.provisional();
        u.occurrences = 1;
        u.glosses = e.hgm_gloss;
        for (const auto& g : e.hgm_gloss)
            if (glossMatches(g, draft)) u.matched.push_back(g);
        byEntry[e.id] = std::move(u);
    }

    for (auto& [id, u] : byEntry) rep.terms.push_back(std::move(u));
    std::sort(rep.terms.begin(), rep.terms.end(),
              [](const TermUse& a, const TermUse& b) {
                  const bool ua = a.matched.empty(), ub = b.matched.empty();
                  if (ua != ub) return ua > ub;      // unmatched first
                  if (a.occurrences != b.occurrences)
                      return a.occurrences > b.occurrences;
                  return a.wylie < b.wylie;
              });

    // shared English: the same matched alternative serving 2+ distinct terms
    std::map<std::string, std::set<std::string>> byAlt;
    for (const auto& u : rep.terms)
        for (const auto& g : u.matched)
            for (const auto& alt : glossAlternatives(g))
                if (draft.find(alt) != std::string::npos)
                    byAlt[alt].insert(u.wylie);
    for (auto& [alt, terms] : byAlt) {
        if (terms.size() < 2) continue;
        TermReport::SharedEnglish sh;
        sh.english = alt;
        sh.term_wylies.assign(terms.begin(), terms.end());
        rep.shared.push_back(std::move(sh));
    }
    return rep;
}

}  // namespace allcore
