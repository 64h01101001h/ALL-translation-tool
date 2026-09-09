#include "allcore/textspan.h"

#include <algorithm>
#include <cctype>
#include <regex>

#include "allcore/engines.h"
#include "allcore/unicode_wylie.h"
#include "text_util.h"

namespace allcore {
namespace textspan {

static const std::regex& folioRe() {
    static const std::regex re(kFolioMarkerPattern);
    return re;
}

std::vector<FolioMarker> folioMarkers(const std::string& line) {
    std::vector<FolioMarker> out;
    for (auto it = std::sregex_iterator(line.begin(), line.end(), folioRe()); it != std::sregex_iterator(); ++it) {
        FolioMarker m; m.begin = (int)it->position(0); m.end = m.begin + (int)it->length(0);
        m.id = (*it)[1].str();
        for (auto& c : m.id) c = (char)std::toupper((unsigned char)c);
        out.push_back(m);
    }
    return out;
}

int folioMarkerEnd(const std::string& line, std::string* folioOut) {
    const auto ms = folioMarkers(line);
    if (ms.empty()) return -1;
    if (folioOut) *folioOut = ms.front().id;
    return ms.front().end;
}

std::string lastFolio(const std::string& line) {
    const auto ms = folioMarkers(line);
    return ms.empty() ? std::string() : ms.back().id;
}

bool folioParts(const std::string& id, int& number, char& side, int& digitWidth) {
    if (id.empty()) return false;
    size_t i = 0; std::string digits;
    while (i < id.size() && std::isdigit((unsigned char)id[i])) digits += id[i++];
    if (digits.empty()) return false;
    number = std::stoi(digits); digitWidth = (int)digits.size();
    side = i < id.size() ? (char)std::toupper((unsigned char)id[i]) : '\0';
    return true;
}

std::vector<Span> apparatusSpans(const std::string& line, bool* balanced, bool angleBrackets) {
    std::vector<Span> out; bool ok = true;
    int openAt = -1; char openKind = 0;
    for (int i = 0; i < (int)line.size(); ++i) {
        const char c = line[i];
        // ⟨ U+27E8 = E2 9F A8, ⟩ U+27E9 = E2 9F A9; both three bytes, so the
        // walk steps over them rather than reading their tail as text
        int width = 1; char opens = 0, closes = 0;
        if (c == '[' || c == '{') opens = c;
        else if (c == ']') closes = '[';
        else if (c == '}') closes = '{';
        else if (angleBrackets && (unsigned char)c == 0xE2 && i + 2 < (int)line.size() &&
                 (unsigned char)line[i + 1] == 0x9F &&
                 ((unsigned char)line[i + 2] == 0xA8 || (unsigned char)line[i + 2] == 0xA9)) {
            width = 3;
            if ((unsigned char)line[i + 2] == 0xA8) opens = '<'; else closes = '<';
        }
        if (opens) {
            if (openAt >= 0) { ok = false; break; }   // nested or unclosed → refuse
            openAt = i; openKind = opens;
        } else if (closes) {
            if (openAt < 0 || openKind != closes) { ok = false; break; }
            out.push_back({openAt, i + width, openKind}); openAt = -1; openKind = 0;
        }
        i += width - 1;
    }
    if (openAt >= 0) ok = false;
    if (balanced) *balanced = ok;
    if (!ok) out.clear();
    return out;
}

std::string stripApparatus(const std::string& line, bool* balanced) {
    bool ok = true;
    const auto spans = apparatusSpans(line, &ok);
    if (balanced) *balanced = ok;
    if (!ok) return line;
    std::string o; int pos = 0;
    for (const auto& s : spans) { o.append(line, pos, s.begin - pos); pos = s.end; }
    o.append(line, pos, std::string::npos);
    std::string c; bool ws = false;
    for (unsigned char ch : o) { if (std::isspace(ch)) { ws = true; continue; } if (ws && !c.empty()) c += ' '; ws = false; c += (char)ch; }
    return c;
}

// ------------------------------------------------------------ syllables
// The body below is quotation.cpp's `syllables` MOVED, not retyped (F0's
// first rule); quotation_smoke is the proof that the move was faithful.
std::vector<std::string> syllablesWylie(const std::string& wylie) {
    std::vector<std::string> out;
    std::string cur;
    auto flush = [&] {
        // strip punctuation from the edges; keep wylie-significant marks
        // ('a-chung apostrophe, +, ., ~) inside the syllable
        size_t b = 0, e = cur.size();
        while (b < e && !std::isalnum((unsigned char)cur[b]) &&
               cur[b] != '\'')
            ++b;
        while (e > b && !std::isalnum((unsigned char)cur[e - 1]) &&
               cur[e - 1] != '\'')
            --e;
        std::string s = cur.substr(b, e - b);
        for (auto& c : s)
            if (c >= 'A' && c <= 'Z') c = (char)(c - 'A' + 'a');
        // page markers (@012a) and pure numbers are not text
        if (!s.empty() && s[0] != '@' &&
            s.find_first_not_of("0123456789") != std::string::npos)
            out.push_back(s);
        cur.clear();
    };
    for (char c : wylie + " ") {
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '/' ||
            c == '|' || c == ',' || c == ';')
            flush();
        else
            cur += c;
    }
    return out;
}

const char* apparatusName(Apparatus a) {
    switch (a) {
        case Apparatus::KeepInner: return "keep-inner";
        case Apparatus::AsWritten: return "as-written";
        case Apparatus::StripSpan: break;
    }
    return "strip-span";
}

namespace {

// A line, its byte offset in the document, and whether the document
// actually ended it with a newline (an unterminated last line carries no
// line-break barrier). Line breaking follows textdiff::splitLines exactly.
struct DocLine { size_t beg = 0; std::string text; bool terminated = false; };

std::vector<DocLine> docLines(const std::string& t) {
    std::vector<DocLine> v;
    std::string cur; size_t beg = 0;
    for (size_t i = 0; i < t.size(); ++i) {
        const char c = t[i];
        if (c == '\r') {
            v.push_back({beg, cur, true}); cur.clear();
            if (i + 1 < t.size() && t[i + 1] == '\n') ++i;
            beg = i + 1;
        } else if (c == '\n') {
            v.push_back({beg, cur, true}); cur.clear(); beg = i + 1;
        } else cur += c;
    }
    if (!cur.empty()) v.push_back({beg, cur, false});
    return v;
}

// The last line whose start is at or before the offset; an offset past
// the end belongs to the last line.
int lineIndexOf(const std::vector<DocLine>& v, size_t off) {
    for (int i = (int)v.size() - 1; i >= 0; --i)
        if (off >= v[i].beg) return i;
    return 0;
}

// The tokenizer's separators, and the subset that means "a boundary the
// scribe wrote", not merely a space.
bool isFlushByte(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '/' ||
           c == '|' || c == ',' || c == ';';
}
bool isBarrierByte(char c) {
    return c == '/' || c == '|' || c == ',' || c == ';' || c == '\n' || c == '\r';
}
// What counts as "inside a syllable" when an apparatus mark sits against
// it: the bytes the tokenizer would keep — letters, digits, and the
// wylie-significant marks it never strips.
bool isSyllableByte(unsigned char c) {
    return std::isalnum(c) || c == '\'' || c == '+' || c == '.' || c == '~';
}

}  // namespace

std::vector<Syllable> syllableSpans(const std::string& wylie, Apparatus mode, Provenance* out) {
    std::vector<Syllable> res;
    long long refused = 0;
    int stripped = 0;
    std::vector<std::string> refusals;

    const auto lines = docLines(wylie);
    for (size_t li = 0; li < lines.size(); ++li) {
        const std::string& L = lines[li].text;
        const size_t base = lines[li].beg;
        const int lineNo = (int)li + 1;

        // 1. the apparatus policy, as a per-byte plan: 0 keep, 1 blank
        //    (becomes a separator), 2 drop (delimiter only)
        std::vector<char> plan(L.size(), 0);
        if (mode != Apparatus::AsWritten && !L.empty()) {
            bool bal = true;
            const auto spans = apparatusSpans(L, &bal, true);
            if (!bal) {
                // half a bracket is not a span; the line is read as written
                // and the reader is told, rather than the guess being made
                refusals.push_back("line " + std::to_string(lineNo) +
                                   ": unbalanced or nested apparatus bracket — "
                                   "line read as written");
            } else if (mode == Apparatus::KeepInner) {
                for (const auto& s : spans) {
                    const int w = s.kind == '<' ? 3 : 1;   // ⟨ ⟩ are three bytes
                    for (int k = s.begin; k < s.begin + w; ++k) plan[k] = 2;
                    for (int k = s.end - w; k < s.end; ++k) plan[k] = 2;
                    ++stripped;
                }
            } else {
                std::vector<char> refusedRun(L.size(), 0);
                for (const auto& s : spans) {
                    const bool intra =
                        (s.begin > 0 && isSyllableByte((unsigned char)L[s.begin - 1])) ||
                        (s.end < (int)L.size() && isSyllableByte((unsigned char)L[s.end]));
                    if (!intra) {
                        for (int k = s.begin; k < s.end; ++k) plan[k] = 1;
                        ++stripped;
                        continue;
                    }
                    // the mark's boundary falls inside a syllable: the whole
                    // syllable is refused and named, never truncated
                    int rb = s.begin; while (rb > 0 && !isFlushByte(L[rb - 1])) --rb;
                    int re = s.end;   while (re < (int)L.size() && !isFlushByte(L[re])) ++re;
                    if (refusedRun[rb]) continue;   // a second mark in the same syllable
                    refusals.push_back("line " + std::to_string(lineNo) + ": " +
                                       L.substr(rb, re - rb) +
                                       " — editorial mark inside a syllable; "
                                       "refused, not truncated");
                    ++refused;
                    for (int k = rb; k < re; ++k) { plan[k] = 1; refusedRun[k] = 1; }
                }
            }
        }

        // 2. syllablesWylie's loop, carrying offsets
        struct Byte { char c; size_t off; };
        std::vector<Byte> cur;
        auto flush = [&] {
            size_t b = 0, e = cur.size();
            while (b < e && !std::isalnum((unsigned char)cur[b].c) && cur[b].c != '\'') ++b;
            while (e > b && !std::isalnum((unsigned char)cur[e - 1].c) && cur[e - 1].c != '\'') --e;
            std::string s;
            for (size_t k = b; k < e; ++k) {
                char c = cur[k].c;
                if (c >= 'A' && c <= 'Z') c = (char)(c - 'A' + 'a');
                s += c;
            }
            if (!s.empty() && s[0] != '@' &&
                s.find_first_not_of("0123456789") != std::string::npos) {
                Syllable y;
                y.text = s; y.begByte = cur[b].off; y.endByte = cur[e - 1].off + 1;
                y.line = lineNo;
                res.push_back(y);
            }
            cur.clear();
        };
        auto barrier = [&] { if (!res.empty()) res.back().barrierAfter = true; };

        for (size_t i = 0; i < L.size(); ++i) {
            if (plan[i] == 2) continue;
            const char c = plan[i] == 1 ? ' ' : L[i];
            if (isFlushByte(c)) { flush(); if (isBarrierByte(c)) barrier(); }
            else cur.push_back({c, base + i});
        }
        flush();
        if (lines[li].terminated) barrier();
    }

    if (out) {
        out->apparatus = apparatusName(mode);
        out->units_read += (long long)res.size();
        out->units_refused += refused;
        out->apparatus_marks_stripped += stripped;
        for (const auto& r : refusals) out->refusals.push_back(r);
    }
    return res;
}

int countParenSpans(const std::string& text) {
    int n = 0, depth = 0;
    for (char c : text) {
        if (c == '(') ++depth;
        else if (c == ')' && depth > 0) { --depth; ++n; }
    }
    return n;
}

// --------------------------------------------------------------- script
std::string toEwtsLine(const std::string& line, bool* ok) {
    if (ok) *ok = true;
    if (textutil::hasTibetanUnicode(line)) {
        const auto r = unicodeToWylie(line);
        if (r.warns > 0) { if (ok) *ok = false; return line; }
        return r.wylie;
    }
    bool upper = false, lower = false;
    for (unsigned char c : line) { if (c >= 'A' && c <= 'Z') upper = true; else if (c >= 'a' && c <= 'z') lower = true; }
    if (upper && !lower) return acipToEwts(line);
    return line;   // Wylie or English: already the common form
}

Converted toEwtsDocument(const std::string& text) {
    Converted c;
    const auto lines = docLines(text);
    for (size_t i = 0; i < lines.size(); ++i) {
        bool ok = true;
        const std::string w = toEwtsLine(lines[i].text, &ok);
        if (!ok) { ++c.unconvertedLines; c.unconvertedLineNumbers.push_back((int)i + 1); }
        if (i) c.wylie += '\n';
        c.wylie += w;
    }
    if (!lines.empty() && lines.back().terminated) c.wylie += '\n';
    return c;
}

// --------------------------------------------------------------- folios
namespace {
// The strict reading: a folio has a side letter. "@0012" is a marker to
// citeAt but not a folio to folioAt, and that divergence is deliberate.
std::string lastStrictFolio(const std::string& line) {
    const auto ms = folioMarkers(line);
    for (int i = (int)ms.size() - 1; i >= 0; --i) {
        int n = 0, w = 0; char side = 0;
        if (folioParts(ms[i].id, n, side, w) && side) return ms[i].id;
    }
    return std::string();
}
}  // namespace

FolioLoc folioAt(const std::string& text, size_t byteOffset) {
    FolioLoc r;
    const auto lines = docLines(text);
    if (lines.empty()) { r.lineInFolio = 1; return r; }   // as citeAt reads it
    const int here = lineIndexOf(lines, byteOffset);
    int at = -1;
    for (int i = here; i >= 0; --i) {
        const std::string id = lastStrictFolio(lines[i].text);
        if (!id.empty()) { at = i; r.folio = id; break; }
    }
    if (at < 0) { r.lineInFolio = here + 1; return r; }   // no folio: the absolute line
    r.lineInFolio = here - at + 1;
    int next = (int)lines.size();
    for (int i = at + 1; i < (int)lines.size(); ++i)
        if (!lastStrictFolio(lines[i].text).empty()) { next = i; break; }
    r.linesInFolio = next - at;
    return r;
}

std::string citeAtLine(const std::vector<std::string>& lines, int line0) {
    for (int i = std::min(line0, (int)lines.size() - 1); i >= 0; --i) {
        const std::string last = lastFolio(lines[i]);
        if (!last.empty()) return "@" + last + "." + std::to_string(line0 - i + 1);
    }
    return "line " + std::to_string(line0 + 1);
}

std::string citeAt(const std::string& text, size_t byteOffset) {
    const auto dl = docLines(text);
    std::vector<std::string> lines;
    lines.reserve(dl.size());
    for (const auto& l : dl) lines.push_back(l.text);
    return citeAtLine(lines, lineIndexOf(dl, byteOffset));
}

// Promoted verbatim from catalog_list.cpp's file-private folioRange, which
// now calls this; acipFolioRange's published behaviour is unchanged.
std::pair<std::string, std::string> folioRange(const std::string& text) {
    std::string first, last;
    for (size_t at = text.find('@'); at != std::string::npos;
         at = text.find('@', at + 1)) {
        size_t j = at + 1;
        while (j < text.size() && text[j] == ' ') ++j;
        size_t d = j;
        while (d < text.size() &&
               std::isdigit(static_cast<unsigned char>(text[d])))
            ++d;
        if (d == j || d >= text.size()) continue;
        const char side =
            static_cast<char>(std::toupper(static_cast<unsigned char>(text[d])));
        if (side != 'A' && side != 'B') continue;
        std::string f = text.substr(j, d - j);
        f.push_back(side);
        if (first.empty()) first = f;
        last = std::move(f);
    }
    return {first, last};
}

int countFolioMarkers(const std::string& text) {
    int n = 0;
    for (auto it = std::sregex_iterator(text.begin(), text.end(), folioRe());
         it != std::sregex_iterator(); ++it)
        ++n;
    return n;
}

int countShads(const std::string& text) {
    // docprops' own markRe, replicated exactly (app/main.cpp textStatistics)
    static const std::regex markRe("@\\S+|\\{[^}]*\\}|\\[[^\\]]*\\]");
    const std::string body = std::regex_replace(text, markRe, " ");
    int n = 0;
    for (char c : body) if (c == ',') ++n;
    return n;
}

}  // namespace textspan
}  // namespace allcore
