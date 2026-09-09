// allcore/textnorm.cpp — see textnorm.h. The rules run per line in one fixed
// order (classify → apparatus → folio markers → shad rules → collapse spaces
// → trim trailing), then the document-level blank-line, line-ending and
// final-newline rules. The order is fixed and documented because the rules are
// not commutative, and every rule is a fixed point of itself so that
// normalizing an already-normalized text changes nothing.
//
// The shad work is a codepoint walk, not a regex: std::regex has no lookbehind,
// so "a space before a shad" cannot be written as a pattern here, and the
// double shad has to be recognised as ONE atomic unit before anything spaces
// its edges.
#include "allcore/textnorm.h"

#include <cctype>
#include <string>
#include <vector>

#include "allcore/textspan.h"
#include "text_util.h"

namespace allcore::textnorm {

namespace {

// ------------------------------------------------------------ small tests
bool isSpaceTab(unsigned cp) { return cp == ' ' || cp == '\t'; }
bool isBlankLine(const std::string& s) {
    for (char c : s) if (c != ' ' && c != '\t') return false;
    return true;
}
// A shad, per the line's script. The tsheg U+0F0B is deliberately absent: it
// separates letters, not clauses, and belongs to the converters.
bool isShadCp(unsigned cp, LineClass cls) {
    if (cls == LineClass::Acip) return cp == ',';
    if (cls == LineClass::TibetanUnicode)
        return cp == 0x0F0D || cp == 0x0F0E || cp == 0x0F11 || cp == 0x0F14;
    return false;
}
// U+0F0E ༎ is already the double shad written as one character.
bool isDoubleShadCp(unsigned cp) { return cp == 0x0F0E; }

bool shadRuleAsked(const Options& o) {
    return !o.doubleShad.empty() || o.spaceBeforeShad != ShadSpace::Keep || o.spaceAfterShad;
}

// The chosen double-shad spelling belongs to one script. Writing ",," onto a
// Tibetan Unicode line (or "༎" onto an ACIP line) would be a script
// conversion, which this engine never does — so on the other script the
// respelling simply does not apply.
bool targetFitsClass(const std::string& t, LineClass cls) {
    if (t.empty()) return false;
    bool acip = false, tib = false;
    for (unsigned char c : t) { if (c == ',') acip = true; else if (c >= 0x80) tib = true; }
    if (cls == LineClass::Acip) return acip && !tib;
    if (cls == LineClass::TibetanUnicode) return tib && !acip;
    return false;
}

// ------------------------------------------------------------- shad walk
// The line as a sequence of items (ordinary text, or one shad token) with the
// [ \t] gaps between them held separately: the spacing rules act on the gaps,
// the respelling acts on the token, and neither can reach inside the other.
struct Item {
    bool shad = false;        // this token is a shad or a double shad
    bool rewritten = false;   // it is a double shad being respelled
    std::string text;
};

// What goes between L and R. Rules, in precedence order:
//  - a respelled double shad owns its own left edge (the chosen spelling is
//    the whole cluster's spelling, ", ," asked for as ",," is written ",,");
//  - otherwise "space before a shad" decides the gap in front of a shad, and
//    never indents a line whose first token is a shad;
//  - "one space after a shad" fills the gap behind a shad, but yields to an
//    explicit "no space before a shad" when two shads meet, and never adds a
//    trailing space at the end of a line.
std::string renderGap(const std::string& gap, const Item* L, const Item* R, const Options& o) {
    std::string g = gap;
    if (R && R->shad) {
        if (R->rewritten) {
            g = (o.spaceBeforeShad == ShadSpace::One && L) ? std::string(" ") : std::string();
        } else {
            switch (o.spaceBeforeShad) {
                case ShadSpace::Keep: break;
                case ShadSpace::None: g.clear(); break;
                case ShadSpace::One:  g = L ? std::string(" ") : std::string(); break;
            }
        }
    }
    if (L && L->shad && R && o.spaceAfterShad) {
        const bool beforeRuleOwnsIt = R->shad && (R->rewritten || o.spaceBeforeShad == ShadSpace::None);
        if (!beforeRuleOwnsIt) g = " ";
    }
    return g;
}

std::string applyShadRules(const std::string& line, const Options& o, LineClass cls) {
    const auto cps = textutil::codepoints(line);
    const bool respell = targetFitsClass(o.doubleShad, cls);
    std::vector<Item> items;
    std::vector<std::string> gaps(1);   // gaps[k] sits in front of items[k]
    std::string text;                   // the ordinary-text run being gathered

    auto byteAt = [&](size_t k) { return k < cps.size() ? (size_t)cps[k].first : line.size(); };
    auto flushText = [&]() {
        if (text.empty()) return;
        Item it; it.text = text; items.push_back(it); gaps.emplace_back(); text.clear();
    };

    for (size_t i = 0; i < cps.size();) {
        const unsigned cp = cps[i].second;
        const size_t b = byteAt(i), e = byteAt(i + 1);
        if (isSpaceTab(cp)) { flushText(); gaps.back().append(line, b, e - b); ++i; continue; }
        if (isShadCp(cp, cls)) {
            // The double shad is one atomic unit: ",,", ", ,", "," + spaces +
            // ",", "།།" — and "༎", which is one character already. A SINGLE
            // shad is never merged into a double.
            size_t j = i + 1;
            while (j < cps.size() && isSpaceTab(cps[j].second)) ++j;
            const bool pair = !isDoubleShadCp(cp) && j < cps.size() && cps[j].second == cp;
            const size_t last = pair ? j : i;
            const bool dbl = pair || isDoubleShadCp(cp);
            flushText();
            Item it;
            it.shad = true;
            it.rewritten = dbl && respell;
            it.text = it.rewritten ? o.doubleShad : line.substr(b, byteAt(last + 1) - b);
            items.push_back(it); gaps.emplace_back();
            i = last + 1;
            continue;
        }
        text.append(line, b, e - b);
        ++i;
    }
    flushText();

    std::string out;
    for (size_t k = 0; k <= items.size(); ++k) {
        const Item* L = k > 0 ? &items[k - 1] : nullptr;
        const Item* R = k < items.size() ? &items[k] : nullptr;
        out += renderGap(gaps[k], L, R, o);
        if (R) out += R->text;
    }
    return out;
}

// ------------------------------------------------------- whitespace rules
// Runs of [ \t] become one space. Only ASCII space and tab are touched, so
// every U+0F0B tsheg (and every other multi-byte character) passes through
// byte for byte. Leading and trailing runs become one space too — trimming is
// the next rule's job, kept separate so the two switches stay independent.
std::string collapseSpacesAndTabs(const std::string& s) {
    std::string o; bool run = false;
    o.reserve(s.size());
    for (char c : s) {
        if (c == ' ' || c == '\t') { run = true; continue; }
        if (run) o += ' ';
        run = false; o += c;
    }
    if (run) o += ' ';
    return o;
}
std::string trimTrailingWs(const std::string& s) {
    size_t n = s.size();
    while (n > 0 && (s[n - 1] == ' ' || s[n - 1] == '\t')) --n;
    return s.substr(0, n);
}

std::string eolName(const std::string& e) {
    if (e == "\r\n") return "CRLF";
    if (e == "\n") return "LF";
    if (e == "\r") return "CR";
    return "none";
}

}  // namespace

// ------------------------------------------------------------- classifier
// Tibetan Unicode wins on sight. Otherwise the case of the WORDS decides: an
// ACIP-looking word is a run of two or more upper-case letters, and a lone
// capital does not count — it is a sentence's capital far more often than it
// is an ACIP syllable, and counting it would refuse ordinary English prose as
// "mixed script". A line carrying an ACIP-looking word AND any lower-case
// letter is Mixed, and Mixed is refused rather than guessed at.
LineClass classifyLine(const std::string& line) {
    if (textutil::hasTibetanUnicode(line)) return LineClass::TibetanUnicode;
    bool acipWord = false, anyLower = false;
    int upperRun = 0;
    for (unsigned char c : line) {
        if (c >= 'A' && c <= 'Z') { if (++upperRun >= 2) acipWord = true; continue; }
        upperRun = 0;
        if (c >= 'a' && c <= 'z') anyLower = true;
    }
    if (acipWord && anyLower) return LineClass::Mixed;
    if (acipWord) return LineClass::Acip;
    return LineClass::Other;
}

bool bracketsBalanced(const std::string& line, char open, char close) {
    int depth = 0;
    for (char c : line) {
        if (c == open) ++depth;
        else if (c == close && --depth < 0) return false;
    }
    return depth == 0;
}

std::string detectEol(const std::string& text) {
    for (size_t i = 0; i < text.size(); ++i) {
        if (text[i] == '\r') return (i + 1 < text.size() && text[i + 1] == '\n') ? "\r\n" : "\r";
        if (text[i] == '\n') return "\n";
    }
    return "";   // no line break at all: there is nothing to keep
}

// ------------------------------------------------------------- the line
std::string normalizeLineText(const std::string& line, const Options& o,
                              LineClass* cls, std::string* refuseReason) {
    if (refuseReason) refuseReason->clear();
    const LineClass c = classifyLine(line);       // 1. classify (the line as given)
    if (cls) *cls = c;
    // A refusal returns the ORIGINAL line, whatever an earlier rule had already
    // done to the working copy: a refused line is byte-identical, always.
    auto refuse = [&](const char* why) -> std::string {
        if (refuseReason) *refuseReason = why;
        return line;
    };
    std::string s = line;

    // 2. Apparatus. Only when BOTH bracket kinds close on this line: a note
    // wrapped across two lines is not a span, and half of one must not be cut.
    if (o.stripApparatus) {
        if (!bracketsBalanced(s, '[', ']')) return refuse("unbalanced [ ]");
        if (!bracketsBalanced(s, '{', '}')) return refuse("unbalanced { }");
        bool ok = true;
        const auto spans = textspan::apparatusSpans(s, &ok);
        // The counts can balance while the spans still cannot be read —
        // "[a {b] c}" interleaves, "[[a]]" nests. Neither is guessed at.
        if (!ok) return refuse("nested or overlapping apparatus");
        if (!spans.empty()) s = textspan::stripApparatus(s);   // removal collapses its own gap
    }

    // 3. Folio markers, through the one marker definition (textspan).
    if (o.stripFolioMarkers) {
        const auto ms = textspan::folioMarkers(s);
        for (size_t i = 0; i + 1 < s.size(); ++i) {
            if (s[i] != '@' || !std::isdigit((unsigned char)s[i + 1])) continue;
            bool known = false;
            for (const auto& m : ms) if (m.begin == (int)i) { known = true; break; }
            // "@001AX": marker-shaped but glued to text. Cutting the part that
            // looks like a marker would invent a folio boundary.
            if (!known) return refuse("marker glued to text");
        }
        if (!ms.empty()) {
            std::string t; int pos = 0;
            for (const auto& m : ms) { t.append(s, pos, m.begin - pos); pos = m.end; }
            t.append(s, pos, std::string::npos);
            s = textutil::collapseWs(t);   // a removal leaves doubled spaces
        }
    }

    // 4. Shad rules — ACIP and Tibetan Unicode lines only.
    if (shadRuleAsked(o)) {
        if (c == LineClass::Mixed) return refuse("mixed script");
        if (c == LineClass::Acip || c == LineClass::TibetanUnicode) s = applyShadRules(s, o, c);
        // Other (English, Wylie, digits): a comma is a comma there. The caller
        // counts these as shadRulesSkipped.
    }

    if (o.collapseSpaces) s = collapseSpacesAndTabs(s);   // 5.
    if (o.trimTrailing)   s = trimTrailingWs(s);          // 6.
    return s;
}

// --------------------------------------------------------- the document
Result normalizeText(const std::vector<std::string>& lines,
                     const std::string& detectedEol, const Options& o) {
    Result r;
    const bool shadAsked = shadRuleAsked(o);

    std::vector<std::string> out;
    out.reserve(lines.size());
    for (size_t i = 0; i < lines.size(); ++i) {
        LineClass c = LineClass::Other;
        std::string why;
        std::string s = normalizeLineText(lines[i], o, &c, &why);
        if (!why.empty()) r.refused.push_back({(int)i, why});
        else if (shadAsked && c == LineClass::Other) ++r.shadRulesSkipped;
        if (s != lines[i]) ++r.changedLines;
        out.push_back(std::move(s));
    }

    // Blank-line runs: the ONE rule that may change the line count. -1 keeps
    // them all, 0 removes them all. Refusal line numbers above are input line
    // numbers and are not moved by this.
    int blanksRemoved = 0;
    if (o.maxBlankLines >= 0) {
        std::vector<std::string> keep;
        keep.reserve(out.size());
        int run = 0;
        for (auto& s : out) {
            if (isBlankLine(s)) {
                if (++run > o.maxBlankLines) { ++blanksRemoved; continue; }
            } else run = 0;
            keep.push_back(std::move(s));
        }
        out.swap(keep);
    }
    r.lines = std::move(out);

    switch (o.eol) {
        case Eol::Keep: r.eol = detectedEol; break;
        case Eol::LF:   r.eol = "\n"; break;
        case Eol::CRLF: r.eol = "\r\n"; break;
        case Eol::CR:   r.eol = "\r"; break;
    }

    // splitLines() cannot see whether the text it came from ended with a line
    // break (both "a\nb" and "a\nb\n" give {a,b}), so this flag says the rule
    // GUARANTEES a final line break, not that one byte was added. The caller
    // holding the raw text is the one that can honestly call it a change.
    r.finalNewlineAdded = o.ensureFinalNewline && !r.lines.empty();

    if (!detectedEol.empty() && !r.eol.empty() && r.eol != detectedEol)
        r.notes.push_back("line endings " + eolName(detectedEol) + " → " + eolName(r.eol));
    if (blanksRemoved > 0)
        r.notes.push_back(std::to_string(blanksRemoved) + " blank line(s) removed");
    if (!r.refused.empty())
        r.notes.push_back(std::to_string((int)r.refused.size()) + " line(s) left alone (listed)");
    return r;
}

}  // namespace allcore::textnorm
