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

// Does `needle` occur in `hay` as a WORD, rather than buried inside a longer
// one? The check was a bare find(), and measured on real English that is a
// false-positive machine: "mind" matched "reminded", "art" matched
// "departure", "one" matched "honest". Each of those reported a term as
// RENDERED when the equivalent never appears as a word at all — and the
// pane's headline verdict is a count of exactly that judgement.
// (Found 2026-09-11 by probing the shipped matcher, not by reading it.)
//
// The boundary class is alphanumeric, so an apostrophe or a hyphen still
// counts as an edge: "buddha" legitimately matches "buddha's", and "being"
// matches "well-being". Only a letter or digit on either side disqualifies.
static bool containsAsWords(const std::string& hay, const std::string& needle) {
    if (needle.empty()) return false;
    size_t at = hay.find(needle);
    while (at != std::string::npos) {
        const bool leftOk =
            at == 0 || !std::isalnum((unsigned char)hay[at - 1]);
        const size_t end = at + needle.size();
        const bool rightOk =
            end >= hay.size() || !std::isalnum((unsigned char)hay[end]);
        if (leftOk && rightOk) return true;
        at = hay.find(needle, at + 1);
    }
    return false;
}

bool glossMatches(const std::string& gloss, const std::string& draft_lower) {
    for (const auto& alt : glossAlternatives(gloss))
        if (containsAsWords(draft_lower, alt)) return true;
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
        // Maximality ranks by extent — but never across tiers. A longer
        // AUTO-ALIGNED span must not delete a curated or glossary term inside
        // it, because that term is his own English and this report exists to
        // check his terminology. Deleting it is worse than mislabelling it:
        // the row is never built, so no downstream label can rescue it, and
        // the verdict then reads as an unrendered term for a draft that used
        // his equivalent verbatim.
        //
        // Measured repro: source SANGS RGYAS CHOS, draft "the Buddha's
        // Dharma". Before this, the report held exactly one term —
        // `sangs rgyas chos`, auto-aligned, glossed "sangye chudang tsokyi
        // choknam", which is a pronunciation line — and neither `sangs rgyas`
        // ("Buddha", curated) nor `chos` ("Dharma (the teaching)", curated).
        //
        // Same defect as walk::bestGlossSpan, and the same fix: extent is the
        // tiebreak, tier is the requirement. Note the honorific advisory
        // already works around this by re-tokenising the source itself —
        // "the register keys on the honorific stem, which a longer matched
        // dictionary term can hide" — which was this bug, seen once and
        // routed around rather than fixed.
        bool containedInGlossed = false;
        for (const auto& t : doc.spans) {
            if (&t == &s) continue;
            const auto& te = doc.entries[t.entry_ix];
            if (te.hgm_gloss.empty()) continue;
            if (te.provisional() && !e.provisional()) continue;
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
