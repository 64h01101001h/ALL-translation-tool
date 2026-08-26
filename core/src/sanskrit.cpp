// Port of engines/sanskrit_convert.py (canonical oracle — rule 2: transcribed
// from the file, never reconstructed from spec).
//
// The Python operates on Unicode CODE POINTS: its tables are keyed by
// characters and its `sorted(key=len)` orders by character count, stably, on
// dict insertion order. This port therefore works in char32_t throughout and
// keeps every table in the Python file's insertion order, applying the same
// stable length-descending sorts at startup.
#include "allcore/sanskrit.h"

#include <algorithm>
#include <optional>
#include <vector>

#include "allcore/engines.h"

namespace allcore {

namespace {

// ---- UTF-8 ⇄ UTF-32 --------------------------------------------------------
std::u32string toU32(const std::string& s) {
    std::u32string out;
    size_t i = 0;
    while (i < s.size()) {
        const unsigned char c = s[i];
        char32_t cp = 0xFFFD;
        size_t n = 1;
        if (c < 0x80) { cp = c; }
        else if ((c >> 5) == 0x6 && i + 1 < s.size()) {
            cp = ((c & 0x1F) << 6) | (s[i + 1] & 0x3F); n = 2;
        } else if ((c >> 4) == 0xE && i + 2 < s.size()) {
            cp = ((c & 0x0F) << 12) | ((s[i + 1] & 0x3F) << 6) | (s[i + 2] & 0x3F);
            n = 3;
        } else if ((c >> 3) == 0x1E && i + 3 < s.size()) {
            cp = ((c & 0x07) << 18) | ((s[i + 1] & 0x3F) << 12) |
                 ((s[i + 2] & 0x3F) << 6) | (s[i + 3] & 0x3F);
            n = 4;
        }
        out += cp;
        i += n;
    }
    return out;
}

std::string fromU32(const std::u32string& s) {
    std::string out;
    for (char32_t cp : s) {
        if (cp < 0x80) out += (char)cp;
        else if (cp < 0x800) {
            out += (char)(0xC0 | (cp >> 6));
            out += (char)(0x80 | (cp & 0x3F));
        } else if (cp < 0x10000) {
            out += (char)(0xE0 | (cp >> 12));
            out += (char)(0x80 | ((cp >> 6) & 0x3F));
            out += (char)(0x80 | (cp & 0x3F));
        } else {
            out += (char)(0xF0 | (cp >> 18));
            out += (char)(0x80 | ((cp >> 12) & 0x3F));
            out += (char)(0x80 | ((cp >> 6) & 0x3F));
            out += (char)(0x80 | (cp & 0x3F));
        }
    }
    return out;
}

// str.lower() over the characters this engine can meet: ASCII + the IAST
// uppercase diacritics.
char32_t lowerCp(char32_t c) {
    if (c >= U'A' && c <= U'Z') return c - U'A' + U'a';
    switch (c) {
        case U'Ā': return U'ā'; case U'Ī': return U'ī'; case U'Ū': return U'ū';
        case U'Ṛ': return U'ṛ'; case U'Ṝ': return U'ṝ'; case U'Ḷ': return U'ḷ';
        case U'Ḹ': return U'ḹ'; case U'Ṅ': return U'ṅ'; case U'Ñ': return U'ñ';
        case U'Ṭ': return U'ṭ'; case U'Ḍ': return U'ḍ'; case U'Ṇ': return U'ṇ';
        case U'Ś': return U'ś'; case U'Ṣ': return U'ṣ'; case U'Ṃ': return U'ṃ';
        case U'Ṁ': return U'ṁ'; case U'Ḥ': return U'ḥ';
        default: return c;
    }
}

std::u32string lowered(const std::u32string& s) {
    std::u32string out = s;
    for (auto& c : out) c = lowerCp(c);
    return out;
}

bool isSpace(char32_t c) {
    return c == U' ' || c == U'\t' || c == U'\n' || c == U'\r' || c == U'\f' ||
           c == U'\v';
}

std::u32string stripped(const std::u32string& s) {
    size_t b = 0, e = s.size();
    while (b < e && isSpace(s[b])) ++b;
    while (e > b && isSpace(s[e - 1])) --e;
    return s.substr(b, e - b);
}

void replaceAll(std::u32string& s, const std::u32string& from,
                const std::u32string& to) {
    if (from.empty()) return;
    size_t pos = 0;
    while ((pos = s.find(from, pos)) != std::u32string::npos) {
        s.replace(pos, from.size(), to);
        pos += to.size();
    }
}

// ---- tables (Python-file insertion order preserved) ------------------------
using Table = std::vector<std::pair<std::u32string, std::u32string>>;

const Table& C_ACIP() {
    static const Table t = {
        {U"k", U"K"}, {U"kh", U"KH"}, {U"g", U"G"}, {U"gh", U"GH"},
        {U"ṅ", U"NG"}, {U"c", U"TZ"}, {U"ch", U"TSH"}, {U"j", U"DZ"},
        {U"jh", U"DZH"}, {U"ñ", U"NY"}, {U"ṭ", U"t"}, {U"ṭh", U"th"},
        {U"ḍ", U"d"}, {U"ḍh", U"dh"}, {U"ṇ", U"n"}, {U"t", U"T"},
        {U"th", U"TH"}, {U"d", U"D"}, {U"dh", U"DH"}, {U"n", U"N"},
        {U"p", U"P"}, {U"ph", U"PH"}, {U"b", U"B"}, {U"bh", U"BH"},
        {U"m", U"M"}, {U"y", U"Y"}, {U"r", U"R"}, {U"l", U"L"},
        {U"v", U"W"}, {U"ś", U"SH"}, {U"ṣ", U"sh"}, {U"s", U"S"},
        {U"h", U"H"},
    };
    return t;
}

const Table& V_ACIP() {
    static const Table t = {
        {U"a", U"A"}, {U"ā", U"'A"}, {U"i", U"I"}, {U"ī", U"'I"},
        {U"u", U"U"}, {U"ū", U"'U"}, {U"e", U"E"}, {U"o", U"O"},
        {U"ai", U"EE"}, {U"au", U"OO"}, {U"ṛ", U"Ri"}, {U"ṝ", U"R'I"},
        {U"ḷ", U"Li"}, {U"ḹ", U"L'I"},
    };
    return t;
}

std::optional<std::u32string> tableGet(const Table& t, const std::u32string& k) {
    for (const auto& [key, val] : t)
        if (key == k) return val;
    return std::nullopt;
}

// sorted(keys, key=len, reverse=True) — stable, character count
std::vector<std::u32string> keysByLenDesc(const Table& t) {
    std::vector<std::u32string> keys;
    for (const auto& [k, v] : t) keys.push_back(k);
    std::stable_sort(keys.begin(), keys.end(),
                     [](const auto& a, const auto& b) { return a.size() > b.size(); });
    return keys;
}

const Table& C_DEVA() {
    static const Table t = {
        {U"k", U"क"}, {U"kh", U"ख"}, {U"g", U"ग"}, {U"gh", U"घ"},
        {U"ṅ", U"ङ"}, {U"c", U"च"}, {U"ch", U"छ"}, {U"j", U"ज"},
        {U"jh", U"झ"}, {U"ñ", U"ञ"}, {U"ṭ", U"ट"}, {U"ṭh", U"ठ"},
        {U"ḍ", U"ड"}, {U"ḍh", U"ढ"}, {U"ṇ", U"ण"}, {U"t", U"त"},
        {U"th", U"थ"}, {U"d", U"द"}, {U"dh", U"ध"}, {U"n", U"न"},
        {U"p", U"प"}, {U"ph", U"फ"}, {U"b", U"ब"}, {U"bh", U"भ"},
        {U"m", U"म"}, {U"y", U"य"}, {U"r", U"र"}, {U"l", U"ल"},
        {U"v", U"व"}, {U"ś", U"श"}, {U"ṣ", U"ष"}, {U"s", U"स"},
        {U"h", U"ह"},
    };
    return t;
}

const Table& V_DEVA_IND() {
    static const Table t = {
        {U"a", U"अ"}, {U"ā", U"आ"}, {U"i", U"इ"}, {U"ī", U"ई"},
        {U"u", U"उ"}, {U"ū", U"ऊ"}, {U"ṛ", U"ऋ"}, {U"ṝ", U"ॠ"},
        {U"ḷ", U"ऌ"}, {U"ḹ", U"ॡ"}, {U"e", U"ए"}, {U"ai", U"ऐ"},
        {U"o", U"ओ"}, {U"au", U"औ"},
    };
    return t;
}

const Table& V_DEVA_DEP() {
    static const Table t = {
        {U"a", U""}, {U"ā", U"ा"}, {U"i", U"ि"}, {U"ī", U"ी"},
        {U"u", U"ु"}, {U"ū", U"ू"}, {U"ṛ", U"ृ"}, {U"ṝ", U"ॄ"},
        {U"ḷ", U"ॢ"}, {U"ḹ", U"ॣ"}, {U"e", U"े"}, {U"ai", U"ै"},
        {U"o", U"ो"}, {U"au", U"ौ"},
    };
    return t;
}

const char32_t VIRAMA = U'्', ANUSVARA = U'ं', VISARGA = U'ः';

const Table& PRON() {
    static const Table t = {
        {U"ā", U"a"}, {U"ī", U"i"}, {U"ū", U"u"}, {U"ṛ", U"ri"},
        {U"ṝ", U"ri"}, {U"ḷ", U"li"}, {U"ḹ", U"li"}, {U"ṃ", U"m"},
        {U"ṁ", U"m"}, {U"ḥ", U"h"}, {U"ṅ", U"ng"}, {U"ñ", U"ny"},
        {U"ṭ", U"t"}, {U"ḍ", U"d"}, {U"ṇ", U"n"}, {U"ś", U"sh"},
        {U"ṣ", U"sh"}, {U"c", U"ch"},
    };
    return t;
}

const Table& NEXTLETTER() {
    static const Table t = {
        {U"ā", U"a-"}, {U"ī", U"i-"}, {U"ū", U"u-"}, {U"ṛ", U"r."},
        {U"ṝ", U"r.-"}, {U"ḷ", U"l."}, {U"ḹ", U"l.-"}, {U"ṃ", U"m."},
        {U"ḥ", U"h."}, {U"ṅ", U"n*"}, {U"ñ", U"n~"}, {U"ṭ", U"t."},
        {U"ḍ", U"d."}, {U"ṇ", U"n."}, {U"ś", U"sh"}, {U"ṣ", U"s."},
    };
    return t;
}

const Table& INPUTCODE() {
    static const Table t = {
        {U"ā", U"a#"}, {U"ī", U"i#"}, {U"ū", U"u#"}, {U"ṛ", U"r%"},
        {U"ṝ", U"r%#"}, {U"ḷ", U"l%"}, {U"ḹ", U"l%#"}, {U"ṃ", U"m%"},
        {U"ḥ", U"h%"}, {U"ṅ", U"n*"}, {U"ñ", U"n~"}, {U"ṭ", U"t%"},
        {U"ḍ", U"d%"}, {U"ṇ", U"n%"}, {U"ś", U"sh"}, {U"ṣ", U"s%"},
    };
    return t;
}

// FINAL = {'ṃ':'m','ḥ':':','ṁ':'m'}
bool isFinal(char32_t c) { return c == U'ṃ' || c == U'ḥ' || c == U'ṁ'; }

// break characters: " -–'’·."
bool isBreak(char32_t c) {
    return c == U' ' || c == U'-' || c == U'–' || c == U'\'' || c == U'’' ||
           c == U'·' || c == U'.';
}

// ---- tokenize / syllabify ---------------------------------------------------
struct Tok {
    char kind;  // 'C' 'V' 'F' 'B'
    std::u32string val;
};

std::optional<std::vector<Tok>> tokenizeIast(const std::string& word) {
    const std::u32string w = stripped(lowered(toU32(word)));
    static const std::vector<std::u32string> CK = keysByLenDesc(C_ACIP());
    static const std::vector<std::u32string> VK = keysByLenDesc(V_ACIP());
    static const std::u32string SIMPLE_VOWELS = U"aāiīuūṛṝḷḹeo";
    std::vector<Tok> out;
    size_t i = 0;
    while (i < w.size()) {
        const char32_t ch = w[i];
        if (isFinal(ch)) { out.push_back({'F', std::u32string(1, ch)}); ++i; continue; }
        if (isBreak(ch)) { out.push_back({'B', U" "}); ++i; continue; }
        const std::u32string* v = nullptr;
        for (const auto& k : VK)
            if (w.compare(i, k.size(), k) == 0) { v = &k; break; }
        const std::u32string* c = nullptr;
        for (const auto& k : CK)
            if (w.compare(i, k.size(), k) == 0) { c = &k; break; }
        if (c && (!v || c->size() >= v->size() ||
                  SIMPLE_VOWELS.find(ch) == std::u32string::npos)) {
            out.push_back({'C', *c});
            i += c->size();
        } else if (v) {
            out.push_back({'V', *v});
            i += v->size();
        } else {
            return std::nullopt;
        }
    }
    return out;
}

struct Syl {
    std::vector<std::u32string> onset;
    std::u32string vowel;
    std::vector<std::u32string> finals;
};

bool isOnset2nd(const std::u32string& c) {
    // ONSET_2ND = {r, y, w, v, l}
    return c == U"r" || c == U"y" || c == U"w" || c == U"v" || c == U"l";
}

std::vector<Syl> syllabify(const std::vector<Tok>& toks) {
    std::vector<Syl> syls;
    std::vector<std::u32string> cur_on;
    size_t i = 0;
    const size_t n = toks.size();
    while (i < n) {
        const auto& [k, v] = toks[i];
        if (k == 'B') { ++i; continue; }
        if (k == 'C') { cur_on.push_back(v); ++i; continue; }
        if (k == 'V') {
            Syl syl{cur_on, v, {}};
            cur_on.clear();
            ++i;
            while (i < n && toks[i].kind == 'F') {
                syl.finals.push_back(toks[i].val);
                ++i;
            }
            size_t j = i;
            std::vector<std::u32string> run;
            while (j < n && toks[j].kind == 'C') { run.push_back(toks[j].val); ++j; }
            const bool nxt_vowel = j < n && toks[j].kind == 'V';
            if (!run.empty() && nxt_vowel) {
                std::vector<std::u32string> on, coda;
                if (run.size() == 1) {
                    on = {run[0]};
                } else if (isOnset2nd(run.back()) && run.size() >= 2) {
                    on.assign(run.end() - 2, run.end());
                    coda.assign(run.begin(), run.end() - 2);
                } else {
                    on = {run.back()};
                    coda.assign(run.begin(), run.end() - 1);
                }
                syl.finals.insert(syl.finals.end(), coda.begin(), coda.end());
                cur_on = on;
                i = j;
            } else if (!run.empty()) {
                syl.finals.insert(syl.finals.end(), run.begin(), run.end());
                i = j;
            }
            syls.push_back(std::move(syl));
            continue;
        }
        ++i;
    }
    if (!cur_on.empty() && !syls.empty())
        syls.back().finals.insert(syls.back().finals.end(), cur_on.begin(),
                                  cur_on.end());
    return syls;
}

// AMBIG pairs — ported verbatim from the Python (some pairs can never fire
// because their first element is not a token; they are kept for fidelity)
bool isAmbig(const std::u32string& a, const std::u32string& b) {
    static const std::vector<std::pair<std::u32string, std::u32string>> P = {
        {U"n", U"y"}, {U"N", U"y"}, {U"t", U"s"}, {U"t", U"sh"},
        {U"T", U"h"}, {U"d", U"h"}, {U"k", U"h"}, {U"g", U"h"},
        {U"p", U"h"}, {U"b", U"h"}, {U"ṇ", U"y"}, {U"d", U"z"},
        {U"t", U"z"},
    };
    for (const auto& [x, y] : P)
        if (a == x && b == y) return true;
    return false;
}

std::optional<std::u32string> iastToAcipU32(const std::string& word) {
    auto toks = tokenizeIast(word);
    if (!toks) return std::nullopt;
    std::u32string joined;
    bool first = true;
    for (const auto& syl : syllabify(*toks)) {
        std::vector<std::u32string> acs;
        for (size_t idx = 0; idx < syl.onset.size(); ++idx) {
            std::u32string a = *tableGet(C_ACIP(), syl.onset[idx]);
            if (syl.onset[idx] == U"v" && idx > 0) a = U"V";
            acs.push_back(std::move(a));
        }
        std::u32string s;
        for (size_t idx = 0; idx < acs.size(); ++idx) {
            if (idx > 0 && isAmbig(syl.onset[idx - 1], syl.onset[idx])) s += U'+';
            s += acs[idx];
        }
        const std::u32string vv = *tableGet(V_ACIP(), syl.vowel);
        if (!syl.onset.empty()) {
            s += vv;
        } else {
            if (syl.vowel == U"a") s = U"AA";
            else s = U"A" + vv;
        }
        for (const auto& f : syl.finals) {
            if (f == U"ṃ" || f == U"ṁ") s += U'm';
            else if (f == U"ḥ") s += U':';
            else s += *tableGet(C_ACIP(), f);
        }
        if (!first) joined += U' ';
        joined += s;
        first = false;
    }
    return joined;
}

std::u32string applyTable(const std::u32string& word, const Table& table) {
    // for k in sorted(table, key=len, reverse=True): s = s.replace(k, table[k])
    std::u32string s = word;
    for (const auto& k : keysByLenDesc(table)) replaceAll(s, k, *tableGet(table, k));
    return s;
}

}  // namespace

// ---- public API -------------------------------------------------------------
std::pair<std::string, bool> iastToAcip(const std::string& iast) {
    auto a = iastToAcipU32(iast);
    if (!a) return {"", false};
    return {fromU32(*a), true};
}

std::pair<std::string, bool> iastToDevanagari(const std::string& iast) {
    auto toks = tokenizeIast(iast);
    if (!toks) return {"", false};
    std::u32string out;
    bool prev_cons = false;
    for (const auto& [k, v] : *toks) {
        if (k == 'B') { out += U' '; prev_cons = false; continue; }
        if (k == 'C') {
            if (prev_cons) out += VIRAMA;
            out += *tableGet(C_DEVA(), v);
            prev_cons = true;
        } else if (k == 'V') {
            out += prev_cons ? *tableGet(V_DEVA_DEP(), v)
                             : *tableGet(V_DEVA_IND(), v);
            prev_cons = false;
        } else {
            out += (v == U"ṃ" || v == U"ṁ") ? ANUSVARA : VISARGA;
            prev_cons = false;
        }
    }
    if (prev_cons) out += VIRAMA;
    return {fromU32(out), true};
}

std::string iastToPronunciation(const std::string& iast) {
    // sequential str.replace in the PRON dict's insertion order
    std::u32string s = lowered(toU32(iast));
    for (const auto& [k, v] : PRON()) replaceAll(s, k, v);
    return fromU32(s);
}

std::pair<std::string, bool> iastToTibetan(const std::string& iast) {
    auto a = iastToAcip(iast);
    if (!a.second) return {"", false};
    std::string w = acipToEwts(a.first);
    for (auto& c : w)
        if (c == 'v') c = 'w';   // Sanskrit va renders with wazur
    auto [u, ok] = wylieToUnicode(w);
    if (!ok) return {"", false};
    return {u, true};
}

std::string acipSanskritToIast(const std::string& acip) {
    std::u32string s = toU32(acipToEwts(acip));
    // REV in Python insertion order; applied sorted by key length desc, stable
    static const Table REV = {
        {U"A", U"ā"}, {U"I", U"ī"}, {U"U", U"ū"}, {U"M", U"ṃ"},
        {U"H", U"ḥ"}, {U"T", U"ṭ"}, {U"Th", U"ṭh"}, {U"D", U"ḍ"},
        {U"N", U"ṇ"}, {U"Sh", U"ṣ"}, {U"sh", U"ś"}, {U"ny", U"ñ"},
        {U"ng", U"ṅ"}, {U"tsh", U"ch"}, {U"ts", U"c"}, {U"dz", U"j"},
        {U"w", U"v"}, {U":", U"ḥ"},
    };
    for (const auto& k : keysByLenDesc(REV)) replaceAll(s, k, *tableGet(REV, k));
    std::u32string out;   // re.sub(r'\s+', '', s)
    for (char32_t c : s)
        if (!isSpace(c)) out += c;
    return fromU32(out);
}

std::string iastToNextletter(const std::string& iast) {
    return fromU32(applyTable(lowered(toU32(iast)), NEXTLETTER()));
}

std::string iastToInputcode(const std::string& iast) {
    return fromU32(applyTable(lowered(toU32(iast)), INPUTCODE()));
}

std::string inputcodeToIast(const std::string& code) {
    // reverse replaces sorted by VALUE length desc, stable over insertion order
    std::u32string s = lowered(toU32(code));
    std::vector<std::pair<std::u32string, std::u32string>> items(
        INPUTCODE().begin(), INPUTCODE().end());
    std::stable_sort(items.begin(), items.end(), [](const auto& a, const auto& b) {
        return a.second.size() > b.second.size();
    });
    for (const auto& [k, v] : items) replaceAll(s, v, k);
    return fromU32(s);
}

// ---- authentic pronunciation layer ------------------------------------------

std::pair<std::string, bool> devanagariToIast(const std::string& devanagari) {
    // inverse walk over the same C_DEVA / V_DEVA tables the forward converter
    // uses: consonant carries an inherent 'a' unless a dependent vowel or
    // virama follows
    const std::u32string in = toU32(devanagari);
    auto findByValue = [](const Table& t,
                          char32_t cp) -> const std::u32string* {
        const std::u32string key(1, cp);
        for (const auto& [k, v] : t)
            if (v == key) return &k;
        return nullptr;
    };
    std::u32string out;
    bool pending_a = false;   // an inherent 'a' owed by the last consonant
    bool ok = true;
    for (size_t i = 0; i < in.size(); ++i) {
        const char32_t cp = in[i];
        if (cp == U' ' || cp == U'।' || cp == U'॥') {
            // space / danda / double danda → word break
            if (pending_a) { out += U'a'; pending_a = false; }
            if (!out.empty() && out.back() != U' ') out += U' ';
            continue;
        }
        if (cp == VIRAMA) { pending_a = false; continue; }
        if (cp == ANUSVARA) {
            if (pending_a) { out += U'a'; pending_a = false; }
            out += U"ṃ";
            continue;
        }
        if (cp == VISARGA) {
            if (pending_a) { out += U'a'; pending_a = false; }
            out += U"ḥ";
            continue;
        }
        if (const auto* dep = findByValue(V_DEVA_DEP(), cp)) {
            // dependent vowel sign replaces the inherent 'a'
            out += *dep;
            pending_a = false;
            continue;
        }
        if (const auto* ind = findByValue(V_DEVA_IND(), cp)) {
            if (pending_a) { out += U'a'; pending_a = false; }
            out += *ind;
            continue;
        }
        if (const auto* c = findByValue(C_DEVA(), cp)) {
            if (pending_a) out += U'a';
            out += *c;
            pending_a = true;
            continue;
        }
        ok = false;   // not Sanskrit Devanagari (flagged, not skipped silently)
        break;
    }
    if (pending_a) out += U'a';
    while (!out.empty() && out.back() == U' ') out.pop_back();
    return {fromU32(out), ok};
}

namespace {

// Classical Sanskrit IPA (Help:IPA/Sanskrit). Dentals are given as in the
// source chart.
const Table& IPA_C() {
    static const Table t = {
        {U"k", U"k"}, {U"kh", U"kʰ"}, {U"g", U"ɡ"}, {U"gh", U"ɡʱ"},
        {U"ṅ", U"ŋ"}, {U"c", U"tɕ"}, {U"ch", U"tɕʰ"}, {U"j", U"dʑ"},
        {U"jh", U"dʑʱ"}, {U"ñ", U"ɲ"}, {U"ṭ", U"ʈ"}, {U"ṭh", U"ʈʰ"},
        {U"ḍ", U"ɖ"}, {U"ḍh", U"ɖʱ"}, {U"ṇ", U"ɳ"}, {U"t", U"t"},
        {U"th", U"tʰ"}, {U"d", U"d"}, {U"dh", U"dʱ"}, {U"n", U"n"},
        {U"p", U"p"}, {U"ph", U"pʰ"}, {U"b", U"b"}, {U"bh", U"bʱ"},
        {U"m", U"m"}, {U"y", U"j"}, {U"r", U"r"}, {U"l", U"l"},
        {U"v", U"ʋ"}, {U"ś", U"ɕ"}, {U"ṣ", U"ʂ"}, {U"s", U"s"},
        {U"h", U"ɦ"},
    };
    return t;
}

const Table& IPA_V() {
    static const Table t = {
        {U"a", U"ɐ"}, {U"ā", U"aː"}, {U"i", U"ɪ"}, {U"ī", U"iː"},
        {U"u", U"ʊ"}, {U"ū", U"uː"}, {U"ṛ", U"r̩"}, {U"ṝ", U"r̩ː"},
        {U"ḷ", U"l̩"}, {U"ḹ", U"l̩ː"}, {U"e", U"eː"}, {U"ai", U"aːi̯"},
        {U"o", U"oː"}, {U"au", U"aːu̯"},
    };
    return t;
}

// row-nasal for anusvāra assimilation (FPMT rule: the nasal of the following
// consonant's row; word-final = m)
std::u32string rowNasal(const std::u32string& cons) {
    auto in = [&](const char* set) {
        const std::u32string s = toU32(set);
        // set holds space-separated consonant keys
        std::u32string cur;
        for (char32_t c : s + U" ") {
            if (c == U' ') {
                if (cur == cons) return true;
                cur.clear();
            } else cur += c;
        }
        return false;
    };
    if (in("k kh g gh ṅ")) return U"ŋ";
    if (in("c ch j jh ñ")) return U"ɲ";
    if (in("ṭ ṭh ḍ ḍh ṇ")) return U"ɳ";
    if (in("t th d dh n")) return U"n";
    if (in("p ph b bh m")) return U"m";
    return U"";   // sibilants, semivowels, h → nasalized vowel instead
}

}  // namespace

std::pair<std::string, bool> iastToIpa(const std::string& iast) {
    auto toks = tokenizeIast(iast);
    if (!toks) return {"", false};
    std::u32string out;
    const auto& T = *toks;
    for (size_t i = 0; i < T.size(); ++i) {
        const auto& [k, v] = T[i];
        if (k == 'B') { out += U' '; continue; }
        if (k == 'C') { out += *tableGet(IPA_C(), v); continue; }
        if (k == 'V') { out += *tableGet(IPA_V(), v); continue; }
        // finals: ṃ / ṁ / ḥ
        if (v == U"ḥ") { out += U'h'; continue; }
        // anusvāra: next consonant's row-nasal; word-final m; otherwise
        // nasalize the preceding vowel
        // STATIC-6: this was a loop whose body always broke - a
        // conditional wearing a loop's clothes, and the shape implied a
        // scan-ahead the contract never asks for. In well-formed IAST
        // the token after an anusvara is either a consonant or a word
        // boundary, so ONE look is the whole algorithm; the loop
        // suggested otherwise and the audit rightly flagged the
        // contradiction. Same behaviour, honest shape.
        std::u32string nxt;
        if (i + 1 < T.size() && T[i + 1].kind == 'C') nxt = T[i + 1].val;
        if (nxt.empty()) {
            out += U'm';
        } else {
            const std::u32string nasal = rowNasal(nxt);
            if (!nasal.empty()) out += nasal;
            else out += U"̃";   // combining tilde on the preceding vowel
        }
    }
    while (!out.empty() && out.back() == U' ') out.pop_back();
    return {fromU32(out), true};
}

std::vector<std::pair<std::string, std::string>> sanskritLetterGuide(
    const std::string& iast) {
    // FPMT Translation Services guide (Nov 2020): American-English anchors.
    static const std::vector<std::pair<std::u32string, const char*>> G = {
        {U"ā", "long a, as in \"father\""},
        {U"ī", "long i, as in \"seek\""},
        {U"ū", "long u, as in \"tool\""},
        {U"ṛ", "vocalic r, as in \"cringe\""},
        {U"ṝ", "long vocalic r, as in \"reed\""},
        {U"ḷ", "vocalic l, as in \"jewelry\""},
        {U"ḹ", "long vocalic l (rare)"},
        {U"ai", "as in \"aisle\""},
        {U"au", "as in \"cow\""},
        {U"ṃ", "anusvāra: nasal — word-finally \"m\"; otherwise the nasal of "
               "the following consonant's row (saṃgha → \"saṅgha\")"},
        {U"ḥ", "visarga: a soft echo of the preceding vowel"},
        {U"ṅ", "as in \"king\""},
        {U"ñ", "as in \"onion\""},
        {U"ṭ", "retroflex: curled-tongue \"tra\""},
        {U"ṭh", "retroflex aspirated \"tra\""},
        {U"ḍ", "retroflex: curled-tongue \"dra\""},
        {U"ḍh", "retroflex aspirated \"dra\""},
        {U"ṇ", "retroflex: curled-tongue \"nra\""},
        {U"ś", "sh, as in \"shawl\" (palatal)"},
        {U"ṣ", "sh, as in \"push\" (retroflex)"},
        {U"c", "ch, as in \"chap\""},
        {U"th", "aspirated t, as in \"boathouse\" — never as in \"the\""},
        {U"ph", "aspirated p, as in \"cup hook\" — never \"f\""},
        {U"kh", "aspirated k, as in \"blockhead\""},
        {U"gh", "aspirated g, as in \"ghastly\""},
        {U"jh", "aspirated j, as in \"bridgehead\""},
        {U"dh", "aspirated d, as in \"bloodhound\""},
        {U"bh", "aspirated b, as in \"abhor\""},
        {U"jñ", "may be pronounced \"jñ\", \"gy\", or \"gñ\" (jñāna)"},
    };
    const std::u32string w = lowered(toU32(iast));
    std::vector<std::pair<std::string, std::string>> out;
    for (const auto& [letter, hint] : G)
        if (w.find(letter) != std::u32string::npos)
            out.emplace_back(fromU32(letter), hint);
    return out;
}

}  // namespace allcore
