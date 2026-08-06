// Port of engines/ewts_unicode.py (v0.27.1) — wylie → Tibetan unicode.
// Transcribed table-for-table and branch-for-branch from the canonical file;
// proven by the 26,318-pair ground-truth battery (engines_battery test).
#include <algorithm>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "allcore/engines.h"

namespace allcore {
namespace {

using std::map;
using std::set;
using std::string;
using std::vector;

// IAST → EWTS normalization (HGM wylie fields use IAST diacritics)
const std::pair<const char*, const char*> IAST[] = {
    {"ā", "A"},  {"ī", "I"},  {"ū", "U"},  {"ṃ", "M"},
    {"ṁ", "M"},  {"ḥ", "H"},  {"ṇ", "N"},  {"ṭ", "T"},
    {"ḍ", "D"},  {"ṣ", "Sh"}, {"ś", "sh"}, {"ñ", "ny"},
    {"ṅ", "ng"}, {"ṛ", "r-i"},{"ṝ", "r-I"},{"ḷ", "l-i"},
    {"ḹ", "l-I"},
    // uppercase forms (python: replace(k.upper(), v))
    {"Ā", "A"},  {"Ī", "I"},  {"Ū", "U"},  {"Ṃ", "M"},
    {"Ṁ", "M"},  {"Ḥ", "H"},  {"Ṇ", "N"},  {"Ṭ", "T"},
    {"Ḍ", "D"},  {"Ṣ", "Sh"}, {"Ś", "sh"}, {"Ñ", "ny"},
    {"Ṅ", "ng"}, {"Ṛ", "r-i"},{"Ṝ", "r-I"},{"Ḷ", "l-i"},
    {"Ḹ", "l-I"},
};

const map<string, string>& consMap() {
    static const map<string, string> m = {
        {"k", "ཀ"},  {"kh", "ཁ"},  {"g", "ག"},  {"ng", "ང"},  {"c", "ཅ"},
        {"ch", "ཆ"}, {"j", "ཇ"},   {"ny", "ཉ"}, {"T", "ཊ"},   {"Th", "ཋ"},
        {"D", "ཌ"},  {"N", "ཎ"},   {"t", "ཏ"},  {"th", "ཐ"},  {"d", "ད"},
        {"n", "ན"},  {"p", "པ"},   {"ph", "ཕ"}, {"b", "བ"},   {"m", "མ"},
        {"ts", "ཙ"}, {"tsh", "ཚ"}, {"dz", "ཛ"}, {"w", "ཝ"},   {"zh", "ཞ"},
        {"z", "ཟ"},  {"'", "འ"},   {"y", "ཡ"},  {"r", "ར"},   {"l", "ལ"},
        {"sh", "ཤ"}, {"Sh", "ཥ"},  {"s", "ས"},  {"h", "ཧ"},   {"gh", "གྷ"},
        {"dh", "དྷ"}, {"bh", "བྷ"}, {"dzh", "ཛྷ"}, {"kSh", "ཀྵ"},
    };
    return m;
}

const map<string, string>& subMap() {
    static const map<string, string> m = {
        {"k", "ྐ"},  {"kh", "ྑ"},  {"g", "ྒ"},  {"ng", "ྔ"},  {"c", "ྕ"},
        {"ch", "ྖ"}, {"j", "ྗ"},   {"ny", "ྙ"}, {"T", "ྚ"},   {"Th", "ྛ"},
        {"D", "ྜ"},  {"N", "ྞ"},   {"t", "ྟ"},  {"th", "ྠ"},  {"d", "ྡ"},
        {"n", "ྣ"},  {"p", "ྤ"},   {"ph", "ྥ"}, {"b", "ྦ"},   {"m", "ྨ"},
        {"ts", "ྩ"}, {"tsh", "ྪ"}, {"dz", "ྫ"}, {"w", "ྭ"},   {"zh", "ྮ"},
        {"z", "ྯ"},  {"'", "ྰ"},   {"y", "ྱ"},  {"r", "ྲ"},   {"l", "ླ"},
        {"sh", "ྴ"}, {"Sh", "ྵ"},  {"s", "ྶ"},  {"h", "ྷ"},   {"gh", "ྒྷ"},
        {"dh", "ྡྷ"}, {"bh", "ྦྷ"}, {"dzh", "ྫྷ"}, {"kSh", "ྐྵ"},
    };
    return m;
}

const map<string, string>& vowMap() {
    static const map<string, string> m = {
        {"a", ""},   {"i", "ི"},   {"u", "ུ"},   {"e", "ེ"},   {"o", "ོ"},
        {"A", "ཱ"},  {"I", "ཱི"},   {"U", "ཱུ"},   {"-i", "ྀ"},  {"-I", "ཱྀ"},
        {"ai", "ཻ"}, {"au", "ཽ"},
    };
    return m;
}

const map<string, string>& finalsMap() {
    static const map<string, string> m = {{"M", "ཾ"}, {"H", "ཿ"}};
    return m;
}

const char* A_CHEN = "ཨ";

vector<string> sortedKeysDesc(const map<string, string>& m) {
    vector<string> k;
    for (auto& kv : m) k.push_back(kv.first);
    std::sort(k.begin(), k.end(), [](const string& a, const string& b) {
        if (a.size() != b.size()) return a.size() > b.size();
        return a < b;
    });
    return k;
}

const set<string> PRE = {"g", "d", "b", "m", "'"};
const map<string, set<string>> SUBJOINABLE = {
    {"y", {"k", "kh", "g", "p", "ph", "b", "m", "h"}},
    {"r", {"k", "kh", "g", "t", "th", "d", "p", "ph", "b", "m", "s", "h"}},
    {"l", {"k", "g", "b", "r", "s", "z"}},
    {"w", {"k", "kh", "g", "c", "ny", "t", "d", "ts", "tsh", "zh", "z", "r", "l",
           "sh", "s", "h"}},
};
const map<string, set<string>> SUPERSTACK = {
    {"r", {"k", "g", "ng", "j", "ny", "t", "d", "n", "b", "m", "ts", "dz"}},
    {"l", {"k", "g", "ng", "c", "j", "t", "d", "p", "b", "h"}},
    {"s", {"k", "g", "ng", "ny", "t", "d", "n", "p", "b", "m", "ts"}},
};

bool inSet(const map<string, set<string>>& m, const string& key, const string& v) {
    auto it = m.find(key);
    return it != m.end() && it->second.count(v) > 0;
}

string normIast(string s) {
    for (auto& [from, to] : IAST) {
        size_t pos = 0;
        const string f = from, t = to;
        while ((pos = s.find(f, pos)) != string::npos) {
            s.replace(pos, f.size(), t);
            pos += t.size();
        }
    }
    return s;
}

// token kinds: C(letter) / V(vowel) / PLUS / DOT
struct Token {
    enum Kind { C, V, PLUS, DOT } kind;
    string v;
};

bool tokenize(const string& syl, vector<Token>& out) {
    static const auto CONS_KEYS = sortedKeysDesc(consMap());
    static const auto VOW_KEYS = sortedKeysDesc(vowMap());
    out.clear();
    size_t i = 0;
    while (i < syl.size()) {
        char ch = syl[i];
        if (ch == '+') { out.push_back({Token::PLUS, "+"}); ++i; continue; }
        if (ch == '.') { out.push_back({Token::DOT, "."}); ++i; continue; }
        if (ch == 'M' || ch == 'H') {
            out.push_back({Token::C, string(1, ch)});
            ++i;
            continue;
        }
        string v, c;
        for (const auto& k : VOW_KEYS)
            if (syl.compare(i, k.size(), k) == 0) { v = k; break; }
        for (const auto& k : CONS_KEYS)
            if (syl.compare(i, k.size(), k) == 0) { c = k; break; }
        if (ch == 'a') {                      // inherent a (but not ai/au)
            if (v == "ai" || v == "au") {
                out.push_back({Token::V, v});
                i += v.size();
                continue;
            }
            out.push_back({Token::V, "a"});
            ++i;
            continue;
        }
        if (!v.empty() && (c.empty() || v.size() > c.size())) {
            out.push_back({Token::V, v});
            i += v.size();
        } else if (!c.empty()) {
            out.push_back({Token::C, c});
            i += c.size();
        } else {
            return false;
        }
    }
    return true;
}

// native Tibetan onset cluster → unicode, or nullopt
bool nativeOnset(const vector<string>& ons, bool dotted, string& u) {
    const auto& CONS = consMap();
    const auto& SUB = subMap();
    const size_t n = ons.size();
    if (n == 1) { u = CONS.at(ons[0]); return true; }
    if (n == 2) {
        const string &a = ons[0], &b = ons[1];
        if (dotted) { u = CONS.at(a) + CONS.at(b); return true; }
        if (inSet(SUPERSTACK, a, b)) { u = CONS.at(a) + SUB.at(b); return true; }
        if (inSet(SUBJOINABLE, b, a)) { u = CONS.at(a) + SUB.at(b); return true; }
        if (PRE.count(a)) { u = CONS.at(a) + CONS.at(b); return true; }
        return false;
    }
    if (n == 3) {
        const string &a = ons[0], &b = ons[1], &c = ons[2];
        if (inSet(SUPERSTACK, a, b) && inSet(SUBJOINABLE, c, b)) {
            u = CONS.at(a) + SUB.at(b) + SUB.at(c);
            return true;
        }
        if (PRE.count(a) && inSet(SUPERSTACK, b, c)) {
            u = CONS.at(a) + CONS.at(b) + SUB.at(c);
            return true;
        }
        if (PRE.count(a) && inSet(SUBJOINABLE, c, b)) {
            u = CONS.at(a) + CONS.at(b) + SUB.at(c);
            return true;
        }
        return false;
    }
    if (n == 4) {
        const string &a = ons[0], &b = ons[1], &c = ons[2], &d = ons[3];
        if (PRE.count(a) && inSet(SUPERSTACK, b, c) && inSet(SUBJOINABLE, d, c)) {
            u = CONS.at(a) + CONS.at(b) + SUB.at(c) + SUB.at(d);
            return true;
        }
    }
    return false;
}

bool sylToUni(const string& syl, string& out) {
    const auto& CONS = consMap();
    const auto& SUB = subMap();
    const auto& VOW = vowMap();
    const auto& FIN = finalsMap();
    vector<Token> t;
    if (!tokenize(syl, t) || t.empty()) return false;

    bool hasPlus = false;
    for (auto& x : t) hasPlus |= (x.kind == Token::PLUS);
    if (hasPlus) {
        // explicit stacks
        out.clear();
        vector<string> stack;
        bool expect = true;
        for (size_t i = 0; i < t.size(); ++i) {
            const Token& x = t[i];
            if (x.kind == Token::PLUS || x.kind == Token::DOT) continue;
            if (x.kind == Token::C) {
                const bool joined = (i > 0 && t[i - 1].kind == Token::PLUS);
                if (expect || joined) {
                    stack.push_back(x.v);
                    if (!(i + 1 < t.size() && t[i + 1].kind == Token::PLUS))
                        expect = false;
                } else {
                    auto it = CONS.find(x.v);
                    if (it != CONS.end()) out += it->second;
                }
            } else {  // vowel
                if (!stack.empty()) {
                    out += CONS.at(stack[0]);
                    for (size_t s = 1; s < stack.size(); ++s) out += SUB.at(stack[s]);
                    stack.clear();
                } else if (out.empty()) {
                    out += A_CHEN;
                }
                out += VOW.at(x.v);
                expect = false;
            }
        }
        if (!stack.empty()) {
            out += CONS.at(stack[0]);
            for (size_t s = 1; s < stack.size(); ++s) out += SUB.at(stack[s]);
        }
        return true;
    }

    // group into CV units; trailing consonants = suffixes
    struct Group {
        vector<string> ons;
        string vowel;
        bool dotted = false;
    };
    vector<Group> groups;
    vector<string> cur;
    bool dotted = false;
    for (const auto& x : t) {
        if (x.kind == Token::DOT) { dotted = true; continue; }
        if (x.kind == Token::C) {
            cur.push_back(x.v);
        } else {
            groups.push_back({cur, x.v, dotted});
            cur.clear();
            dotted = false;
        }
    }
    const vector<string> suffixes = cur;
    if (groups.empty()) return false;

    out.clear();
    for (size_t gi = 0; gi < groups.size(); ++gi) {
        const auto& g = groups[gi];
        if (g.ons.empty()) {
            if (gi == 0) out += A_CHEN;
            out += VOW.at(g.vowel);
        } else if (gi == 0) {
            string u;
            if (!nativeOnset(g.ons, g.dotted, u)) {
                // Sanskrit-style stack fallback
                bool ok = CONS.count(g.ons[0]) > 0;
                for (size_t s = 1; s < g.ons.size(); ++s) ok &= SUB.count(g.ons[s]) > 0;
                if (!ok) return false;
                u = CONS.at(g.ons[0]);
                for (size_t s = 1; s < g.ons.size(); ++s) u += SUB.at(g.ons[s]);
            }
            out += u + VOW.at(g.vowel);
        } else {
            // medial cluster: first letter full form, rest subjoined
            if (!CONS.count(g.ons[0])) return false;
            for (size_t s = 1; s < g.ons.size(); ++s)
                if (!SUB.count(g.ons[s])) return false;
            out += CONS.at(g.ons[0]);
            for (size_t s = 1; s < g.ons.size(); ++s) out += SUB.at(g.ons[s]);
            out += VOW.at(g.vowel);
        }
    }
    for (const auto& s : suffixes) {
        if (FIN.count(s)) out += FIN.at(s);
        else if (CONS.count(s)) out += CONS.at(s);
        else return false;
    }
    return true;
}

}  // namespace

std::pair<std::string, bool> wylieToUnicode(const std::string& wylie) {
    string w = normIast(wylie);
    // trim
    size_t b = w.find_first_not_of(" \t\r\n");
    if (b == string::npos) return {"", false};
    size_t e = w.find_last_not_of(" \t\r\n");
    w = w.substr(b, e - b + 1);
    // ellipses → space
    for (const char* ell : {"...", "…"}) {
        size_t pos = 0;
        const string f = ell;
        while ((pos = w.find(f, pos)) != string::npos) w.replace(pos, f.size(), " ");
    }
    if (w.empty()) return {"", false};

    // split keeping delimiters: whitespace runs, ',', '/', ';'
    vector<string> parts;
    string cur;
    for (size_t i = 0; i < w.size(); ++i) {
        char c = w[i];
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
            if (!cur.empty()) { parts.push_back(cur); cur.clear(); }
            continue;   // whitespace delimiter itself is skipped downstream
        }
        if (c == ',' || c == '/' || c == ';') {
            if (!cur.empty()) { parts.push_back(cur); cur.clear(); }
            parts.push_back(string(1, c));
            continue;
        }
        cur += c;
    }
    if (!cur.empty()) parts.push_back(cur);

    static const map<string, string> PUNCT = {
        {",", "།"}, {"/", "།"}, {";", "༔"}};
    const string TSHEG = "་";

    bool ok = true;
    vector<string> res;
    for (const auto& p : parts) {
        auto pu = PUNCT.find(p);
        if (pu != PUNCT.end()) {
            if (!res.empty() && res.back() == TSHEG) res.pop_back();
            res.push_back(pu->second + " ");
            continue;
        }
        string u;
        if (!sylToUni(p, u)) {
            ok = false;
            u = "⟨" + p + "⟩";   // ⟨wylie⟩ marker, never guessed
        }
        res.push_back(u);
        res.push_back(TSHEG);
    }
    if (!res.empty() && res.back() == TSHEG) res.pop_back();
    string joined;
    for (const auto& r : res) joined += r;
    // strip
    size_t jb = joined.find_first_not_of(' ');
    if (jb == string::npos) return {"", ok};
    size_t je = joined.find_last_not_of(' ');
    return {joined.substr(jb, je - jb + 1), ok};
}

}  // namespace allcore
