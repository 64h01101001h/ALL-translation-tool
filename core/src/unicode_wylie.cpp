// unicode_wylie.cpp — see allcore/unicode_wylie.h. A faithful
// transcription of pyewts.py::toWylie and its helpers; the tables are
// generated from the module itself. Structure and order mirror the
// original so the two can be diffed function by function.
#include "allcore/unicode_wylie.h"

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace allcore {
namespace {

#include "unicode_wylie_tables.inc"

const std::string* mapGet(
    const std::unordered_map<std::string, std::string>& m,
    const std::string& k) {
    auto it = m.find(k);
    return it == m.end() ? nullptr : &it->second;
}

const std::string* tibTop(const std::string& c) { return mapGet(kTibTop, c); }
const std::string* tibSubjoined(const std::string& c) { return mapGet(kTibSubjoined, c); }
const std::string* tibVowel(const std::string& c) { return mapGet(kTibVowel, c); }
const std::string* tibVowelLong(const std::string& s) { return mapGet(kTibVowelLong, s); }
const std::string* tibFinalWylie(const std::string& c) { return mapGet(kTibFinalWylie, c); }
const std::string* tibFinalClass(const std::string& c) { return mapGet(kTibFinalClass, c); }
const std::string* tibCaret(const std::string& s) { return mapGet(kTibCaret, s); }
const std::string* tibOther(const std::string& c) { return mapGet(kTibOther, c); }
bool tibStack(const std::string& s) { return kTibStacks.count(s) != 0; }
bool isPrefixPair(const std::string& pref, const std::string& after) {
    return kPrefixPairs.count(pref + "|" + after) != 0;
}
bool isSuffix(const std::string& s) { return kSuffixes.count(s) != 0; }
bool isSuff2Pair(const std::string& suff, const std::string& before) {
    return kSuff2Pairs.count(suff + "|" + before) != 0;
}

// ---- codepoint utilities: the original walks python chars; we walk
// a decoded vector of single-codepoint UTF-8 strings + their values
struct CpText {
    std::vector<std::string> cp;   // UTF-8 per codepoint
    std::vector<uint32_t> val;
};

CpText decode(const std::string& s) {
    CpText t;
    size_t i = 0;
    while (i < s.size()) {
        const unsigned char b = s[i];
        size_t n = b < 0x80 ? 1 : (b >> 5) == 6 ? 2 : (b >> 4) == 14 ? 3 : 4;
        if (i + n > s.size()) n = 1;
        uint32_t v = 0;
        if (n == 1) v = b;
        else if (n == 2) v = ((b & 0x1F) << 6) | (s[i+1] & 0x3F);
        else if (n == 3) v = ((b & 0x0F) << 12) | ((s[i+1] & 0x3F) << 6) | (s[i+2] & 0x3F);
        else v = ((b & 0x07) << 18) | ((s[i+1] & 0x3F) << 12) | ((s[i+2] & 0x3F) << 6) | (s[i+3] & 0x3F);
        t.cp.push_back(s.substr(i, n));
        t.val.push_back(v);
        i += n;
    }
    return t;
}

std::string encodeCp(uint32_t v) {
    std::string o;
    if (v < 0x80) o += char(v);
    else if (v < 0x800) {
        o += char(0xC0 | (v >> 6));
        o += char(0x80 | (v & 0x3F));
    } else if (v < 0x10000) {
        o += char(0xE0 | (v >> 12));
        o += char(0x80 | ((v >> 6) & 0x3F));
        o += char(0x80 | (v & 0x3F));
    } else {
        o += char(0xF0 | (v >> 18));
        o += char(0x80 | ((v >> 12) & 0x3F));
        o += char(0x80 | ((v >> 6) & 0x3F));
        o += char(0x80 | (v & 0x3F));
    }
    return o;
}

std::string formatHex(uint32_t v) {
    char b[12];
    std::snprintf(b, sizeof b, "\\u%04x", v);
    return b;
}

std::string joinStrings(const std::vector<std::string>& a,
                        const char* sep) {
    std::string o;
    for (size_t i = 0; i < a.size(); ++i) {
        if (i) o += sep;
        o += a[i];
    }
    return o;
}

// ---- the stack/tsekbar structures, as in the original
struct ToWylieStack {
    std::string top;
    std::vector<std::string> stack;
    bool caret = false;
    std::vector<std::string> vowels;
    std::vector<std::string> finals;
    std::unordered_map<std::string, std::string> finals_found;
    bool visarga = false;
    std::string cons_str;
    bool has_single_cons = false;
    std::string single_cons;
    bool prefix = false, suffix = false, suff2 = false, dot = false;
    int tokens_used = 0;
    int warns = 0;
};

struct ToWylieTsekbar {
    std::string wylie;
    int tokens_used = 0;
    int warns = 0;
};

ToWylieStack toWylieOneStack(const CpText& in, size_t lenstr, size_t i) {
    const size_t orig_i = i;
    bool have_final = false;
    bool have_vowel = false;
    ToWylieStack st;
    const std::string t0 = in.cp[i];
    ++i;
    if (const auto* top = tibTop(t0)) {
        st.top = *top;
        st.stack.push_back(st.top);
    }
    while (i < lenstr) {
        const std::string t = in.cp[i];
        if (const auto* o = tibSubjoined(t)) {
            ++i;
            st.stack.push_back(*o);
            if (!st.finals.empty()) ++st.warns;
            else if (!st.vowels.empty()) ++st.warns;
        } else if (const auto* o2 = tibVowel(t)) {
            ++i;
            st.vowels.push_back(*o2);
            have_vowel = true;
            if (!st.finals.empty()) ++st.warns;
        } else if (const auto* o3 = tibFinalWylie(t)) {
            const std::string klass = *tibFinalClass(t);
            ++i;
            if (*o3 == "^") {
                st.caret = true;
            } else {
                if (*o3 == "H") st.visarga = true;
                st.finals.push_back(*o3);
                have_final = true;
                if (st.finals_found.count(klass)) ++st.warns;
                else st.finals_found[klass] = *o3;
            }
        } else {
            break;
        }
    }
    (void)have_final;
    (void)have_vowel;
    // top of stack is a "a" with vowel signs: drop the "a"
    if (st.top == "a" && st.stack.size() == 1 && !st.vowels.empty())
        st.stack.erase(st.stack.begin());
    // long vowels: "A" + vowel => the long form
    if (st.vowels.size() > 1 && st.vowels[0] == "A") {
        if (const auto* l = tibVowelLong(st.vowels[1])) {
            const std::string lv = *l;
            st.vowels.erase(st.vowels.begin());
            st.vowels.erase(st.vowels.begin());
            st.vowels.insert(st.vowels.begin(), lv);
        }
    }
    // caret on a lone consonant with a caret form (tsa-phru letters)
    if (st.caret && st.stack.size() == 1) {
        if (const auto* l = tibCaret(st.top)) {
            st.top = *l;
            st.stack.erase(st.stack.begin());
            st.stack.insert(st.stack.begin(), *l);
            st.caret = false;
        }
    }
    st.cons_str = joinStrings(st.stack, "+");
    if (st.stack.size() == 1 && st.stack[0] != "a" && !st.caret &&
        st.vowels.empty() && st.finals.empty()) {
        st.has_single_cons = true;
        st.single_cons = st.cons_str;
    }
    st.tokens_used = int(i - orig_i);
    return st;
}

std::string replaceAll(std::string s, const std::string& a,
                       const std::string& b) {
    size_t p = 0;
    while ((p = s.find(a, p)) != std::string::npos) {
        s.replace(p, a.size(), b);
        p += b.size();
    }
    return s;
}

std::string putStackTogether(const ToWylieStack& st) {
    std::string out;
    if (tibStack(st.cons_str))
        out += joinStrings(st.stack, "");
    else
        out += st.cons_str;
    if (st.caret) out += "^";
    if (!st.vowels.empty())
        out += joinStrings(st.vowels, "+");
    else if (!st.prefix && !st.suffix && !st.suff2 &&
             (st.cons_str.empty() || st.cons_str.back() != 'a'))
        out += "a";
    out += joinStrings(st.finals, "");
    if (st.dot) out += ".";
    return out;
}

ToWylieTsekbar toWylieOneTsekbar(const CpText& in, size_t lenstr,
                                 size_t i) {
    const size_t orig_i = i;
    int warns = 0;
    std::vector<ToWylieStack> stacks;
    while (true) {
        ToWylieStack st = toWylieOneStack(in, lenstr, i);
        warns += st.warns;
        i += st.tokens_used;
        const bool visarga = st.visarga;
        stacks.push_back(std::move(st));
        if (visarga) break;
        if (i >= lenstr || tibTop(in.cp[i]) == nullptr) break;
    }
    const size_t last = stacks.size() - 1;
    if (stacks.size() > 1 && stacks[0].has_single_cons) {
        const std::string cs = replaceAll(stacks[1].cons_str, "+w", "");
        if (isPrefixPair(stacks[0].single_cons, cs))
            stacks[0].prefix = true;
    }
    if (stacks.size() > 1 && stacks[last].has_single_cons &&
        isSuffix(stacks[last].single_cons))
        stacks[last].suffix = true;
    if (stacks.size() > 2 && stacks[last].has_single_cons &&
        stacks[last - 1].has_single_cons &&
        isSuffix(stacks[last - 1].single_cons) &&
        isSuff2Pair(stacks[last].single_cons,
                    stacks[last - 1].single_cons)) {
        stacks[last].suff2 = true;
        stacks[last - 1].suffix = true;
    }
    if (stacks.size() == 2 && stacks[0].prefix && stacks[1].suffix)
        stacks[0].prefix = false;
    if (stacks.size() == 3 && stacks[0].prefix && stacks[1].suffix &&
        stacks[2].suff2) {
        std::string ztr;
        for (const auto& st : stacks) ztr += st.single_cons;
        int root = 1;
        auto it = kAmbiguousKey.find(ztr);
        if (it == kAmbiguousKey.end())
            ++warns;   // ambiguous syllable: root unknown
        else
            root = it->second;
        stacks[root].prefix = stacks[root].suffix = false;
        stacks[root + 1].suff2 = false;
    }
    if (stacks[0].prefix &&
        tibStack(stacks[0].single_cons + "+" + stacks[1].cons_str))
        stacks[0].dot = true;
    std::string out;
    for (const auto& st : stacks) out += putStackTogether(st);
    ToWylieTsekbar ret;
    ret.wylie = out;
    ret.tokens_used = int(i - orig_i);
    ret.warns = warns;
    return ret;
}

bool followedByNonTibetan(const CpText& in, size_t i) {
    const size_t lenstr = in.cp.size();
    while (i < lenstr && in.cp[i] == " ") ++i;
    if (i == lenstr) return false;
    const std::string& t = in.cp[i];
    return tibTop(t) == nullptr && tibOther(t) == nullptr &&
           t != "\r" && t != "\n";
}

}  // namespace

UnicodeToWylieResult unicodeToWylie(const std::string& unicode_utf8) {
    // the original's precomposed-sign replacements, in its order
    std::string src = unicode_utf8;
    src = replaceAll(src, encodeCp(0x0f76), encodeCp(0x0fb2) + encodeCp(0x0f80));
    src = replaceAll(src, encodeCp(0x0f77), encodeCp(0x0fb2) + encodeCp(0x0f71) + encodeCp(0x0f80));
    src = replaceAll(src, encodeCp(0x0f78), encodeCp(0x0fb3) + encodeCp(0x0f80));
    src = replaceAll(src, encodeCp(0x0f79), encodeCp(0x0fb3) + encodeCp(0x0f71) + encodeCp(0x0f80));
    src = replaceAll(src, encodeCp(0x0f81), encodeCp(0x0f71) + encodeCp(0x0f80));
    src = replaceAll(src, encodeCp(0x0F75), encodeCp(0x0F71) + encodeCp(0x0F74));
    src = replaceAll(src, encodeCp(0x0F73), encodeCp(0x0F71) + encodeCp(0x0F72));

    const CpText in = decode(src);
    const size_t lenstr = in.cp.size();
    UnicodeToWylieResult res;
    std::string& out = res.wylie;
    size_t i = 0;
    const bool escape = true;   // pyewts default; the battery runs it
    while (i < lenstr) {
        const std::string t = in.cp[i];
        if (tibTop(t) != nullptr) {
            ToWylieTsekbar tb = toWylieOneTsekbar(in, lenstr, i);
            out += tb.wylie;
            i += tb.tokens_used;
            res.warns += tb.warns;
            continue;
        }
        const std::string* o = tibOther(t);
        if (o != nullptr &&
            (t != " " || (escape && !followedByNonTibetan(in, i)))) {
            out += *o;
            ++i;
            continue;
        }
        if (t == "\r" || t == "\n") {
            ++i;
            out += t;
            if (t == "\r" && i < lenstr && in.cp[i] == "\n") {
                ++i;
                out += "\n";
            }
            continue;
        }
        if (in.val[i] == 0xfeff || in.val[i] == 0x200b) {
            ++i;
            continue;
        }
        if (in.val[i] >= 0x0f00 && in.val[i] <= 0x0fff) {
            out += formatHex(in.val[i]);
            const std::string cur = t;
            ++i;
            if (tibSubjoined(cur) || tibVowel(cur) || tibFinalWylie(cur))
                ++res.warns;   // sign needs a top symbol
            continue;
        }
        out += "[";
        std::string tt = t;
        size_t vi = i;
        while (tibTop(tt) == nullptr &&
               (tibOther(tt) == nullptr || tt == " ") && tt != "\r" &&
               tt != "\n") {
            if (tt == "[" || tt == "]") {
                out += "\\";
                out += tt;
            } else if (in.val[vi] >= 0x0f00 && in.val[vi] <= 0x0fff) {
                out += formatHex(in.val[vi]);
            } else {
                out += tt;
            }
            ++i;
            ++vi;
            if (i >= lenstr) break;
            tt = in.cp[i];
        }
        out += "]";
    }
    return res;
}

}  // namespace allcore
