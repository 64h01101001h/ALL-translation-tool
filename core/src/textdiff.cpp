// allcore/textdiff.cpp — see textdiff.h. Myers' O(ND) difference algorithm
// in its linear-space (bisecting) form, written from the 1986 paper; the
// diff3 walk follows the classic description. No product code.
#include "allcore/textdiff.h"

#include <algorithm>
#include <cctype>
#include <map>
#include <regex>
#include <sstream>
#include <unordered_map>

#include "allcore/engines.h"
#include "allcore/unicode_wylie.h"

namespace allcore {
namespace textdiff {

// ---------------------------------------------------------------- lines
std::vector<std::string> splitLines(const std::string& t) {
    std::vector<std::string> v;
    std::string cur;
    for (size_t i = 0; i < t.size(); ++i) {
        const char c = t[i];
        if (c == '\r') {
            v.push_back(cur); cur.clear();
            if (i + 1 < t.size() && t[i + 1] == '\n') ++i;
        } else if (c == '\n') {
            v.push_back(cur); cur.clear();
        } else cur += c;
    }
    if (!cur.empty()) v.push_back(cur);
    return v;
}
std::string joinLines(const std::vector<std::string>& lines, const std::string& eol) {
    std::string out;
    for (size_t i = 0; i < lines.size(); ++i) { out += lines[i]; out += eol; }
    return out;
}

// ---------------------------------------------------------------- utf-8
static std::vector<std::pair<int, unsigned>> codepoints(const std::string& s) {
    // (byte offset, code point) per character; malformed bytes pass as-is
    std::vector<std::pair<int, unsigned>> v;
    for (size_t i = 0; i < s.size();) {
        const unsigned char c = (unsigned char)s[i];
        unsigned cp = c; size_t len = 1;
        if (c >= 0xF0 && i + 3 < s.size() + 0) { cp = ((c & 7u) << 18) | (((unsigned char)s[i+1] & 0x3Fu) << 12) | (((unsigned char)s[i+2] & 0x3Fu) << 6) | ((unsigned char)s[i+3] & 0x3Fu); len = 4; }
        else if (c >= 0xE0 && i + 2 < s.size()) { cp = ((c & 0xFu) << 12) | (((unsigned char)s[i+1] & 0x3Fu) << 6) | ((unsigned char)s[i+2] & 0x3Fu); len = 3; }
        else if (c >= 0xC0 && i + 1 < s.size()) { cp = ((c & 0x1Fu) << 6) | ((unsigned char)s[i+1] & 0x3Fu); len = 2; }
        v.push_back({(int)i, cp});
        i += len;
    }
    return v;
}
static bool isTibetanPunctCp(unsigned cp) {
    return (cp >= 0x0F01 && cp <= 0x0F14) || (cp >= 0x0F3A && cp <= 0x0F3D) || cp == 0x0F85 || (cp >= 0x0FD0 && cp <= 0x0FD4);
}
static bool hasTibetanUnicode(const std::string& s) {
    for (size_t i = 0; i + 2 < s.size(); ++i)
        if ((unsigned char)s[i] == 0xE0 && ((unsigned char)s[i+1] & 0xFC) == 0xBC) return true;
    return false;
}
bool looksTibetan(const std::string& s) {
    if (hasTibetanUnicode(s)) return true;
    // ACIP: mostly uppercase ASCII letters; Wylie: the engine's judgement
    int up = 0, low = 0;
    for (unsigned char c : s) { if (c >= 'A' && c <= 'Z') ++up; else if (c >= 'a' && c <= 'z') ++low; }
    if (up > 0 && low == 0) return true;
    return looksLikeWylie(s);
}

// ---------------------------------------------------------- normalise
static std::string collapseWs(const std::string& s) {
    std::string o; bool ws = false;
    for (unsigned char c : s) {
        if (std::isspace(c)) { ws = true; continue; }
        if (ws && !o.empty()) o += ' ';
        ws = false; o += (char)c;
    }
    return o;
}
static std::string toEwts(const std::string& line, bool* ok) {
    if (hasTibetanUnicode(line)) {
        const auto r = unicodeToWylie(line);
        if (r.warns > 0) { if (ok) *ok = false; return line; }
        return r.wylie;
    }
    bool upper = false, lower = false;
    for (unsigned char c : line) { if (c >= 'A' && c <= 'Z') upper = true; else if (c >= 'a' && c <= 'z') lower = true; }
    if (upper && !lower) return acipToEwts(line);
    return line;   // Wylie or English: already the common form
}
std::string normalizeLine(const std::string& in, const Options& o, bool* normalisedOk) {
    if (normalisedOk) *normalisedOk = true;
    std::string s = in;
    for (const auto& sub : o.substitutions) {
        try { s = std::regex_replace(s, std::regex(sub.first), sub.second); } catch (...) {}
    }
    if (o.scriptAgnostic) { bool ok = true; s = toEwts(s, &ok); if (!ok && normalisedOk) *normalisedOk = false; }
    if (o.ignoreApparatus) {
        static const std::regex ap("\\[[^\\]]*\\]|\\{[^}]*\\}");
        s = std::regex_replace(s, ap, "");
    }
    if (o.ignoreFolioMarkers) {
        static const std::regex fol("@[0-9]{1,4}[AaBb]?(?![A-Za-z0-9])");
        s = std::regex_replace(s, fol, "");
    }
    if (o.ignoreApparatus || o.ignoreFolioMarkers) s = collapseWs(s);   // removals leave doubled spaces
    if (o.ignoreTibetanPunct) {
        std::string t;
        const auto cps = codepoints(s);
        for (size_t i = 0; i < cps.size(); ++i) {
            const unsigned cp = cps[i].second;
            const int b = cps[i].first, e = (i + 1 < cps.size()) ? cps[i+1].first : (int)s.size();
            if (cp == 0x0F0B || cp == 0x0F0C) { t += ' '; continue; }        // tsheg → space keeps syllables apart
            if (isTibetanPunctCp(cp)) continue;
            if (cp == ',' || cp == ';' || cp == '*' || cp == '#' || cp == '%' || cp == '/' || cp == '|' || cp == '!') continue;
            t.append(s, b, e - b);
        }
        s = collapseWs(t);
    }
    if (o.ignoreWhitespace) { std::string t; for (unsigned char c : s) if (!std::isspace(c)) t += (char)c; s = t; }
    else if (o.ignoreWhitespaceChange) s = collapseWs(s);
    if (o.ignoreCase) for (auto& c : s) c = (char)std::tolower((unsigned char)c);
    return s;
}
bool matchesIgnoreRegex(const std::string& line, const Options& o) {
    for (const auto& p : o.ignoreRegex) {
        try { if (std::regex_search(line, std::regex(p))) return true; } catch (...) {}
    }
    return false;
}

// ------------------------------------------------------------- myers
namespace {
struct Op { int kind; int aPos, bPos, len; };   // 0 equal, 1 delete (a only), 2 insert (b only)

struct Range { int aLo, aHi, bLo, bHi; };

// The bisection: find a point on a shortest edit path where the forward
// and reverse searches meet. Returns false when there is no common line.
bool bisect(const std::vector<int>& a, const std::vector<int>& b, const Range& r, int& sx, int& sy) {
    const int N = r.aHi - r.aLo, M = r.bHi - r.bLo;
    const int maxD = (N + M + 1) / 2;
    const int off = maxD + 1, vlen = 2 * off + 2;
    std::vector<int> v1(vlen, -1), v2(vlen, -1);
    v1[off + 1] = 0; v2[off + 1] = 0;
    const int delta = N - M;
    const bool front = (delta % 2) != 0;
    int k1s = 0, k1e = 0, k2s = 0, k2e = 0;
    for (int d = 0; d < maxD + 1; ++d) {
        for (int k1 = -d + k1s; k1 <= d - k1e; k1 += 2) {
            const int ko = off + k1;
            int x1 = (k1 == -d || (k1 != d && v1[ko - 1] < v1[ko + 1])) ? v1[ko + 1] : v1[ko - 1] + 1;
            int y1 = x1 - k1;
            while (x1 < N && y1 < M && a[r.aLo + x1] == b[r.bLo + y1]) { ++x1; ++y1; }
            v1[ko] = x1;
            if (x1 > N) k1e += 2;
            else if (y1 > M) k1s += 2;
            else if (front) {
                const int k2o = off + delta - k1;
                if (k2o >= 0 && k2o < vlen && v2[k2o] != -1) {
                    const int x2 = N - v2[k2o];
                    if (x1 >= x2) { sx = r.aLo + x1; sy = r.bLo + y1; return true; }
                }
            }
        }
        for (int k2 = -d + k2s; k2 <= d - k2e; k2 += 2) {
            const int ko = off + k2;
            int x2 = (k2 == -d || (k2 != d && v2[ko - 1] < v2[ko + 1])) ? v2[ko + 1] : v2[ko - 1] + 1;
            int y2 = x2 - k2;
            while (x2 < N && y2 < M && a[r.aHi - x2 - 1] == b[r.bHi - y2 - 1]) { ++x2; ++y2; }
            v2[ko] = x2;
            if (x2 > N) k2e += 2;
            else if (y2 > M) k2s += 2;
            else if (!front) {
                const int k1o = off + delta - k2;
                if (k1o >= 0 && k1o < vlen && v1[k1o] != -1) {
                    const int x1 = v1[k1o];
                    const int y1 = off + x1 - k1o;
                    const int x2r = N - x2;
                    if (x1 >= x2r) { sx = r.aLo + x1; sy = r.bLo + y1; return true; }
                }
            }
        }
    }
    return false;
}

std::vector<Op> myers(const std::vector<int>& a, const std::vector<int>& b) {
    std::vector<Op> ops;
    std::vector<Range> work{{0, (int)a.size(), 0, (int)b.size()}};
    while (!work.empty()) {
        Range r = work.back(); work.pop_back();
        // common prefix / suffix
        int p = 0;
        while (r.aLo + p < r.aHi && r.bLo + p < r.bHi && a[r.aLo + p] == b[r.bLo + p]) ++p;
        if (p) { ops.push_back({0, r.aLo, r.bLo, p}); r.aLo += p; r.bLo += p; }
        int q = 0;
        while (r.aHi - q > r.aLo && r.bHi - q > r.bLo && a[r.aHi - q - 1] == b[r.bHi - q - 1]) ++q;
        if (q) { ops.push_back({0, r.aHi - q, r.bHi - q, q}); r.aHi -= q; r.bHi -= q; }
        const int N = r.aHi - r.aLo, M = r.bHi - r.bLo;
        if (N == 0 && M == 0) continue;
        if (N == 0) { ops.push_back({2, r.aLo, r.bLo, M}); continue; }
        if (M == 0) { ops.push_back({1, r.aLo, r.bLo, N}); continue; }
        int sx = 0, sy = 0;
        if (!bisect(a, b, r, sx, sy)) {
            ops.push_back({1, r.aLo, r.bLo, N});
            ops.push_back({2, r.aLo, r.bLo, M});
            continue;
        }
        work.push_back({sx, r.aHi, sy, r.bHi});
        work.push_back({r.aLo, sx, r.bLo, sy});
    }
    std::sort(ops.begin(), ops.end(), [](const Op& x, const Op& y) {
        if (x.aPos != y.aPos) return x.aPos < y.aPos;
        if (x.bPos != y.bPos) return x.bPos < y.bPos;
        return x.kind < y.kind;   // deletes before inserts at the same point
    });
    return ops;
}

std::vector<int> idsFor(const std::vector<std::string>& keys, std::unordered_map<std::string, int>& table) {
    std::vector<int> v; v.reserve(keys.size());
    for (const auto& k : keys) {
        auto it = table.find(k);
        if (it == table.end()) it = table.emplace(k, (int)table.size()).first;
        v.push_back(it->second);
    }
    return v;
}
}  // namespace

Result diffLines(const std::vector<std::string>& a, const std::vector<std::string>& b, const Options& o) {
    Result R; R.aLines = (int)a.size(); R.bLines = (int)b.size();
    std::vector<std::string> ka(a.size()), kb(b.size());
    std::vector<char> okA(a.size(), 1), okB(b.size(), 1);
    for (size_t i = 0; i < a.size(); ++i) { bool ok = true; ka[i] = normalizeLine(a[i], o, &ok); okA[i] = ok; if (!ok) ++R.unnormalised; }
    for (size_t i = 0; i < b.size(); ++i) { bool ok = true; kb[i] = normalizeLine(b[i], o, &ok); okB[i] = ok; if (!ok) ++R.unnormalised; }
    std::unordered_map<std::string, int> table;
    const auto ia = idsFor(ka, table), ib = idsFor(kb, table);
    const auto ops = myers(ia, ib);

    // ops → hunks; a Delete immediately followed by an Insert is a Change
    std::vector<Hunk> hs;
    for (size_t i = 0; i < ops.size(); ++i) {
        const Op& op = ops[i];
        if (op.kind == 0) {
            // split equal runs into truly-equal and "minor" (raw differs)
            int s = 0;
            while (s < op.len) {
                const bool raw = a[op.aPos + s] == b[op.bPos + s];
                int e = s;
                while (e < op.len && (a[op.aPos + e] == b[op.bPos + e]) == raw) ++e;
                Hunk h; h.kind = raw ? Kind::Equal : Kind::Change; h.unimportant = !raw;
                h.aBeg = op.aPos + s; h.aEnd = op.aPos + e; h.bBeg = op.bPos + s; h.bEnd = op.bPos + e;
                hs.push_back(h);
                s = e;
            }
        } else if (op.kind == 1) {
            Hunk h; h.kind = Kind::Delete; h.aBeg = op.aPos; h.aEnd = op.aPos + op.len; h.bBeg = h.bEnd = op.bPos;
            if (i + 1 < ops.size() && ops[i + 1].kind == 2 && ops[i + 1].aPos == op.aPos + op.len && ops[i + 1].bPos == op.bPos) {
                h.kind = Kind::Change; h.bEnd = op.bPos + ops[i + 1].len; ++i;
            }
            hs.push_back(h);
        } else {
            Hunk h; h.kind = Kind::Insert; h.aBeg = h.aEnd = op.aPos; h.bBeg = op.bPos; h.bEnd = op.bPos + op.len;
            if (i + 1 < ops.size() && ops[i + 1].kind == 1 && ops[i + 1].aPos == op.aPos && ops[i + 1].bPos == op.bPos + op.len) {
                h.kind = Kind::Change; h.aEnd = op.aPos + ops[i + 1].len; ++i;
            }
            hs.push_back(h);
        }
    }
    // blank-line and regex rules → minor
    auto allBlank = [&](const std::vector<std::string>& keys, int lo, int hi) { for (int i = lo; i < hi; ++i) if (!keys[i].empty()) return false; return true; };
    auto allRegex = [&](const std::vector<std::string>& lines, int lo, int hi) { for (int i = lo; i < hi; ++i) if (!matchesIgnoreRegex(lines[i], o)) return false; return true; };
    for (auto& h : hs) {
        if (h.kind == Kind::Equal || h.unimportant) continue;
        if (o.ignoreBlankLines && allBlank(ka, h.aBeg, h.aEnd) && allBlank(kb, h.bBeg, h.bEnd)) h.unimportant = true;
        else if (!o.ignoreRegex.empty() && (h.aEnd > h.aBeg || h.bEnd > h.bBeg) && allRegex(a, h.aBeg, h.aEnd) && allRegex(b, h.bBeg, h.bEnd)) h.unimportant = true;
    }
    for (auto& h : hs) {
        for (int i = h.aBeg; i < h.aEnd; ++i) if (!okA[i]) h.rawNormalised = false;
        for (int i = h.bBeg; i < h.bEnd; ++i) if (!okB[i]) h.rawNormalised = false;
    }
    // moved blocks: a Delete whose key sequence equals an Insert's
    if (o.detectMoves) {
        std::map<std::string, std::vector<int>> dels;
        auto keyOf = [&](const std::vector<std::string>& keys, int lo, int hi) { std::string k; bool any = false; for (int i = lo; i < hi; ++i) { k += keys[i]; k += '\n'; if (!keys[i].empty()) any = true; } return any ? k : std::string(); };
        for (size_t i = 0; i < hs.size(); ++i) if (hs[i].kind == Kind::Delete && !hs[i].unimportant) { const auto k = keyOf(ka, hs[i].aBeg, hs[i].aEnd); if (!k.empty()) dels[k].push_back((int)i); }
        for (size_t i = 0; i < hs.size(); ++i) if (hs[i].kind == Kind::Insert && !hs[i].unimportant) {
            const auto k = keyOf(kb, hs[i].bBeg, hs[i].bEnd);
            auto it = dels.find(k);
            if (it == dels.end() || it->second.empty()) continue;
            const int j = it->second.front(); it->second.erase(it->second.begin());
            hs[i].movedPeer = j; hs[j].movedPeer = (int)i; ++R.moved;
        }
    }
    for (const auto& h : hs) {
        if (h.kind == Kind::Equal) continue;
        if (h.unimportant) { ++R.unimportant; continue; }
        if (h.kind == Kind::Change) ++R.changes; else if (h.kind == Kind::Insert) ++R.inserts; else ++R.deletes;
    }
    R.hunks = std::move(hs);
    return R;
}

// ------------------------------------------------------------ in-line
namespace {
struct Tok { int beg, end; std::string key; };
std::vector<Tok> tokenize(const std::string& s, Mode mode) {
    std::vector<Tok> v;
    const auto cps = codepoints(s);
    auto endOf = [&](size_t i) { return (i + 1 < cps.size()) ? cps[i + 1].first : (int)s.size(); };
    if (mode == Mode::Chars) {
        for (size_t i = 0; i < cps.size(); ++i) v.push_back({cps[i].first, endOf(i), s.substr(cps[i].first, endOf(i) - cps[i].first)});
        return v;
    }
    // syllables / words: runs of letters (incl. Tibetan letters/vowels),
    // separated by whitespace, tsheg, and punctuation; each separator run
    // is its own token so spacing differences show but do not swallow words
    auto isSep = [&](unsigned cp) {
        if (cp == 0x0F0B || cp == 0x0F0C) return true;
        if (isTibetanPunctCp(cp)) return true;
        if (cp < 0x80) { const unsigned char c = (unsigned char)cp; return std::isspace(c) || (std::ispunct(c) && c != '\'' && c != '-' && c != '+'); }
        return false;
    };
    size_t i = 0;
    while (i < cps.size()) {
        const bool sep = isSep(cps[i].second);
        size_t j = i;
        if (sep) { while (j < cps.size() && isSep(cps[j].second)) ++j; }
        else { while (j < cps.size() && !isSep(cps[j].second)) ++j; }
        const int b = cps[i].first, e = (j < cps.size()) ? cps[j].first : (int)s.size();
        v.push_back({b, e, s.substr(b, e - b)});
        i = j;
    }
    return v;
}
}  // namespace

std::vector<Span> diffInline(const std::string& a, const std::string& b, Mode mode) {
    if (mode == Mode::Auto) mode = looksTibetan(a) || looksTibetan(b) ? Mode::Syllables : Mode::Words;
    auto ta = tokenize(a, mode), tb = tokenize(b, mode);
    std::unordered_map<std::string, int> table;
    std::vector<std::string> ka, kb;
    for (const auto& t : ta) ka.push_back(t.key);
    for (const auto& t : tb) kb.push_back(t.key);
    const auto ops = myers(idsFor(ka, table), idsFor(kb, table));
    std::vector<Span> out;
    auto aOff = [&](int tokIdx) { return tokIdx < (int)ta.size() ? ta[tokIdx].beg : (int)a.size(); };
    auto bOff = [&](int tokIdx) { return tokIdx < (int)tb.size() ? tb[tokIdx].beg : (int)b.size(); };
    for (size_t i = 0; i < ops.size(); ++i) {
        const Op& op = ops[i];
        if (op.kind == 0) { out.push_back({aOff(op.aPos), aOff(op.aPos + op.len), bOff(op.bPos), bOff(op.bPos + op.len), true}); continue; }
        Span s{aOff(op.aPos), aOff(op.aPos), bOff(op.bPos), bOff(op.bPos), false};
        if (op.kind == 1) { s.aEnd = aOff(op.aPos + op.len); if (i + 1 < ops.size() && ops[i + 1].kind == 2 && ops[i + 1].aPos == op.aPos + op.len && ops[i + 1].bPos == op.bPos) { s.bEnd = bOff(op.bPos + ops[i + 1].len); ++i; } }
        else s.bEnd = bOff(op.bPos + op.len);
        out.push_back(s);
    }
    for (size_t i = 1; i < out.size();) {
        if (!out[i].equal && !out[i - 1].equal) { out[i - 1].aEnd = out[i].aEnd; out[i - 1].bEnd = out[i].bEnd; out.erase(out.begin() + i); }
        else ++i;
    }
    // Two edits separated only by whitespace/punctuation read as one
    // variant ("THAMS CAD" ↔ "KUN", not "THAMS" + "CAD"): merge them.
    auto sepOnly = [&](const std::string& s, int lo, int hi) {
        for (const auto& cp : codepoints(s.substr(lo, hi - lo))) {
            const unsigned c = cp.second;
            const bool sep = c == 0x0F0B || c == 0x0F0C || isTibetanPunctCp(c) || (c < 0x80 && (std::isspace((int)c) || std::ispunct((int)c)));
            if (!sep) return false;
        }
        return true;
    };
    for (size_t i = 1; i + 1 < out.size();) {
        if (out[i].equal && !out[i - 1].equal && !out[i + 1].equal && sepOnly(a, out[i].aBeg, out[i].aEnd)) {
            out[i - 1].aEnd = out[i + 1].aEnd; out[i - 1].bEnd = out[i + 1].bEnd;
            out.erase(out.begin() + i, out.begin() + i + 2);
        } else ++i;
    }
    if (out.empty()) out.push_back({0, (int)a.size(), 0, (int)b.size(), a == b});
    return out;
}

// -------------------------------------------------------------- diff3
MergeResult merge3(const std::vector<std::string>& base, const std::vector<std::string>& left,
                   const std::vector<std::string>& right, const Options& o) {
    struct Ch { int bBeg, bEnd, sBeg, sEnd; };   // base range → side range
    auto changes = [&](const std::vector<std::string>& side) {
        std::vector<Ch> v;
        for (const auto& h : diffLines(base, side, o).hunks)
            if (h.kind != Kind::Equal && !h.unimportant) v.push_back({h.aBeg, h.aEnd, h.bBeg, h.bEnd});
        return v;
    };
    const auto L = changes(left), Rr = changes(right);
    MergeResult m;
    size_t i = 0, j = 0;
    int basePos = 0, leftPos = 0, rightPos = 0;
    const int INF = 1 << 30;
    auto stable = [&](int upto) {
        if (upto <= basePos) return;
        Chunk c; c.kind = ChunkKind::Stable;
        c.baseBeg = basePos; c.baseEnd = upto;
        c.leftBeg = leftPos; c.leftEnd = leftPos + (upto - basePos);
        c.rightBeg = rightPos; c.rightEnd = rightPos + (upto - basePos);
        m.chunks.push_back(c);
        leftPos = c.leftEnd; rightPos = c.rightEnd; basePos = upto;
    };
    while (i < L.size() || j < Rr.size()) {
        const int nl = i < L.size() ? L[i].bBeg : INF, nr = j < Rr.size() ? Rr[j].bBeg : INF;
        const int rs = std::min(nl, nr);
        stable(rs);
        int re = rs; bool usedL = false, usedR = false; int lEnd = -1, rEnd = -1;
        bool progress = true;
        while (progress) {
            progress = false;
            // absorb a change that starts inside the region, or at its start
            // (two insertions at the same point overlap by definition)
            if (i < L.size() && (L[i].bBeg < re || L[i].bBeg == rs)) {
                re = std::max(re, L[i].bEnd); lEnd = L[i].sEnd; usedL = true; ++i; progress = true; continue;
            }
            if (j < Rr.size() && (Rr[j].bBeg < re || Rr[j].bBeg == rs)) {
                re = std::max(re, Rr[j].bEnd); rEnd = Rr[j].sEnd; usedR = true; ++j; progress = true;
            }
        }
        Chunk c; c.baseBeg = rs; c.baseEnd = re;
        // a side's range ends where its last absorbed change ended, plus the
        // base lines of the region after that change (unchanged on that side)
        c.leftBeg = leftPos;  c.leftEnd = usedL ? lEnd + (re - L[i - 1].bEnd) : leftPos + (re - rs);
        c.rightBeg = rightPos; c.rightEnd = usedR ? rEnd + (re - Rr[j - 1].bEnd) : rightPos + (re - rs);
        if (usedL && usedR) {
            const bool same = (c.leftEnd - c.leftBeg) == (c.rightEnd - c.rightBeg) &&
                              std::equal(left.begin() + c.leftBeg, left.begin() + c.leftEnd, right.begin() + c.rightBeg);
            c.kind = same ? ChunkKind::Both : ChunkKind::Conflict;
            if (same) ++m.autoMerged; else ++m.conflicts;
        } else if (usedL) { c.kind = ChunkKind::Left; ++m.autoMerged; }
        else { c.kind = ChunkKind::Right; ++m.autoMerged; }
        m.chunks.push_back(c);
        basePos = re; leftPos = c.leftEnd; rightPos = c.rightEnd;
    }
    stable((int)base.size());
    return m;
}

std::vector<std::string> renderMerge(const MergeResult& m, const std::vector<std::string>& base,
                                     const std::vector<std::string>& left, const std::vector<std::string>& right,
                                     const std::string& leftName, const std::string& rightName) {
    std::vector<std::string> out;
    auto put = [&](const std::vector<std::string>& v, int lo, int hi) { for (int k = lo; k < hi && k < (int)v.size(); ++k) out.push_back(v[k]); };
    for (const auto& c : m.chunks) {
        switch (c.kind) {
        case ChunkKind::Stable: put(base, c.baseBeg, c.baseEnd); break;
        case ChunkKind::Left: case ChunkKind::Both: put(left, c.leftBeg, c.leftEnd); break;
        case ChunkKind::Right: put(right, c.rightBeg, c.rightEnd); break;
        case ChunkKind::Conflict:
            if (c.resolution == 1) put(left, c.leftBeg, c.leftEnd);
            else if (c.resolution == 2) put(right, c.rightBeg, c.rightEnd);
            else if (c.resolution == 3) { put(left, c.leftBeg, c.leftEnd); put(right, c.rightBeg, c.rightEnd); }
            else if (c.resolution == 4) put(base, c.baseBeg, c.baseEnd);
            else {
                out.push_back("<<<<<<< " + leftName); put(left, c.leftBeg, c.leftEnd);
                out.push_back("======="); put(right, c.rightBeg, c.rightEnd);
                out.push_back(">>>>>>> " + rightName);
            }
            break;
        }
    }
    return out;
}

// ------------------------------------------------------------ reports
std::string unifiedDiff(const std::string& aName, const std::string& bName,
                        const std::vector<std::string>& a, const std::vector<std::string>& b,
                        const Result& r, int context) {
    std::ostringstream o;
    o << "--- " << aName << "\n+++ " << bName << "\n";
    // group non-equal hunks (raw differences, minor included — a patch is exact)
    const auto& H = r.hunks;
    size_t i = 0;
    while (i < H.size()) {
        if (H[i].kind == Kind::Equal) { ++i; continue; }
        size_t j = i;
        // extend while the gap of equal lines between differences ≤ 2*context
        while (true) {
            size_t k = j + 1;
            if (k >= H.size()) break;
            if (H[k].kind == Kind::Equal) {
                if (H[k].aEnd - H[k].aBeg > 2 * context || k + 1 >= H.size()) break;
                j = k + 1;
            } else j = k;
        }
        const int aStart = std::max(0, H[i].aBeg - context), bStart = std::max(0, H[i].bBeg - context);
        const int aStop = std::min((int)a.size(), H[j].aEnd + context), bStop = std::min((int)b.size(), H[j].bEnd + context);
        o << "@@ -" << (aStart + 1) << "," << (aStop - aStart) << " +" << (bStart + 1) << "," << (bStop - bStart) << " @@\n";
        for (int k = aStart; k < H[i].aBeg; ++k) o << " " << a[k] << "\n";
        for (size_t h = i; h <= j; ++h) {
            if (H[h].kind == Kind::Equal) { for (int k = H[h].aBeg; k < H[h].aEnd; ++k) o << " " << a[k] << "\n"; continue; }
            for (int k = H[h].aBeg; k < H[h].aEnd; ++k) o << "-" << a[k] << "\n";
            for (int k = H[h].bBeg; k < H[h].bEnd; ++k) o << "+" << b[k] << "\n";
        }
        for (int k = H[j].aEnd; k < aStop; ++k) o << " " << a[k] << "\n";
        i = j + 1;
    }
    return o.str();
}

static std::string esc(const std::string& s) {
    std::string o;
    for (char c : s) { if (c == '&') o += "&amp;"; else if (c == '<') o += "&lt;"; else if (c == '>') o += "&gt;"; else o += c; }
    return o;
}
static std::string markInline(const std::string& a, const std::string& b, bool leftSide) {
    const auto spans = diffInline(a, b);
    std::string o;
    for (const auto& s : spans) {
        const std::string piece = leftSide ? a.substr(s.aBeg, s.aEnd - s.aBeg) : b.substr(s.bBeg, s.bEnd - s.bBeg);
        if (piece.empty()) continue;
        o += s.equal ? esc(piece) : "<mark>" + esc(piece) + "</mark>";
    }
    return o;
}

std::string sideBySideHtml(const std::string& aName, const std::string& bName,
                           const std::vector<std::string>& a, const std::vector<std::string>& b,
                           const Result& r, bool differencesOnly, int context) {
    std::ostringstream o;
    o << "<!doctype html><meta charset='utf-8'><title>Compare: " << esc(aName) << " vs " << esc(bName) << "</title>\n"
      << "<style>body{font-family:-apple-system,Helvetica,sans-serif;font-size:13px}table{border-collapse:collapse;width:100%}"
         "td{vertical-align:top;padding:1px 6px;white-space:pre-wrap;font-family:'Noto Serif Tibetan','Menlo',monospace}"
         "td.n{color:#888;text-align:right;width:3em;user-select:none}tr.chg td.t{background:#fff3c4}tr.ins td.r{background:#d9f5d3}"
         "tr.del td.l{background:#f8d7d5}tr.min td.t{background:#eeeeee}tr.mov td.t{background:#dbe7ff}mark{background:#ffd54a}"
         "tr.gap td{color:#999;text-align:center;background:#fafafa}th{text-align:left;padding:4px 6px;background:#f1ebdd}</style>\n"
      << "<h2>Compare</h2><p>" << esc(summary(r)) << "</p>\n<table><tr><th></th><th>" << esc(aName) << "</th><th></th><th>" << esc(bName) << "</th></tr>\n";
    const auto& H = r.hunks;
    for (size_t i = 0; i < H.size(); ++i) {
        const Hunk& h = H[i];
        if (h.kind == Kind::Equal) {
            const int n = h.aEnd - h.aBeg;
            if (differencesOnly) {
                // leading context (after a difference) and trailing context (before one)
                const bool prevDiff = i > 0, nextDiff = i + 1 < H.size();
                int head = prevDiff ? std::min(n, context) : 0, tail = nextDiff ? std::min(n, context) : 0;
                if (head + tail >= n) { head = n; tail = 0; }
                for (int k = 0; k < head; ++k) o << "<tr><td class='n'>" << (h.aBeg + k + 1) << "</td><td class='t l'>" << esc(a[h.aBeg + k]) << "</td><td class='n'>" << (h.bBeg + k + 1) << "</td><td class='t r'>" << esc(b[h.bBeg + k]) << "</td></tr>\n";
                if (n - head - tail > 0) o << "<tr class='gap'><td colspan='4'>⋯ " << (n - head - tail) << " identical lines ⋯</td></tr>\n";
                for (int k = n - tail; k < n; ++k) o << "<tr><td class='n'>" << (h.aBeg + k + 1) << "</td><td class='t l'>" << esc(a[h.aBeg + k]) << "</td><td class='n'>" << (h.bBeg + k + 1) << "</td><td class='t r'>" << esc(b[h.bBeg + k]) << "</td></tr>\n";
            } else
                for (int k = 0; k < n; ++k) o << "<tr><td class='n'>" << (h.aBeg + k + 1) << "</td><td class='t l'>" << esc(a[h.aBeg + k]) << "</td><td class='n'>" << (h.bBeg + k + 1) << "</td><td class='t r'>" << esc(b[h.bBeg + k]) << "</td></tr>\n";
            continue;
        }
        const char* cls = h.unimportant ? "min" : h.movedPeer >= 0 ? "mov" : h.kind == Kind::Change ? "chg" : h.kind == Kind::Insert ? "ins" : "del";
        const int rows = std::max(h.aEnd - h.aBeg, h.bEnd - h.bBeg);
        for (int k = 0; k < rows; ++k) {
            const bool hasA = h.aBeg + k < h.aEnd, hasB = h.bBeg + k < h.bEnd;
            o << "<tr class='" << cls << "'>";
            o << "<td class='n'>" << (hasA ? std::to_string(h.aBeg + k + 1) : "") << "</td><td class='t l'>";
            if (hasA) o << (hasB && h.kind == Kind::Change ? markInline(a[h.aBeg + k], b[h.bBeg + k], true) : esc(a[h.aBeg + k]));
            o << "</td><td class='n'>" << (hasB ? std::to_string(h.bBeg + k + 1) : "") << "</td><td class='t r'>";
            if (hasB) o << (hasA && h.kind == Kind::Change ? markInline(a[h.aBeg + k], b[h.bBeg + k], false) : esc(b[h.bBeg + k]));
            o << "</td></tr>\n";
        }
    }
    o << "</table>\n";
    return o.str();
}

static std::string citeFor(const std::vector<std::string>& a, int line0) {
    static const std::regex fol("@([0-9]{1,4}[AaBb]?)");
    for (int i = std::min(line0, (int)a.size() - 1); i >= 0; --i) {
        std::smatch m;
        std::string last; auto it = std::sregex_iterator(a[i].begin(), a[i].end(), fol);
        for (; it != std::sregex_iterator(); ++it) last = (*it)[1].str();
        if (!last.empty()) {
            std::string up = last; for (auto& c : up) c = (char)std::toupper((unsigned char)c);
            return "@" + up + "." + std::to_string(line0 - i + 1);
        }
    }
    return "line " + std::to_string(line0 + 1);
}
static std::string differingTokens(const std::string& a, const std::string& b, bool leftSide) {
    std::string o;
    for (const auto& s : diffInline(a, b)) {
        if (s.equal) continue;
        const std::string piece = leftSide ? a.substr(s.aBeg, s.aEnd - s.aBeg) : b.substr(s.bBeg, s.bEnd - s.bBeg);
        std::string t = collapseWs(piece);
        if (t.empty()) t = "∅";   // reading absent on this side
        if (!o.empty()) o += " … ";
        o += t;
    }
    return o;
}

std::vector<ApparatusEntry> apparatus(const std::vector<std::string>& a, const std::vector<std::string>& b, const Result& r) {
    std::vector<ApparatusEntry> out;
    for (const auto& h : r.hunks) {
        if (h.kind == Kind::Equal || h.unimportant) continue;
        ApparatusEntry e; e.kind = h.kind; e.aLine = h.aBeg + 1; e.bLine = h.bBeg + 1;
        e.cite = citeFor(a, h.kind == Kind::Insert ? std::max(0, h.aBeg - 1) : h.aBeg);
        if (h.kind == Kind::Change) {
            // only the variant readings; " / " marks a line boundary
            std::string at, bt;
            for (int k = h.aBeg; k < h.aEnd; ++k) { if (!at.empty()) at += " / "; at += collapseWs(a[k]); }
            for (int k = h.bBeg; k < h.bEnd; ++k) { if (!bt.empty()) bt += " / "; bt += collapseWs(b[k]); }
            e.left = differingTokens(at, bt, true);
            e.right = differingTokens(at, bt, false);
        } else {
            for (int k = h.aBeg; k < h.aEnd; ++k) { if (!e.left.empty()) e.left += " / "; e.left += collapseWs(a[k]); }
            for (int k = h.bBeg; k < h.bEnd; ++k) { if (!e.right.empty()) e.right += " / "; e.right += collapseWs(b[k]); }
            if (e.left.empty()) e.left = "∅";
            if (e.right.empty()) e.right = "∅";
        }
        out.push_back(e);
    }
    return out;
}
std::string apparatusMarkdown(const std::string& aName, const std::string& bName, const std::vector<ApparatusEntry>& e) {
    std::ostringstream o;
    o << "# Apparatus: " << aName << " (A) against " << bName << " (B)\n\n"
      << "Cited by folio marker and line within the folio, from A. ∅ = reading absent.\n\n";
    for (const auto& x : e) o << "- **" << x.cite << "** A: " << x.left << " ] B: " << x.right << "\n";
    if (e.empty()) o << "_No important differences._\n";
    return o.str();
}
std::string apparatusCsv(const std::vector<ApparatusEntry>& e) {
    auto q = [](const std::string& s) { std::string o = "\""; for (char c : s) { if (c == '"') o += "\"\""; else o += c; } return o + "\""; };
    std::ostringstream o;
    o << "cite,kind,a_line,b_line,a_reading,b_reading\n";
    for (const auto& x : e)
        o << q(x.cite) << "," << (x.kind == Kind::Change ? "change" : x.kind == Kind::Insert ? "insert" : "delete") << "," << x.aLine << "," << x.bLine << "," << q(x.left) << "," << q(x.right) << "\n";
    return o.str();
}
std::string summary(const Result& r) {
    std::ostringstream o;
    if (r.differences() == 0) o << "No important differences";
    else o << r.differences() << " difference" << (r.differences() == 1 ? "" : "s") << " (" << r.changes << " changed, " << r.inserts << " only on the right, " << r.deletes << " only on the left)";
    if (r.unimportant) o << "; " << r.unimportant << " minor";
    if (r.moved) o << "; " << r.moved << " moved";
    if (r.unnormalised) o << "; " << r.unnormalised << " line" << (r.unnormalised == 1 ? "" : "s") << " compared raw (could not be converted)";
    o << ". Lines: " << r.aLines << " / " << r.bLines << ".";
    return o.str();
}

}  // namespace textdiff
}  // namespace allcore
