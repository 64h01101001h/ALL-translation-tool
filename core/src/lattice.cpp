#include "allcore/lattice.h"

#include <algorithm>
#include <map>
#include <sstream>

#include "allcore/particles.h"

namespace allcore {

void tokenizeDocument(const std::string& raw, std::vector<std::string>& tokens,
                      std::vector<bool>& barrier_after) {
    tokens.clear();
    barrier_after.clear();
    std::string cur;
    bool pending_barrier = false;
    auto flush = [&](bool barrier) {
        if (!cur.empty()) {
            tokens.push_back(cur);
            barrier_after.push_back(false);
            cur.clear();
            pending_barrier = false;
        }
        if (barrier && !tokens.empty()) barrier_after.back() = true;
        else if (barrier) pending_barrier = true;   // leading punctuation: ignore
    };
    for (size_t i = 0; i < raw.size();) {
        // curly apostrophes → '
        if (i + 2 < raw.size() && (unsigned char)raw[i] == 0xE2 &&
            (unsigned char)raw[i + 1] == 0x80 &&
            ((unsigned char)raw[i + 2] == 0x98 || (unsigned char)raw[i + 2] == 0x99)) {
            cur += '\'';
            i += 3;
            continue;
        }
        char c = raw[i++];
        if (c == '[') {                              // folio marker
            while (i < raw.size() && raw[i - 1] != ']') ++i;
            flush(false);
            continue;
        }
        if (c == ',' || c == ';' || c == '/') { flush(true); continue; }
        if (c == ' ' || c == '\t') { flush(false); continue; }
        if (c == '\n' || c == '\r') { flush(true); continue; }
        cur += c;
    }
    flush(false);
    (void)pending_barrier;
}

std::vector<int> OverlayDoc::spansAt(int tok) const {
    std::vector<int> ix;
    for (int i = 0; i < (int)spans.size(); ++i)
        if (spans[i].beg <= tok && tok < spans[i].end) ix.push_back(i);
    std::sort(ix.begin(), ix.end(), [this](int a, int b) {
        int la = spans[a].end - spans[a].beg, lb = spans[b].end - spans[b].beg;
        if (la != lb) return la < lb;                // innermost (shortest) first
        return spans[a].beg > spans[b].beg;
    });
    return ix;
}

std::vector<int> OverlayDoc::coverDepth(int cap) const {
    std::vector<int> d(tokens.size(), 0);
    for (const auto& s : spans)
        for (int t = s.beg; t < s.end; ++t)
            if (d[t] < cap) ++d[t];
    return d;
}

OverlayDoc buildOverlay(const Spine& spine, const std::string& acip_document,
                        int max_phrase_syllables) {
    OverlayDoc doc;
    tokenizeDocument(acip_document, doc.tokens, doc.barrier_after);
    const int n = (int)doc.tokens.size();
    std::map<long long, int> entry_ix;

    auto barrierWithin = [&](int beg, int end) {   // barrier strictly inside span
        for (int t = beg; t < end - 1; ++t)
            if (doc.barrier_after[t]) return true;
        return false;
    };

    auto internEntry = [&](const Entry& e) {
        auto it = entry_ix.find(e.id);
        if (it != entry_ix.end()) return it->second;
        int ix = (int)doc.entries.size();
        doc.entries.push_back(e);
        entry_ix[e.id] = ix;
        return ix;
    };

    for (int i = 0; i < n; ++i) {
        for (int len = std::min(max_phrase_syllables, n - i); len >= 1; --len) {
            if (barrierWithin(i, i + len)) continue;
            std::string cand;
            for (int t = i; t < i + len; ++t) {
                if (t > i) cand += ' ';
                cand += doc.tokens[t];
            }
            auto hits = spine.lookup(cand);
            if (!hits.empty()) {
                doc.spans.push_back({i, i + len, internEntry(hits.front()), 0, ""});
                continue;
            }
            // fused-ending split on the window's LAST syllable (Wilson particle
            // layer): PA'I → PA + 'i, PAS → PA + s, PAR → PA + r
            const std::string& last = doc.tokens[i + len - 1];
            if (auto split = splitFusedEnding(last)) {
                std::string cand2 =
                    cand.substr(0, cand.size() - (last.size() - split->base.size()));
                auto hits2 = spine.lookup(cand2);
                if (!hits2.empty())
                    doc.spans.push_back(
                        {i, i + len, internEntry(hits2.front()), 0, split->ending});
            }
        }
    }

    std::sort(doc.spans.begin(), doc.spans.end(), [](const OverlaySpan& a,
                                                     const OverlaySpan& b) {
        if (a.beg != b.beg) return a.beg < b.beg;
        return (a.end - a.beg) > (b.end - b.beg);
    });
    // nesting depth = number of strictly containing spans
    for (auto& s : doc.spans) {
        s.depth = 0;
        for (const auto& t : doc.spans) {
            if (&t == &s) continue;
            const bool contains = t.beg <= s.beg && s.end <= t.end &&
                                  (t.end - t.beg) > (s.end - s.beg);
            if (contains) ++s.depth;
        }
    }
    return doc;
}

HeadwordIndex::HeadwordIndex(const Spine& spine) {
    for (auto& [id, acip] : spine.allAcipHeadwords()) {
        Cand c;
        c.entry_id = id;
        std::istringstream in(acip);
        std::string t;
        while (in >> t) c.tokens.push_back(t);
        if (c.tokens.empty()) continue;
        buckets_[c.tokens.front()].push_back(std::move(c));
        ++n_;
    }
}

const std::vector<HeadwordIndex::Cand>* HeadwordIndex::bucket(
    const std::string& first_token) const {
    auto it = buckets_.find(first_token);
    return it == buckets_.end() ? nullptr : &it->second;
}

OverlayDoc buildOverlay(const Spine& spine, const HeadwordIndex& index,
                        const std::string& acip_document,
                        int max_phrase_syllables) {
    OverlayDoc doc;
    tokenizeDocument(acip_document, doc.tokens, doc.barrier_after);
    const int n = (int)doc.tokens.size();
    std::map<long long, int> entry_ix;
    auto internEntry = [&](long long id) {
        auto it = entry_ix.find(id);
        if (it != entry_ix.end()) return it->second;
        int ix = (int)doc.entries.size();
        doc.entries.push_back(spine.entryById(id));
        entry_ix[id] = ix;
        return ix;
    };
    for (int i = 0; i < n; ++i) {
        const auto* bucket = index.bucket(doc.tokens[i]);
        // per length: first exact candidate wins; fused-ending match only used
        // when no exact exists at that (i, len) — mirrors the SQL path exactly
        struct Best {
            long long exact = 0;
            long long clit = 0;
            std::string clitic;
        };
        std::map<int, Best> best;
        static const std::vector<HeadwordIndex::Cand> kEmpty;
        for (const auto& cand : bucket ? *bucket : kEmpty) {
            const int len = (int)cand.tokens.size();
            if (len > max_phrase_syllables || i + len > n) continue;
            bool barrier = false;
            for (int t = i; t < i + len - 1; ++t) barrier |= doc.barrier_after[t];
            if (barrier) continue;
            bool okPrefix = true;
            for (int k = 0; k + 1 < len && okPrefix; ++k)
                okPrefix = (doc.tokens[i + k] == cand.tokens[k]);
            if (!okPrefix) continue;
            const std::string& dl = doc.tokens[i + len - 1];
            const std::string& cl = cand.tokens[len - 1];
            Best& b = best[len];
            if (dl == cl) {
                if (!b.exact) b.exact = cand.entry_id;
                continue;
            }
            if (!b.clit && dl.size() > cl.size() &&
                dl.compare(0, cl.size(), cl) == 0) {
                if (auto ending = fusedEndingIf(cl, dl.substr(cl.size()))) {
                    b.clit = cand.entry_id;
                    b.clitic = *ending;
                }
            }
        }
        // single-syllable fused-ending match lives in the STRIPPED base's
        // bucket (PA'I → candidates under "PA"), which the primary key never
        // scans
        if (auto split = splitFusedEnding(doc.tokens[i])) {
            if (const auto* bb = index.bucket(split->base)) {
                for (const auto& cand : *bb) {
                    if (cand.tokens.size() != 1 || cand.tokens[0] != split->base)
                        continue;
                    Best& b = best[1];
                    if (!b.clit && !b.exact) {
                        b.clit = cand.entry_id;
                        b.clitic = split->ending;
                    }
                    break;
                }
            }
        }
        for (auto& [len, b] : best) {
            if (b.exact)
                doc.spans.push_back({i, i + len, internEntry(b.exact), 0, ""});
            else if (b.clit)
                doc.spans.push_back(
                    {i, i + len, internEntry(b.clit), 0, b.clitic});
        }
    }

    std::sort(doc.spans.begin(), doc.spans.end(), [](const OverlaySpan& a,
                                                     const OverlaySpan& b) {
        if (a.beg != b.beg) return a.beg < b.beg;
        return (a.end - a.beg) > (b.end - b.beg);
    });
    for (auto& s : doc.spans) {
        s.depth = 0;
        for (const auto& t : doc.spans) {
            if (&t == &s) continue;
            const bool contains = t.beg <= s.beg && s.end <= t.end &&
                                  (t.end - t.beg) > (s.end - s.beg);
            if (contains) ++s.depth;
        }
    }
    return doc;
}

}  // namespace allcore
