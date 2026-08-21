// Port of the remaining lucene-bo analyzers (Apache-2.0, © BDRC;
// authors Elie Roux, Hélios Hildt et al.) — see searchnorm.h for the
// integration map. Pattern rules are transcribed from
// TibPattFilter.java / TibCharFilter.java; character classes are
// implemented on UTF-32 codepoints rather than Java regex.
#include "allcore/searchnorm.h"

#include <fstream>
#include <vector>

#include "allcore/affixnorm.h"

namespace allcore {

namespace {

// ---- small UTF helpers (as in affixnorm.cpp) -----------------------
std::vector<char32_t> decode(const std::string& s) {
    std::vector<char32_t> out;
    for (size_t i = 0; i < s.size();) {
        unsigned char c = s[i];
        char32_t cp = 0;
        int n = 1;
        if (c < 0x80) { cp = c; }
        else if ((c >> 5) == 6) { cp = c & 0x1F; n = 2; }
        else if ((c >> 4) == 14) { cp = c & 0x0F; n = 3; }
        else { cp = c & 0x07; n = 4; }
        for (int k = 1; k < n && i + k < s.size(); ++k)
            cp = (cp << 6) | (s[i + k] & 0x3F);
        out.push_back(cp);
        i += n;
    }
    return out;
}

void append(std::string& s, char32_t cp) {
    if (cp < 0x80) s += (char)cp;
    else if (cp < 0x800) {
        s += (char)(0xC0 | (cp >> 6));
        s += (char)(0x80 | (cp & 0x3F));
    } else {
        s += (char)(0xE0 | (cp >> 12));
        s += (char)(0x80 | ((cp >> 6) & 0x3F));
        s += (char)(0x80 | (cp & 0x3F));
    }
}

std::string encode(const std::vector<char32_t>& v) {
    std::string s;
    for (char32_t cp : v) append(s, cp);
    return s;
}

// ---- TibCharFilter maps (transcribed; longest-first applied) -------
struct MapEnt { const char32_t* from; const char32_t* to; };
// strict (always-on) map
const MapEnt kStrict[] = {
    {U"ཿ", U"ཿ་"},
    {U"0", U"༠"}, {U"1", U"༡"}, {U"2", U"༢"}, {U"3", U"༣"},
    {U"4", U"༤"}, {U"5", U"༥"}, {U"6", U"༦"}, {U"7", U"༧"},
    {U"8", U"༨"}, {U"9", U"༩"},
    {U"༵", U""}, {U"༷", U""}, {U"༹", U""},
    {U"ༀ", U"ཨོཾ"},
    {U"༂", U"འུྂཿ"},
    {U"༃", U"འུྂ༔"},
    {U"གྷ", U"གྷ"}, {U"཈", U"ཇྷ"},
    {U"དྷ", U"དྷ"}, {U"བྷ", U"བྷ"},
    {U"ཛྷ", U"ཛྷ"}, {U"ྒྷ", U"ྒྷ"},
    {U"ྡྷ", U"ྡྷ"}, {U"ྦྷ", U"ྦྷ"},
    {U"ྫྷ", U"ྫྷ"},
};
// lenient additions (upstream lenient=true branches + lenient list)
const MapEnt kLenient[] = {
    {U"ཌྷ", U"ཇྷ"}, {U"ཀྵ", U"ཀྴ"},
    {U"ཱི", U"ི"}, {U"ཱུ", U"ུ"},
    {U"ྲྀ", U"ྲི"}, {U"ཷ", U"ྲི"},
    {U"ླྀ", U"ླི"}, {U"ཹ", U"ླི"},
    {U"ཱྀ", U"ི"}, {U"ྜྷ", U"ྡྷ"},
    {U"ྐྵ", U"ྐྴ"},
    {U"ཾ", U"ྃ"}, {U"ེེ", U"ཻ"}, {U"ོོ", U"ཽ"},
    {U"ཊ", U"ཏ"}, {U"ཋ", U"ཐ"}, {U"ཌ", U"ད"}, {U"ཎ", U"ན"},
    {U"ཱ", U""}, {U"ྂ", U"ྃ"},
    {U"ྚ", U"ྟ"}, {U"ྛ", U"ྠ"}, {U"ྜ", U"ྡ"}, {U"ྞ", U"ྣ"},
    {U"ྺ", U"ྭ"}, {U"ྻ", U"ྱ"}, {U"ྰ", U""}, {U"ྼ", U"ྲ"},
    {U"ཪ", U"ར"}, {U"ཥ", U"ཤ"}, {U"ྵ", U"ྴ"},
    {U"ནྱ", U"ཉ"}, {U"ྣྱ", U"ྙ"},
    {U"རྨྨ", U"རྨ"}, {U"རྦྦ", U"རྦ"}, {U"རྒྒ", U"རྒ"},
    {U"པདམ", U"པད་མ"}, {U"པདྨ", U"པད་མ"},
    {U"སེངྒེ", U"སེང་གེ"}, {U"སེངགེ", U"སེང་གེ"},
    {U"ལིངྒ", U"ལིང་ག"},
    {U"ལོཙ", U"ལོ་ཙ"}, {U"ལོཚ", U"ལོ་ཙ"}, {U"ལོ་ཙྭ", U"ལོ་ཙ"},
    {U"ཙྪ", U"ཙ"},
    {U"ཀུཎྜ", U"ཀུ་ནྡ"}, {U"ཀུནྡ", U"ཀུ་ནྡ"},
    {U"བནྡྷ", U"བན་དྷ"}, {U"མནྟ", U"མན་ཏ"},
    {U"ཀྲོདྷ", U"ཀྲོ་དྷ"}, {U"ཀྲོདྡྷ", U"ཀྲོ་དྷ"},
    {U"པནདི", U"པནྡི"}, {U"ཀྐ", U"ཀ"},
    {U"རཏན", U"རཏྣ"}, {U"ཀརྨ", U"ཀར་མ"},
    {U"ཀིརྟི", U"ཀིར་ཏི"}, {U"ཤམྦྷ", U"ཤམ་བྷ"},
    {U"བནྡེ", U"བན་དེ"}, {U"ལྡེའུ་བཙན", U"ལྡེ་བཙན"},
    {U"࿆", U""}, {U"༸", U"༧"},
    {U"ྀ", U"ི"}, {U"ོེ", U"ོའི"},
};

size_t len32(const char32_t* p) {
    size_t n = 0;
    while (p[n]) ++n;
    return n;
}

std::vector<char32_t> applyMaps(const std::vector<char32_t>& in,
                                bool lenient) {
    std::vector<char32_t> out;
    out.reserve(in.size());
    for (size_t i = 0; i < in.size();) {
        const MapEnt* hit = nullptr;
        size_t hitLen = 0;
        auto tryTable = [&](const MapEnt* tbl, size_t n) {
            for (size_t k = 0; k < n; ++k) {
                const size_t fl = len32(tbl[k].from);
                if (fl <= hitLen || i + fl > in.size()) continue;
                bool eq = true;
                for (size_t j = 0; j < fl && eq; ++j)
                    eq = (in[i + j] == tbl[k].from[j]);
                if (eq) { hit = &tbl[k]; hitLen = fl; }
            }
        };
        tryTable(kStrict, sizeof(kStrict) / sizeof(*kStrict));
        if (lenient)
            tryTable(kLenient, sizeof(kLenient) / sizeof(*kLenient));
        if (hit) {
            for (size_t j = 0; hit->to[j]; ++j) out.push_back(hit->to[j]);
            i += hitLen;
        } else {
            out.push_back(in[i++]);
        }
    }
    return out;
}

// ---- TibPattFilter (character classes on codepoints) ---------------
bool inRange(char32_t c, char32_t a, char32_t b) { return c >= a && c <= b; }
bool anyCons(char32_t c) { return inRange(c, U'ཀ', U'ྼ'); }
bool vowelSign(char32_t c) { return inRange(c, 0x0F71, 0x0F87); }
bool subscript(char32_t c) { return inRange(c, 0x0F8D, 0x0FBC); }

std::vector<char32_t> applyPatterns(std::vector<char32_t> v) {
    // ReorderFilter: vowel-sign run + subscript run -> swap
    for (size_t i = 0; i < v.size();) {
        if (!vowelSign(v[i])) { ++i; continue; }
        size_t a = i;
        while (i < v.size() && vowelSign(v[i])) ++i;
        size_t b = i;
        while (i < v.size() && subscript(v[i])) ++i;
        if (i > b) {
            std::vector<char32_t> swapped(v.begin() + b, v.begin() + i);
            swapped.insert(swapped.end(), v.begin() + a, v.begin() + b);
            std::copy(swapped.begin(), swapped.end(), v.begin() + a);
        }
    }
    std::vector<char32_t> o;
    o.reserve(v.size());
    for (size_t i = 0; i < v.size(); ++i) {
        // MergedSyl1: C ས ྟ ེ -> C ས ་ ཏ ེ
        if (i + 3 < v.size() && anyCons(v[i]) && v[i + 1] == U'ས' &&
            v[i + 2] == 0x0F9F && v[i + 3] == 0x0F7A) {
            o.push_back(v[i]);
            o.push_back(U'ས');
            o.push_back(0x0F0B);
            o.push_back(U'ཏ');
            o.push_back(0x0F7A);
            i += 3;
            continue;
        }
        // MergedSyl2: C [ན ལ ར] ྟ [ེ ོ] -> …་ཏ<v>
        if (i + 3 < v.size() && anyCons(v[i]) &&
            (v[i + 1] == U'ན' || v[i + 1] == U'ལ' || v[i + 1] == U'ར') &&
            v[i + 2] == 0x0F9F &&
            (v[i + 3] == 0x0F7A || v[i + 3] == 0x0F7C)) {
            o.push_back(v[i]);
            o.push_back(v[i + 1]);
            o.push_back(0x0F0B);
            o.push_back(U'ཏ');
            o.push_back(v[i + 3]);
            i += 3;
            continue;
        }
        // MergedSyl3: X ག [ི ྀ] Y  (X not a valid prefix of ga:
        // ད བ མ འ excluded; Y not a consonant) -> X ག ་ ག <v> Y
        if (i + 2 < v.size() && anyCons(v[i]) && v[i] != U'ད' &&
            v[i] != U'བ' && v[i] != U'མ' && v[i] != U'འ' &&
            v[i + 1] == U'ག' &&
            (v[i + 2] == 0x0F72 || v[i + 2] == 0x0F80) &&
            (i + 3 >= v.size() || !anyCons(v[i + 3]))) {
            o.push_back(v[i]);
            o.push_back(U'ག');
            o.push_back(0x0F0B);
            o.push_back(U'ག');
            o.push_back(v[i + 2]);
            i += 2;
            continue;
        }
        // SktFilters 1–4: [ཙ ཚ]→ཅ, [ྩ ྪ]→ྕ, ཛ→ཇ, ྫ→ྗ before
        // Sanskrit-context signs
        auto sktCtx = [&](size_t j) {
            return j < v.size() &&
                   (v[j] == 0x0F71 || inRange(v[j], 0x0F90, 0x0FAC) ||
                    v[j] == 0x0FB3 || v[j] == 0x0FB7);
        };
        if (i + 1 < v.size() && sktCtx(i + 1)) {
            if (v[i] == 0x0F59 || v[i] == 0x0F5A) { o.push_back(0x0F45); continue; }
            if (v[i] == 0x0FA9 || v[i] == 0x0FAA) { o.push_back(0x0F95); continue; }
            if (v[i] == 0x0F5B) { o.push_back(0x0F47); continue; }
            if (v[i] == 0x0FAB) { o.push_back(0x0F97); continue; }
        }
        o.push_back(v[i]);
    }
    return o;
}

LemmaFold g_lemmas;
bool g_lemmasLoaded = false;

}  // namespace

std::string paBaFoldWylie(const std::string& token) {
    if (token == "ba") return "pa";
    if (token == "bo") return "po";
    return token;
}

bool LemmaFold::load(const std::string& tsv_path) {
    std::ifstream f(tsv_path);
    if (!f) return false;
    std::string line;
    while (std::getline(f, line)) {
        if (line.empty() || line[0] == '#') continue;
        const auto t1 = line.find('\t');
        if (t1 == std::string::npos) continue;
        const auto t2 = line.find('\t', t1 + 1);
        const std::string from = line.substr(0, t1);
        const std::string to =
            line.substr(t1 + 1, (t2 == std::string::npos
                                     ? line.size()
                                     : t2) - t1 - 1);
        if (!from.empty() && !to.empty()) map_.emplace(from, to);
    }
    return !map_.empty();
}

const std::string* LemmaFold::fold(const std::string& w) const {
    auto it = map_.find(w);
    return it == map_.end() ? nullptr : &it->second;
}

void configureLemmaFold(const std::string& tsv_path) {
    g_lemmasLoaded = g_lemmas.load(tsv_path);
}

const LemmaFold* globalLemmaFold() {
    return g_lemmasLoaded ? &g_lemmas : nullptr;
}

std::string searchFoldWylie(const std::string& token) {
    // Old-Tibetan da-drag folds first, so archaic spellings meet
    // their classical forms in every search
    std::string t = paBaFoldWylie(
        stripAffixedParticlesWylie(stripDaDragWylie(token)));
    if (g_lemmasLoaded)
        if (const auto* l = g_lemmas.fold(t)) t = *l;
    return t;
}

std::string normalizeTibetanUnicode(const std::string& utf8,
                                    bool lenient) {
    return encode(applyPatterns(applyMaps(decode(utf8), lenient)));
}

}  // namespace allcore
