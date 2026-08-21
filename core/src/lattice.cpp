#include "allcore/engines.h"
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

namespace {
// nesting depth in O(n log n): spans arrive sorted (beg asc, longer
// first), so every EARLIER span already has beg <= s.beg. A span
// contains s iff additionally end >= s.end and it is strictly longer;
// with begs sorted, any earlier span with end >= s.end is strictly
// longer unless it is the identical (beg, end) — identical duplicates
// are adjacent in the sort and subtracted. Count of earlier ends
// >= s.end via a Fenwick tree over end positions. The old pairwise
// scan was quadratic and took minutes on canon-sized files.
void computeDepths(std::vector<OverlaySpan>& spans, int n_tokens) {
    if (spans.empty()) return;
    std::vector<int> fen(n_tokens + 2, 0);
    auto add = [&](int pos) {
        for (int x = pos + 1; x <= n_tokens + 1; x += x & -x) fen[x] += 1;
    };
    auto prefix = [&](int pos) {   // count of inserted ends <= pos
        int r = 0;
        for (int x = pos + 1; x > 0; x -= x & -x) r += fen[x];
        return r;
    };
    int inserted = 0;
    size_t runStart = 0;
    for (size_t i = 0; i < spans.size(); ++i) {
        auto& s = spans[i];
        if (i > 0 && !(spans[i - 1].beg == s.beg &&
                       spans[i - 1].end == s.end))
            runStart = i;
        const int endsGE = inserted - prefix(s.end - 1);
        const int dupPriors = (int)(i - runStart);
        s.depth = endsGE - dupPriors;
        add(s.end);
        ++inserted;
    }
}
}  // namespace

namespace {
// matching happens in wylie; ACIP tokens (defined uppercase) convert
// through the proven engine, wylie tokens pass through untouched
std::vector<std::string> normTokens(const std::vector<std::string>& toks) {
    std::vector<std::string> norm;
    norm.reserve(toks.size());
    for (const auto& t : toks) {
        norm.push_back(tokenToEwts(t));
    }
    return norm;
}
}  // namespace

OverlayDoc buildOverlay(const Spine& spine, const std::string& acip_document,
                        int max_phrase_syllables) {
    OverlayDoc doc;
    tokenizeDocument(acip_document, doc.tokens, doc.barrier_after);
    const auto norm = normTokens(doc.tokens);
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
                cand += norm[t];
            }
            auto hits = spine.lookup(cand);
            if (!hits.empty()) {
                doc.spans.push_back({i, i + len, internEntry(hits.front()), 0, ""});
                continue;
            }
            // fused-ending split on the window's LAST syllable (Wilson particle
            // layer): PA'I → PA + 'i, PAS → PA + s, PAR → PA + r
            const std::string& last = norm[i + len - 1];
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
    computeDepths(doc.spans, n);
    return doc;
}

HeadwordIndex::HeadwordIndex(const Spine& spine) {
    // keys in WYLIE (the canonical matching space): ACIP headwords go
    // through the proven converter once at build time, so documents in
    // either script match after the same per-token normalization
    for (auto& [id, acip] : spine.allAcipHeadwords()) {
        Cand c;
        c.entry_id = id;
        std::istringstream in(acipToEwts(acip));
        std::string t;
        while (in >> t) c.tokens.push_back(t);
        if (c.tokens.empty()) continue;
        if (c.tokens.size() == 1) {
            single_.emplace(c.tokens[0], id);   // first wins on dups
        } else {
            multi_[c.tokens[0]][c.tokens[1]].push_back(std::move(c));
        }
        ++n_;
    }
}

const long long* HeadwordIndex::single(const std::string& tok) const {
    auto it = single_.find(tok);
    return it == single_.end() ? nullptr : &it->second;
}

const std::vector<HeadwordIndex::Cand>* HeadwordIndex::pair(
    const std::string& t1, const std::string& t2) const {
    auto i1 = multi_.find(t1);
    if (i1 == multi_.end()) return nullptr;
    auto i2 = i1->second.find(t2);
    return i2 == i1->second.end() ? nullptr : &i2->second;
}

OverlayDoc buildOverlay(const Spine& spine, const HeadwordIndex& index,
                        const std::string& acip_document,
                        int max_phrase_syllables) {
    OverlayDoc doc;
    tokenizeDocument(acip_document, doc.tokens, doc.barrier_after);
    const auto norm = normTokens(doc.tokens);
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
        // per length: first exact candidate wins; fused-ending match only used
        // when no exact exists at that (i, len) — mirrors the SQL path exactly
        struct Best {
            long long exact = 0;
            long long clit = 0;
            std::string clitic;
        };
        std::map<int, Best> best;
        // singles: exact by hash, fused via the stripped base
        if (const auto* id = index.single(norm[i]))
            best[1].exact = *id;
        else if (auto split = splitFusedEnding(norm[i])) {
            if (const auto* bid = index.single(split->base)) {
                best[1].clit = *bid;
                best[1].clitic = split->ending;
            }
        }
        // phrases: candidates share their first TWO tokens with the
        // document (only the last token may differ, by a fused ending)
        auto scanPair = [&](const std::string& t2, bool t2isBase) {
            const auto* cands = index.pair(norm[i], t2);
            if (!cands || i + 1 >= n) return;
            for (const auto& cand : *cands) {
                const int len = (int)cand.tokens.size();
                if (len > max_phrase_syllables || i + len > n) continue;
                if (t2isBase && len != 2) continue;
                bool barrier = false;
                for (int t = i; t < i + len - 1; ++t)
                    barrier |= doc.barrier_after[t];
                if (barrier) continue;
                bool okPrefix = true;
                for (int k = 2; k + 1 < len && okPrefix; ++k)
                    okPrefix = (norm[i + k] == cand.tokens[k]);
                if (!okPrefix) continue;
                const std::string& dl = norm[i + len - 1];
                const std::string& cl = cand.tokens[len - 1];
                Best& b = best[len];
                if (dl == cl) {
                    if (!b.exact) b.exact = cand.entry_id;
                    continue;
                }
                if (!b.clit && dl.size() > cl.size() &&
                    dl.compare(0, cl.size(), cl) == 0) {
                    if (auto ending =
                            fusedEndingIf(cl, dl.substr(cl.size()))) {
                        b.clit = cand.entry_id;
                        b.clitic = *ending;
                    }
                }
            }
        };
        if (i + 1 < n && !doc.barrier_after[i]) {
            scanPair(norm[i + 1], false);
            // a two-token phrase whose SECOND token carries the fused
            // ending is keyed under its bare form
            if (auto split2 = splitFusedEnding(norm[i + 1])) {
                const auto* cands = index.pair(norm[i], split2->base);
                if (cands) {
                    for (const auto& cand : *cands) {
                        if (cand.tokens.size() != 2) continue;
                        Best& b = best[2];
                        if (!b.exact && !b.clit) {
                            b.clit = cand.entry_id;
                            b.clitic = split2->ending;
                        }
                        break;
                    }
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
    computeDepths(doc.spans, n);
    return doc;
}

}  // namespace allcore
