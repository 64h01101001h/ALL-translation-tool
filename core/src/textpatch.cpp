// allcore/textpatch.cpp — see textpatch.h. Two halves that never talk to each
// other: a parser that turns unified-diff text into hunks (or into errors that
// name their line), and an applier that places those hunks by search.
//
// The applier is deliberately dumb and legible: for each hunk, build the
// "before" image (' ' and '-' lines) and the "after" image (' ' and '+' lines),
// then walk candidate positions outward from where the header said the hunk
// belongs. No scoring, no best-of: the FIRST position whose lines match wins,
// and if none does the hunk is rejected. That is what makes every verdict
// explainable to the translator in one sentence.
#include "allcore/textpatch.h"

#include <algorithm>
#include <cstring>
#include <string>
#include <vector>

namespace allcore::textpatch {

namespace {

// The one reason string the .rej export must recognise and skip (a hunk the
// user simply did not tick is not a failure of the patch).
const char* const kNotSelected = "not selected";

// ------------------------------------------------------------ small text work
bool startsWith(const std::string& s, const char* p) {
    const size_t n = std::strlen(p);
    return s.size() >= n && std::memcmp(s.data(), p, n) == 0;
}
std::string lineTag(size_t ix0) { return "line " + std::to_string(ix0 + 1) + ": "; }

// A quoted fragment for a message: one line, clipped on a UTF-8 boundary so a
// Tibetan line never comes back as broken bytes.
std::string quoteClip(const std::string& s, size_t limit = 60) {
    if (s.size() <= limit) return s;
    size_t n = limit;
    while (n > 0 && (static_cast<unsigned char>(s[n]) & 0xC0) == 0x80) --n;
    return s.substr(0, n) + "\xE2\x80\xA6";   // …
}

// Split on '\n', dropping one '\r' before it. The patch's own line endings are
// not data: the applier compares text, and the caller re-joins in the TARGET's
// EOL. A CRLF patch is therefore tolerated by stripping, and noted.
std::vector<std::string> splitPatchLines(const std::string& t, bool& sawCrlf) {
    std::vector<std::string> out;
    sawCrlf = false;
    size_t start = 0;
    while (start <= t.size()) {
        const size_t nl = t.find('\n', start);
        if (nl == std::string::npos) {
            if (start < t.size()) {
                std::string s = t.substr(start);
                if (!s.empty() && s.back() == '\r') { s.pop_back(); sawCrlf = true; }
                out.push_back(std::move(s));
            }
            break;
        }
        std::string s = t.substr(start, nl - start);
        if (!s.empty() && s.back() == '\r') { s.pop_back(); sawCrlf = true; }
        out.push_back(std::move(s));
        start = nl + 1;
    }
    return out;
}

// ------------------------------------------------------------ comparison
// One trailing CR never decides a comparison (CRLF target, LF patch).
size_t effLen(const std::string& s) { return (!s.empty() && s.back() == '\r') ? s.size() - 1 : s.size(); }

// patch -l for CONTEXT lines: runs of spaces and tabs collapse, the ends are
// trimmed. Written as a two-pointer walk so the hot search allocates nothing.
bool looseEqual(const char* a, size_t an, const char* b, size_t bn) {
    auto ws = [](char c) { return c == ' ' || c == '\t'; };
    size_t i = 0, j = 0;
    while (i < an && ws(a[i])) ++i;
    while (j < bn && ws(b[j])) ++j;
    while (an > i && ws(a[an - 1])) --an;
    while (bn > j && ws(b[bn - 1])) --bn;
    while (i < an && j < bn) {
        const bool wa = ws(a[i]), wb = ws(b[j]);
        if (wa != wb) return false;
        if (wa) { while (i < an && ws(a[i])) ++i; while (j < bn && ws(b[j])) ++j; }
        else { if (a[i] != b[j]) return false; ++i; ++j; }
    }
    return i == an && j == bn;
}
bool sameLine(const std::string& patchText, const std::string& targetText, bool loose) {
    const size_t pn = effLen(patchText), tn = effLen(targetText);
    if (!loose) return pn == tn && std::memcmp(patchText.data(), targetText.data(), pn) == 0;
    return looseEqual(patchText.data(), pn, targetText.data(), tn);
}

// ------------------------------------------------------------ header parsing
bool readInt(const std::string& s, size_t& i, int& out) {
    if (i >= s.size() || s[i] < '0' || s[i] > '9') return false;
    long long v = 0;
    while (i < s.size() && s[i] >= '0' && s[i] <= '9') {
        if (v < 100000000LL) v = v * 10 + (s[i] - '0');   // clamp; no overflow, no guess
        ++i;
    }
    out = static_cast<int>(v);
    return true;
}

// "@@ -a[,n] +b[,m] @@[ section]". A missing count means 1.
bool parseHunkHeader(const std::string& s, int& aStart, int& aLen, int& bStart, int& bLen) {
    if (!startsWith(s, "@@")) return false;
    size_t i = 2;
    while (i < s.size() && s[i] == ' ') ++i;
    if (i >= s.size() || s[i] != '-') return false;
    ++i;
    if (!readInt(s, i, aStart)) return false;
    aLen = 1;
    if (i < s.size() && s[i] == ',') { ++i; if (!readInt(s, i, aLen)) return false; }
    if (i >= s.size() || s[i] != ' ') return false;
    while (i < s.size() && s[i] == ' ') ++i;
    if (i >= s.size() || s[i] != '+') return false;
    ++i;
    if (!readInt(s, i, bStart)) return false;
    bLen = 1;
    if (i < s.size() && s[i] == ',') { ++i; if (!readInt(s, i, bLen)) return false; }
    while (i < s.size() && s[i] == ' ') ++i;
    return s.compare(i, 2, "@@") == 0;
}

// Everything after the second "@@" of a header (the section text git puts there).
std::string headerSection(const std::string& header) {
    const size_t p1 = header.find("@@");
    if (p1 == std::string::npos) return std::string();
    const size_t p2 = header.find("@@", p1 + 2);
    if (p2 == std::string::npos) return std::string();
    return header.substr(p2 + 2);
}

// "--- name[\t timestamp]" → name. The name ends at a tab if there is one (GNU
// writes the timestamp after a tab), otherwise it is the rest of the line with
// trailing whitespace trimmed — so a name containing spaces round-trips.
std::string headerName(const std::string& s) {
    std::string n = s.substr(4);
    const size_t tab = n.find('\t');
    if (tab != std::string::npos) n = n.substr(0, tab);
    while (!n.empty() && (n.back() == ' ' || n.back() == '\t')) n.pop_back();
    return n;
}
bool isMinusHeader(const std::string& s) { return startsWith(s, "--- "); }
bool isPlusHeader(const std::string& s) { return startsWith(s, "+++ "); }
bool isFileHeaderAt(const std::vector<std::string>& L, size_t i) {
    return isMinusHeader(L[i]) && i + 1 < L.size() && isPlusHeader(L[i + 1]);
}

// Noise a real patch carries around its hunks. The spec names diff --git,
// index, similarity, Index:, ==== and blank separators; the git mode/rename
// lines always travel with "diff --git", so they are tolerated with them
// rather than turned into errors the user cannot act on.
bool isTolerated(const std::string& s) {
    return startsWith(s, "diff ") || startsWith(s, "index ") ||
           startsWith(s, "similarity ") || startsWith(s, "dissimilarity ") ||
           startsWith(s, "old mode") || startsWith(s, "new mode") ||
           startsWith(s, "new file mode") || startsWith(s, "deleted file mode") ||
           startsWith(s, "rename ") || startsWith(s, "copy ") ||
           startsWith(s, "Index:") || startsWith(s, "====");
}
bool isBodyTag(const std::string& s) {
    return !s.empty() && (s[0] == ' ' || s[0] == '-' || s[0] == '+' || s[0] == '\\');
}

// ------------------------------------------------------------ names
std::string stripAB(const std::string& n) {
    if (n.size() > 2 && (n[0] == 'a' || n[0] == 'b') && n[1] == '/') return n.substr(2);
    return n;
}
std::string baseName(const std::string& n) {
    const size_t s = n.find_last_of("/\\");
    return s == std::string::npos ? n : n.substr(s + 1);
}

// ------------------------------------------------------------ hunk images
std::vector<PatchLine> imageOf(const std::vector<PatchLine>& body, bool before) {
    std::vector<PatchLine> img;
    img.reserve(body.size());
    for (const auto& l : body) {
        if (l.tag == ' ' || (before ? l.tag == '-' : l.tag == '+')) img.push_back(l);
    }
    return img;
}

// Drop up to f leading and up to f trailing CONTEXT lines — and only context
// lines. dropFront is reported so the caller can shift the expected position
// by exactly what fuzzing removed.
std::vector<PatchLine> fuzzTrim(const std::vector<PatchLine>& body, int f, size_t& dropFront) {
    dropFront = 0;
    size_t lead = 0, trail = 0;
    while (lead < body.size() && body[lead].tag == ' ') ++lead;
    while (trail < body.size() && body[body.size() - 1 - trail].tag == ' ') ++trail;
    const size_t wf = static_cast<size_t>(f);
    size_t front = std::min(wf, lead), back = std::min(wf, trail);
    if (front + back >= body.size()) return std::vector<PatchLine>();
    dropFront = front;
    return std::vector<PatchLine>(body.begin() + static_cast<long>(front),
                                  body.end() - static_cast<long>(back));
}
bool hasContext(const std::vector<PatchLine>& body) {
    for (const auto& l : body) if (l.tag == ' ') return true;
    return false;
}

// Does img sit at pos? firstBad reports the first line that did not, so a
// rejection can quote the target line the translator has to look at.
bool matchAt(const std::vector<std::string>& lines, long long pos,
             const std::vector<PatchLine>& img, bool loose, long long* firstBad = nullptr) {
    if (pos < 0 || pos + static_cast<long long>(img.size()) > static_cast<long long>(lines.size())) {
        if (firstBad) *firstBad = -1;
        return false;
    }
    for (size_t k = 0; k < img.size(); ++k) {
        // loose relaxes CONTEXT only — a '-' or '+' line is never relaxed
        if (!sameLine(img[k].text, lines[static_cast<size_t>(pos) + k], loose && img[k].tag == ' ')) {
            if (firstBad) *firstBad = static_cast<long long>(k);
            return false;
        }
    }
    return true;
}

}  // namespace

// ================================================================= parser
ParseResult parseUnified(const std::string& patchText) {
    ParseResult pr;
    bool crlf = false;
    const std::vector<std::string> L = splitPatchLines(patchText, crlf);
    std::vector<std::string> pending;
    if (crlf) pending.push_back("the patch uses CRLF line endings; the carriage returns were stripped");

    int curIx = -1;
    size_t i = 0;
    while (i < L.size()) {
        const std::string& s = L[i];

        if (isFileHeaderAt(L, i)) {
            PatchFile f;
            f.aName = headerName(s);
            f.bName = headerName(L[i + 1]);
            f.warnings = std::move(pending);
            pending.clear();
            pr.files.push_back(std::move(f));
            curIx = static_cast<int>(pr.files.size()) - 1;
            i += 2;
            continue;
        }
        if (curIx < 0) {   // preamble: patch(1) skips leading garbage, and says so
            if (!s.empty()) pending.push_back(lineTag(i) + quoteClip(s, 120));
            ++i;
            continue;
        }
        PatchFile& cur = pr.files[static_cast<size_t>(curIx)];

        if (startsWith(s, "@@")) {
            int aStart = 0, aLen = 0, bStart = 0, bLen = 0;
            if (!parseHunkHeader(s, aStart, aLen, bStart, bLen)) {
                pr.errors.push_back(lineTag(i) + "expected @@ header, got \"" + quoteClip(s) + "\"");
                ++i;
                continue;
            }
            const size_t headerIx = i;
            Hunk h;
            h.aStart = aStart; h.aLen = aLen; h.bStart = bStart; h.bLen = bLen;
            h.header = s;
            ++i;
            int aSeen = 0, bSeen = 0;
            bool broken = false;
            while (i < L.size() && (aSeen < aLen || bSeen < bLen)) {
                const std::string& t = L[i];
                if (!t.empty() && t[0] == '\\') {   // "\ No newline at end of file"
                    if (!h.lines.empty()) {
                        const char last = h.lines.back().tag;
                        if (last == '-' || last == ' ') h.noNewlineA = true;
                        if (last == '+' || last == ' ') h.noNewlineB = true;
                    }
                    ++i;
                    continue;
                }
                char tag = ' ';
                std::string text;
                if (t.empty()) {
                    // a mailer that ate the trailing space off a blank context
                    // line; only read as context while the hunk still needs lines
                    tag = ' ';
                } else if (t[0] == ' ' || t[0] == '-' || t[0] == '+') {
                    tag = t[0];
                    text = t.substr(1);
                } else { broken = true; break; }
                const int aAdd = (tag == '+') ? 0 : 1, bAdd = (tag == '-') ? 0 : 1;
                if (aSeen + aAdd > aLen || bSeen + bAdd > bLen) { broken = true; break; }
                aSeen += aAdd; bSeen += bAdd;
                h.lines.push_back(PatchLine{tag, std::move(text)});
                ++i;
            }
            if (broken || aSeen != aLen || bSeen != bLen) {
                pr.errors.push_back(lineTag(headerIx) + "hunk \"" + quoteClip(h.header) +
                                    "\" body has -" + std::to_string(aSeen) + " +" + std::to_string(bSeen) +
                                    " lines but the header says -" + std::to_string(aLen) +
                                    " +" + std::to_string(bLen) + "; the hunk was dropped");
                // swallow the rest of this body so one bad header is one error
                while (i < L.size() && isBodyTag(L[i]) && !startsWith(L[i], "@@") && !isFileHeaderAt(L, i)) ++i;
                continue;
            }
            if (i < L.size() && !L[i].empty() && L[i][0] == '\\') {
                if (!h.lines.empty()) {
                    const char last = h.lines.back().tag;
                    if (last == '-' || last == ' ') h.noNewlineA = true;
                    if (last == '+' || last == ' ') h.noNewlineB = true;
                }
                ++i;
            }
            cur.hunks.push_back(std::move(h));
            continue;
        }
        if (isTolerated(s)) { cur.warnings.push_back(lineTag(i) + quoteClip(s, 120)); ++i; continue; }
        if (s.empty()) { ++i; continue; }   // blank separator between files
        pr.errors.push_back(lineTag(i) + "expected @@ header, got \"" + quoteClip(s) + "\"");
        ++i;
    }
    if (!pending.empty() && !pr.files.empty()) {
        PatchFile& last = pr.files.back();
        last.warnings.insert(last.warnings.end(), pending.begin(), pending.end());
    }
    return pr;
}

// ================================================================= applier
ApplyResult applyPatch(const std::vector<std::string>& target, const PatchFile& pIn,
                       const ApplyOptions& o) {
    const PatchFile p = o.reverse ? reversePatch(pIn) : pIn;
    const int maxFuzz = o.maxFuzz < 0 ? 0 : o.maxFuzz;
    const bool loose = o.looseWhitespace;

    ApplyResult r;
    r.lines = target;
    long long delta = 0;

    for (size_t hi = 0; hi < p.hunks.size(); ++hi) {
        Outcome out;
        out.hunkIx = static_cast<int>(hi);
        out.how = Placement::Rejected;

        if (!o.onlyHunks.empty() &&
            std::find(o.onlyHunks.begin(), o.onlyHunks.end(), static_cast<int>(hi)) == o.onlyHunks.end()) {
            out.reason = kNotSelected;
            r.outcomes.push_back(out);
            ++r.rejected;
            continue;   // the delta is untouched: nothing was placed
        }

        const Hunk& h = p.hunks[hi];
        const std::vector<PatchLine> before0 = imageOf(h.lines, true);
        const std::vector<PatchLine> after0 = imageOf(h.lines, false);
        // A pure insertion's header names the line it goes AFTER (patch(1)'s
        // "-5,0"); every other hunk's names its first line.
        const long long expected =
            (before0.empty() ? static_cast<long long>(h.aStart) : static_cast<long long>(h.aStart) - 1) + delta;

        bool placed = false, noContextLeft = false;
        for (int f = 0; f <= maxFuzz && !placed; ++f) {
            size_t dropFront = 0;
            const std::vector<PatchLine> body = (f == 0) ? h.lines : fuzzTrim(h.lines, f, dropFront);
            if (f > 0 && !hasContext(body)) { noContextLeft = true; break; }   // and every higher f too

            const std::vector<PatchLine> before = imageOf(body, true);
            const std::vector<PatchLine> after = imageOf(body, false);
            const long long hiPos = static_cast<long long>(r.lines.size()) - static_cast<long long>(before.size());
            if (hiPos < 0) continue;   // the hunk is longer than what is left
            const long long centre = expected + static_cast<long long>(dropFront);

            // An empty before image matches vacuously anywhere, so it carries no
            // evidence: ask the '+' side first, or a -U0 insertion would be
            // applied twice in silence.
            if (f == 0 && before.empty() && !after.empty()) {
                const long long q = std::min(std::max(centre, 0LL), hiPos);
                if (matchAt(r.lines, q, after, loose)) {
                    out.how = Placement::AlreadyApplied;
                    out.atLine = static_cast<int>(q);
                    out.offset = static_cast<int>(q - centre);
                    placed = true;
                    break;
                }
            }

            const long long maxD = std::max(centre - 0LL, hiPos - centre);
            for (long long d = 0; d <= maxD && !placed; ++d) {
                for (int side = 0; side < 2 && !placed; ++side) {
                    if (d == 0 && side == 1) continue;
                    const long long q = (side == 0) ? centre - d : centre + d;   // -1, +1, -2, +2, …
                    if (q < 0 || q > hiPos) continue;
                    if (matchAt(r.lines, q, before, loose)) {
                        std::vector<std::string> repl;
                        repl.reserve(after.size());
                        size_t tq = static_cast<size_t>(q);
                        for (const auto& l : body) {
                            if (l.tag == ' ') { repl.push_back(r.lines[tq]); ++tq; }   // the TARGET's line
                            else if (l.tag == '-') { ++tq; }
                            else repl.push_back(l.text);
                        }
                        r.lines.erase(r.lines.begin() + static_cast<long>(q),
                                      r.lines.begin() + static_cast<long>(q) + static_cast<long>(before.size()));
                        r.lines.insert(r.lines.begin() + static_cast<long>(q), repl.begin(), repl.end());
                        delta += static_cast<long long>(after.size()) - static_cast<long long>(before.size());
                        out.atLine = static_cast<int>(q);
                        out.offset = static_cast<int>(q - centre);
                        out.fuzz = f;
                        out.how = (f > 0) ? Placement::Fuzz
                                          : (out.offset != 0 ? Placement::Offset : Placement::Exact);
                        placed = true;
                    } else if (f == 0 && !after.empty() && matchAt(r.lines, q, after, loose)) {
                        out.how = Placement::AlreadyApplied;
                        out.atLine = static_cast<int>(q);
                        out.offset = static_cast<int>(q - centre);
                        placed = true;
                    }
                }
            }
        }

        if (!placed) {
            // Name the line the translator has to look at: the first mismatch
            // where the patch said the hunk belongs.
            const long long hiPos = static_cast<long long>(r.lines.size()) - static_cast<long long>(before0.size());
            if (hiPos < 0) {
                out.reason = "the hunk is longer than the text it is being applied to";
            } else {
                const long long q = std::min(std::max(expected, 0LL), hiPos);
                long long bad = 0;
                matchAt(r.lines, q, before0, loose, &bad);
                if (bad < 0) out.reason = "the hunk runs past the end of the text";
                else {
                    const size_t tl = static_cast<size_t>(q + bad);
                    out.reason = "line " + std::to_string(tl + 1) + " does not match \"" +
                                 quoteClip(r.lines[tl]) + "\"";
                }
            }
            if (noContextLeft) out.reason += " (fuzz would leave no context, so the hunk was not placed anywhere)";
            ++r.rejected;
        } else {
            switch (out.how) {
                case Placement::Exact: ++r.exact; break;
                case Placement::Offset: ++r.offsetApplied; break;
                case Placement::Fuzz: ++r.fuzzApplied; break;
                case Placement::AlreadyApplied: ++r.alreadyApplied; break;
                case Placement::Rejected: break;
            }
        }
        r.outcomes.push_back(std::move(out));
    }
    return r;
}

// ================================================================= reports
std::string rejectedAsUnified(const PatchFile& p, const ApplyResult& r) {
    std::vector<int> rej;
    for (const auto& o : r.outcomes) {
        if (o.how != Placement::Rejected) continue;
        if (o.reason == kNotSelected) continue;   // not a failure of the patch
        if (o.hunkIx >= 0 && static_cast<size_t>(o.hunkIx) < p.hunks.size()) rej.push_back(o.hunkIx);
    }
    if (rej.empty()) return std::string();

    std::string out;
    out += "--- " + p.aName + "\n";
    out += "+++ " + p.bName + "\n";
    for (int ix : rej) {
        const Hunk& h = p.hunks[static_cast<size_t>(ix)];
        out += h.header + "\n";   // byte for byte, as the corrector wrote it
        size_t lastMinus = h.lines.size(), lastPlus = h.lines.size();
        for (size_t k = 0; k < h.lines.size(); ++k) {
            if (h.lines[k].tag == '-') lastMinus = k;
            if (h.lines[k].tag == '+') lastPlus = k;
        }
        for (size_t k = 0; k < h.lines.size(); ++k) {
            out += h.lines[k].tag;
            out += h.lines[k].text;
            out += "\n";
            if ((h.noNewlineA && k == lastMinus) || (h.noNewlineB && k == lastPlus))
                out += "\\ No newline at end of file\n";
        }
    }
    return out;
}

PatchFile reversePatch(const PatchFile& p) {
    PatchFile q = p;
    std::swap(q.aName, q.bName);
    for (size_t k = 0; k < q.hunks.size(); ++k) {
        Hunk& h = q.hunks[k];
        std::swap(h.aStart, h.bStart);
        std::swap(h.aLen, h.bLen);
        std::swap(h.noNewlineA, h.noNewlineB);
        for (auto& l : h.lines) {
            if (l.tag == '-') l.tag = '+';
            else if (l.tag == '+') l.tag = '-';
        }
        h.header = "@@ -" + std::to_string(h.aStart) + "," + std::to_string(h.aLen) +
                   " +" + std::to_string(h.bStart) + "," + std::to_string(h.bLen) + " @@" +
                   headerSection(p.hunks[k].header);
    }
    return q;
}

int pickFileForTarget(const ParseResult& pr, const std::string& targetBasename) {
    const std::string want = baseName(targetBasename);
    if (want.empty()) return -1;
    int found = -1;
    for (size_t k = 0; k < pr.files.size(); ++k) {
        const std::string a = baseName(stripAB(pr.files[k].aName));
        const std::string b = baseName(stripAB(pr.files[k].bName));
        if (a != want && b != want) continue;
        if (found >= 0) return -1;   // two candidates: the caller must not guess
        found = static_cast<int>(k);
    }
    return found;
}

}  // namespace allcore::textpatch
