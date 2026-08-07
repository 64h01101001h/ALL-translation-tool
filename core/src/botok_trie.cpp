// botok_trie.cpp — C++ port of Botok's trie layer (see botok.h header
// comment). Port sources: tries/basictrie.py, tries/trie.py (in-memory logic
// only), textunits/sylcomponents.py + bosyl.py, third_party/has_skrt_syl.py.
// Apache-2.0 upstream; every quirk preserved and marked [UPSTREAM QUIRK].
#include "allcore/botok.h"

#include <fstream>
#include <sstream>
#include <stdexcept>

namespace allcore {
namespace botok {

// ------------------------------------------------------------ minimal JSON
// SylComponents.json holds only objects/arrays/strings — a tiny recursive
// parser covering exactly those shapes (throws on anything else).
namespace {

struct JsonValue {
    enum Kind { Str, Arr, Obj } kind = Str;
    std::string str;
    std::vector<JsonValue> arr;
    std::vector<std::pair<std::string, JsonValue>> obj;  // order preserved
};

class JsonParser {
public:
    explicit JsonParser(const std::string& text) : s_(text) {}
    JsonValue parse() {
        skipWs();
        JsonValue v = value();
        return v;
    }

private:
    void skipWs() {
        while (i_ < s_.size() && (s_[i_] == ' ' || s_[i_] == '\t' ||
                                  s_[i_] == '\n' || s_[i_] == '\r'))
            ++i_;
    }
    char peek() {
        if (i_ >= s_.size()) throw std::runtime_error("json: unexpected end");
        return s_[i_];
    }
    JsonValue value() {
        skipWs();
        char c = peek();
        if (c == '{') return object();
        if (c == '[') return array();
        if (c == '"') {
            JsonValue v;
            v.kind = JsonValue::Str;
            v.str = string();
            return v;
        }
        throw std::runtime_error("json: unsupported value");
    }
    std::string string() {
        if (peek() != '"') throw std::runtime_error("json: expected string");
        ++i_;
        std::string out;
        while (true) {
            if (i_ >= s_.size()) throw std::runtime_error("json: unterminated");
            char c = s_[i_++];
            if (c == '"') break;
            if (c == '\\') {
                char e = s_[i_++];
                switch (e) {
                    case '"': out += '"'; break;
                    case '\\': out += '\\'; break;
                    case '/': out += '/'; break;
                    case 'n': out += '\n'; break;
                    case 't': out += '\t'; break;
                    case 'u': {  // BMP only — Tibetan never needs surrogates
                        unsigned cp = std::stoul(s_.substr(i_, 4), nullptr, 16);
                        i_ += 4;
                        std::u32string one(1, static_cast<char32_t>(cp));
                        out += u32to8(one);
                        break;
                    }
                    default: throw std::runtime_error("json: bad escape");
                }
            } else {
                out += c;
            }
        }
        return out;
    }
    JsonValue array() {
        ++i_;  // [
        JsonValue v;
        v.kind = JsonValue::Arr;
        skipWs();
        if (peek() == ']') { ++i_; return v; }
        while (true) {
            v.arr.push_back(value());
            skipWs();
            char c = peek();
            ++i_;
            if (c == ']') break;
            if (c != ',') throw std::runtime_error("json: expected , or ]");
        }
        return v;
    }
    JsonValue object() {
        ++i_;  // {
        JsonValue v;
        v.kind = JsonValue::Obj;
        skipWs();
        if (peek() == '}') { ++i_; return v; }
        while (true) {
            skipWs();
            std::string k = string();
            skipWs();
            if (peek() != ':') throw std::runtime_error("json: expected :");
            ++i_;
            v.obj.emplace_back(k, value());
            skipWs();
            char c = peek();
            ++i_;
            if (c == '}') break;
            if (c != ',') throw std::runtime_error("json: expected , or }");
        }
        return v;
    }
    const std::string& s_;
    size_t i_ = 0;
};

const JsonValue* find(const JsonValue& obj, const char* key) {
    for (const auto& [k, v] : obj.obj)
        if (k == key) return &v;
    return nullptr;
}

std::vector<std::u32string> strList(const JsonValue& v) {
    std::vector<std::u32string> out;
    for (const auto& e : v.arr) out.push_back(u8to32(e.str));
    return out;
}

}  // namespace

// ---------------------------------------------------------------- BasicTrie

void BasicTrie::add(const std::vector<std::string>& word, const DataPatch* data) {
    TrieNode* current = head_.get();
    for (const std::string& syl : word) {
        auto it = current->children.find(syl);
        if (it == current->children.end())
            it = current->children.emplace(syl, std::make_unique<TrieNode>()).first;
        current = it->second.get();
    }
    current->leaf = true;
    if (data) {  // Python: current_node.data.update(data)
        if (data->affixation) {
            current->data.hasAffixation = true;
            current->data.affixation = *data->affixation;
        }
        if (data->skrt) current->data.skrt = *data->skrt;
    }
}

TrieNode* BasicTrie::walk(const std::string& syl, TrieNode* current) const {
    if (!current) current = head_.get();
    auto it = current->children.find(syl);
    return it == current->children.end() ? nullptr : it->second.get();
}

bool BasicTrie::isDiffMeaning(const Sense& m1, const Sense& m2) {
    // for k,v in m1: k not in m2 or m2[k] != v -> diff
    bool diff = false;
    auto chk = [&diff](const auto& a, const auto& b) {
        if (a && (!b || *a != *b)) diff = true;
    };
    chk(m1.pos, m2.pos);
    chk(m1.lemma, m2.lemma);
    chk(m1.sense, m2.sense);
    chk(m1.freq, m2.freq);
    chk(m1.affixed, m2.affixed);
    return diff;
}

bool BasicTrie::addMeaning(std::vector<Sense>& meanings, const Sense& meaning) {
    // [UPSTREAM QUIRK] appends at the FIRST existing meaning that differs
    // (not "if none equal"), preserved verbatim.
    if (!meanings.empty()) {
        for (const Sense& m : meanings) {
            if (isDiffMeaning(meaning, m)) {
                meanings.push_back(meaning);
                return true;
            }
        }
        return false;
    }
    meanings.push_back(meaning);
    return true;
}

bool BasicTrie::addData(const std::vector<std::string>& word, const Sense& sense) {
    if (word.empty()) throw std::runtime_error("addData: empty word");
    TrieNode* current = head_.get();
    for (const std::string& syl : word) {
        current = walk(syl, current);
        if (!current) return false;
    }
    if (!current->leaf) return false;
    current->data.hasSenses = true;
    return addMeaning(current->data.senses, sense);
}

bool BasicTrie::addData(const std::vector<std::string>& word, long formFreq) {
    if (word.empty()) throw std::runtime_error("addData: empty word");
    TrieNode* current = head_.get();
    for (const std::string& syl : word) {
        current = walk(syl, current);
        if (!current) return false;
    }
    if (!current->leaf) return false;
    current->data.hasFormFreq = true;
    current->data.formFreq = formFreq;
    return true;
}

bool BasicTrie::deactivate(const std::vector<std::string>& word, bool rev) {
    TrieNode* current = head_.get();
    for (const std::string& syl : word) {
        current = walk(syl, current);
        if (!current) return false;
    }
    current->leaf = rev;  // data is always a dict upstream, so always reached
    return true;
}

// -------------------------------------------------------------------- BoSyl

BoSyl::BoSyl(const std::string& dir) {
    const std::string path = dir + "/SylComponents.json";
    std::ifstream in(path, std::ios::binary);
    if (!in) throw std::runtime_error("BoSyl: cannot open " + path);
    std::stringstream buf;
    buf << in.rdbuf();
    std::string text = buf.str();
    if (text.size() >= 3 && static_cast<unsigned char>(text[0]) == 0xEF)
        text = text.substr(3);  // utf-8-sig BOM
    JsonValue root = JsonParser(text).parse();

    const JsonValue* v;
    if ((v = find(root, "dadrag"))) dadrag_ = strList(*v);
    if ((v = find(root, "suffixes"))) suffixes_ = strList(*v);
    if ((v = find(root, "Csuffixes"))) csuffixes_ = strList(*v);
    // exceptions = special + wazurs (that order)
    if ((v = find(root, "special")))
        for (auto& s : strList(*v)) exceptions_.push_back(s);
    if ((v = find(root, "wazurs")))
        for (auto& s : strList(*v)) exceptions_.push_back(s);
    if ((v = find(root, "roots")))
        for (const auto& [k, val] : v->obj) roots_[u8to32(k)] = val.str;
    // ambiguous values are 2-item lists -> get_parts returns a Python list,
    // which get_mingzhi treats as "no unambiguous mingzhi"; keys suffice.
    if ((v = find(root, "ambiguous")))
        for (const auto& [k, val] : v->obj) ambiguous_.push_back(u8to32(k));
    // mingzhis = m_roots updated with m_exceptions then m_wazurs
    for (const char* key : {"m_roots", "m_exceptions", "m_wazurs"})
        if ((v = find(root, key)))
            for (const auto& [k, val] : v->obj)
                mingzhis_[u8to32(k)] = u8to32(val.str);
    if (roots_.size() < 300 || suffixes_.size() < 100)
        throw std::runtime_error("BoSyl: SylComponents.json looks wrong");
}

template <typename Vec, typename T>
static bool contains(const Vec& v, const T& x) {
    for (const auto& e : v)
        if (e == x) return true;
    return false;
}

BoSyl::Parts BoSyl::getParts(const std::u32string& syl) const {
    Parts none;  // NotWellFormed doubles for Python None AND list results,
                 // both of which mean "no unambiguous single split" here.
    if (!contains(exceptions_, syl) && !contains(ambiguous_, syl)) {
        const size_t ls = syl.size();
        // all possible roots, longest first (Python: len(syl) > 5 and
        // syl[:6] in roots ... down to syl[:1]; len(syl) > n-1 == ls >= n)
        std::vector<std::u32string> root;
        for (size_t n = 6; n >= 1; --n) {
            if (ls < n) continue;
            std::u32string cand = syl.substr(0, n);
            if (roots_.count(cand)) root.push_back(cand);
        }
        // all possible suffixes (1..5 chars from the end)
        std::vector<std::u32string> suffix;
        if (ls > 1) {
            for (size_t n = 1; n <= 5; ++n) {
                if (n > ls) break;
                std::u32string cand = syl.substr(ls - n);
                if (contains(suffixes_, cand)) suffix.push_back(cand);
            }
        }
        // C roots
        if (!root.empty() && roots_.at(root[0]) == "C") {
            if (root[0] == syl) {
                Parts p;
                p.kind = Parts::Single;
                p.root = root[0];
                return p;
            }
            for (const auto& s : suffix) {
                if (contains(csuffixes_, s) && root[0] + s == syl) {
                    Parts p;
                    p.kind = Parts::Single;
                    p.root = root[0];
                    p.suffix = s;
                    return p;
                }
            }
        }
        // all possible matches
        std::vector<std::pair<std::u32string, std::u32string>> solutions;
        if (!suffix.empty() && !root.empty()) {
            for (const auto& r : root) {
                for (const auto& s : suffix) {
                    if (roots_.at(r) == "A" && s == U"འ" && r + s == syl)
                        return none;  // unexpected འ
                    if (r + s == syl) {
                        bool dup = false;
                        for (const auto& sol : solutions)
                            if (sol.first == r && sol.second == s) dup = true;
                        if (!dup) solutions.emplace_back(r, s);
                    }
                }
            }
            if (!solutions.empty()) {
                if (solutions.size() > 1) {
                    Parts p;
                    p.kind = Parts::Multiple;
                    return p;
                }
                Parts p;
                p.kind = Parts::Single;
                p.root = solutions[0].first;
                p.suffix = solutions[0].second;
                return p;
            }
            return none;
        } else if (!root.empty()) {
            for (const auto& r : root) {
                if (r == syl && roots_.at(r) != "NB") {
                    bool dup = false;
                    for (const auto& sol : solutions)
                        if (sol.first == r && sol.second.empty()) dup = true;
                    if (!dup) solutions.emplace_back(r, std::u32string());
                }
            }
            if (!solutions.empty()) {
                if (solutions.size() > 1) {
                    Parts p;
                    p.kind = Parts::Multiple;
                    return p;
                }
                Parts p;
                p.kind = Parts::Single;
                p.root = solutions[0].first;
                p.suffix = solutions[0].second;
                return p;
            }
            return none;
        }
        return none;
    } else if (contains(ambiguous_, syl)) {
        // Python returns a LIST here -> downstream treats like Multiple
        Parts p;
        p.kind = Parts::Multiple;
        return p;
    } else {
        Parts p;  // exceptions: (syl, 'x')
        p.kind = Parts::Single;
        p.root = syl;
        p.suffix = U"x";
        return p;
    }
}

std::u32string BoSyl::getMingzhi(const std::u32string& syl) const {
    Parts c = getParts(syl);
    if (c.kind != Parts::Single) {
        if (!syl.empty() && syl.back() == U'ད') {
            Parts c2 = getParts(syl.substr(0, syl.size() - 1));
            if (c2.kind != Parts::Single) return U"";
            auto it = mingzhis_.find(c2.root);
            return it == mingzhis_.end() ? std::u32string() : it->second;
        }
        return U"";
    }
    auto it = mingzhis_.find(c.root);
    return it == mingzhis_.end() ? std::u32string() : it->second;
}

// hand-rolled equivalent of the get_info regex (std::regex is not reliable
// over non-ASCII): mingzhi + [ྱྲླྭྷ]? [ིེོུ]? ( འ?[ིོུ]?ར?ས? | འ[མང] |
// འོའ[མང] | འིའ[ོམང] ) anchored at the end, searched at every position.
namespace {

bool inSet(char32_t c, const char32_t* set) {
    for (const char32_t* p = set; *p; ++p)
        if (*p == c) return true;
    return false;
}

bool thameAlt1(const std::u32string& r) {  // འ?[ིོུ]?ར?ས?$
    size_t i = 0;
    if (i < r.size() && r[i] == U'འ') ++i;
    if (i < r.size() && inSet(r[i], U"ིོུ")) ++i;
    if (i < r.size() && r[i] == U'ར') ++i;
    if (i < r.size() && r[i] == U'ས') ++i;
    return i == r.size();
}

bool thameTailAfterMingzhi(const std::u32string& rest) {
    // optional subjoined, optional vowel, then one of the alternatives;
    // try every combination of taking/skipping the optional prefixes
    for (int a = 0; a <= 1; ++a) {
        for (int v = 0; v <= 1; ++v) {
            size_t i = 0;
            if (a) {
                if (i < rest.size() && inSet(rest[i], U"ྱྲླྭྷ")) ++i;
                else continue;
            }
            if (v) {
                if (i < rest.size() && inSet(rest[i], U"ིེོུ")) ++i;
                else continue;
            }
            std::u32string r = rest.substr(i);
            if (thameAlt1(r)) return true;
            if (r.size() == 2 && r[0] == U'འ' && inSet(r[1], U"མང")) return true;
            if (r.size() == 4 && r[0] == U'འ' && r[1] == U'ོ' && r[2] == U'འ' &&
                inSet(r[3], U"མང"))
                return true;
            if (r.size() == 4 && r[0] == U'འ' && r[1] == U'ི' && r[2] == U'འ' &&
                inSet(r[3], U"ོམང"))
                return true;
        }
    }
    return false;
}

}  // namespace

std::u32string BoSyl::getInfo(const std::u32string& syl) const {
    std::u32string mingzhi = getMingzhi(syl);
    if (mingzhi.empty()) return U"";  // Python None
    if (contains(dadrag_, syl)) return U"dadrag";
    // re.search: mingzhi may match at any position, tail anchored at end
    for (size_t p = 0; p + mingzhi.size() <= syl.size(); ++p) {
        if (syl.compare(p, mingzhi.size(), mingzhi) == 0 &&
            thameTailAfterMingzhi(syl.substr(p + mingzhi.size())))
            return U"thame";
    }
    return syl;
}

bool BoSyl::isThame(const std::u32string& syl) const {
    return getInfo(syl) == U"thame";
}

bool BoSyl::isAffixable(const std::u32string& syl) const {
    if (!isThame(syl)) return false;
    static const char32_t* kEndings[] = {U"ར", U"ས", U"འི", U"འོ", U"མ", U"ང"};
    for (const char32_t* e : kEndings) {
        std::u32string end(e);
        if (syl.size() > end.size() &&
            syl.compare(syl.size() - end.size(), end.size(), end) == 0)
            return false;
    }
    return true;
}

std::vector<std::pair<std::u32string, Affixation>> BoSyl::getAllAffixed(
    const std::u32string& sylIn) const {
    if (!isAffixable(sylIn)) return {};
    std::u32string syl = sylIn;
    bool aa = false;
    if (!syl.empty() && syl.back() == U'འ' && syl.size() > 1) {
        syl.pop_back();
        aa = true;
    }
    // botok's affix dict in insertion order
    static const struct {
        const char32_t* affix;
        int len;
        const char* type;
    } kAffixes[] = {
        {U"ར", 1, "la"},      {U"ས", 1, "gis"},     {U"འི", 2, "gi"},
        {U"འམ", 2, "am"},     {U"འང", 2, "ang"},    {U"འོ", 2, "o"},
        {U"འིའོ", 4, "gi+o"}, {U"འིའམ", 4, "gi+am"}, {U"འིའང", 4, "gi+ang"},
        {U"འོའམ", 4, "o+am"}, {U"འོའང", 4, "o+ang"},
    };
    std::vector<std::pair<std::u32string, Affixation>> out;
    for (const auto& a : kAffixes) {
        Affixation meta;
        meta.len = a.len;
        meta.type = a.type;
        meta.aa = aa;
        out.emplace_back(syl + a.affix, meta);
    }
    return out;
}

// -------------------------------------------------------------- has_skrt_syl
// Char classes expanded from the upstream regexes with sre_parse (exact
// codepoint sets; single-char classes are literals 0x0FB7 ྷ, 0x0FB2 ྲ,
// 0x0F39 ༹). re.search + empty-matching pads make each pattern equivalent
// to "core sequence appears anywhere in the syllable".
namespace {

struct CpRange { char32_t lo, hi; };

bool inRanges(char32_t c, const CpRange* r, size_t n) {
    for (size_t i = 0; i < n; ++i)
        if (c >= r[i].lo && c <= r[i].hi) return true;
    return false;
}

const CpRange kSkrtVow[] = {{0x0F71, 0x0F71}, {0x0F73, 0x0F73}, {0x0F75, 0x0F79},
                            {0x0F7B, 0x0F7B}, {0x0F7D, 0x0F83}};
const CpRange kGdbCons[] = {{0x0F42, 0x0F42}, {0x0F4C, 0x0F4C}, {0x0F51, 0x0F51},
                            {0x0F56, 0x0F56}, {0x0F5B, 0x0F5B}, {0x0F92, 0x0F92},
                            {0x0F9C, 0x0F9C}, {0x0FA1, 0x0FA1}, {0x0FA6, 0x0FA6},
                            {0x0FAB, 0x0FAB}};
const CpRange kShHa[] = {{0x0F64, 0x0F64}, {0x0F67, 0x0F67}};
const CpRange kB4Skrt[] = {{0x0F43, 0x0F43}, {0x0F4A, 0x0F4E}, {0x0F52, 0x0F52},
                           {0x0F57, 0x0F57}, {0x0F5C, 0x0F5C}, {0x0F65, 0x0F65},
                           {0x0F69, 0x0F6C}, {0x0F71, 0x0F71}, {0x0F73, 0x0F73},
                           {0x0F75, 0x0F79}, {0x0F7B, 0x0F7B}, {0x0F7D, 0x0F83},
                           {0x0F93, 0x0F93}, {0x0F9A, 0x0F9E}, {0x0FA2, 0x0FA2},
                           {0x0FA7, 0x0FA7}, {0x0FAC, 0x0FAC}, {0x0FB5, 0x0FB5},
                           {0x0FB9, 0x0FBC}};
const CpRange kR2First[] = {{0x0F40, 0x0F42}, {0x0F44, 0x0F49}, {0x0F4F, 0x0F51},
                            {0x0F53, 0x0F56}, {0x0F58, 0x0F5B}, {0x0F5D, 0x0F61},
                            {0x0F64, 0x0F64}, {0x0F67, 0x0F68}};
const CpRange kR2Second[] = {{0x0F90, 0x0FAC}, {0x0FAE, 0x0FB0}, {0x0FB4, 0x0FBC}};

#define NRANGES(x) (sizeof(x) / sizeof((x)[0]))

bool isSkrtSyl(const std::u32string& s) {
    for (size_t i = 0; i < s.size(); ++i) {
        char32_t c = s[i];
        if (inRanges(c, kSkrtVow, NRANGES(kSkrtVow))) return true;          // r1 b1
        if (i + 1 < s.size()) {
            if (inRanges(c, kGdbCons, NRANGES(kGdbCons)) && s[i + 1] == 0x0FB7)
                return true;                                                // r1 b2
            if (inRanges(c, kShHa, NRANGES(kShHa)) && s[i + 1] == 0x0FB2)
                return true;                                                // r1 b3
            if (inRanges(c, kR2First, NRANGES(kR2First)) &&
                inRanges(s[i + 1], kR2Second, NRANGES(kR2Second)))
                return true;                                                // r2
        }
        if (inRanges(c, kB4Skrt, NRANGES(kB4Skrt))) return true;            // r1 b4
        if (c == 0x0F39) return true;                                       // r3
    }
    return false;
}

}  // namespace

bool hasSkrtSyl(const std::u32string& word) {
    // strip tseks at both ends, split on tsek, any syllable matches
    size_t b = 0, e = word.size();
    while (b < e && word[b] == U'་') ++b;
    while (e > b && word[e - 1] == U'་') --e;
    std::u32string cur;
    for (size_t i = b; i <= e; ++i) {
        if (i == e || word[i] == U'་') {
            if (isSkrtSyl(cur)) return true;
            cur.clear();
        } else {
            cur.push_back(word[i]);
        }
    }
    return false;
}

// --------------------------------------------------------------------- Trie

static std::string joinSyls(const std::vector<std::string>& syls) {
    // syl + tsek unless the syllable ends with NAMCHE ཿ
    static const std::string kNamche = u32to8(U"ཿ");
    static const std::string kTsek = u32to8(U"་");
    std::string out;
    for (const std::string& s : syls) {
        out += s;
        bool namche = s.size() >= kNamche.size() &&
                      s.compare(s.size() - kNamche.size(), kNamche.size(),
                                kNamche) == 0;
        if (!namche) out += kTsek;
    }
    return out;
}

const std::vector<Trie::Inflected>* Trie::getInflected(const std::string& word) {
    auto cached = tmpInflected_.find(word);
    if (cached != tmpInflected_.end()) return &cached->second;

    ChunkFramework cf(table_, u8to32(word));
    std::vector<std::string> syls = cf.getSyls();
    std::vector<Inflected> result;
    if (!syls.empty()) {
        Inflected base;
        base.syls = syls;
        result.push_back(base);
        auto affixed = bosyl_.getAllAffixed(u8to32(syls.back()));
        for (const auto& [infl, data] : affixed) {
            Inflected e;
            e.syls = std::vector<std::string>(syls.begin(), syls.end() - 1);
            e.syls.push_back(u32to8(infl));
            e.affixation = data;
            result.push_back(e);
        }
    }
    auto it = tmpInflected_.emplace(word, std::move(result)).first;
    return &it->second;  // empty vector = Python None
}

void Trie::inflectNModifyTrie(const std::string& word, bool deactivate,
                              bool skrt) {
    const auto* inflected = getInflected(word);
    if (inflected->empty()) return;
    for (const auto& infl : *inflected) {
        if (deactivate) {
            BasicTrie::deactivate(infl.syls);
        } else {
            DataPatch patch;
            patch.affixation = infl.affixation;
            if (skrt) patch.skrt = true;
            add(infl.syls, &patch);
        }
    }
}

void Trie::addNonInflectible(const std::string& word) {
    ChunkFramework cf(table_, u8to32(word));
    std::vector<std::string> syls = cf.getSyls();
    if (syls.empty()) return;
    add(syls);
}

void Trie::inflectNAddData(const std::string& line) {
    // __parse_line: '\t' preferred, then ',', else bare form —
    // [UPSTREAM QUIRK] the bare-form branch puts "NOPOS" into the LEMMA
    // slot (fields[2]), which then washes out to an empty-string lemma.
    std::optional<std::string> fields[5];
    char sep = 0;
    if (line.find('\t') != std::string::npos) sep = '\t';
    else if (line.find(',') != std::string::npos) sep = ',';
    if (!sep) {
        fields[0] = line;
        fields[2] = "NOPOS";
    } else {
        std::stringstream ss(line);
        std::string cell;
        int num = 0;
        while (num < 5 && std::getline(ss, cell, sep)) {
            if (!cell.empty()) fields[num] = cell;
            ++num;
        }
    }
    const auto& form = fields[0];
    const auto& pos = fields[1];
    auto lemma = fields[2];
    const auto& sense = fields[3];
    std::optional<long> freq;
    if (fields[4]) freq = std::stol(*fields[4]);
    if (lemma) {
        ChunkFramework cf(table_, u8to32(*lemma));
        lemma = joinSyls(cf.getSyls());  // may become "" (kept, as upstream)
    }

    const auto* inflected = getInflected(form ? *form : std::string());
    if (inflected->empty()) return;  // not Tibetan
    for (const auto& infl : *inflected) {
        Sense data;
        data.lemma = lemma;
        data.pos = pos;
        data.freq = freq;
        data.sense = sense;
        data.affixed = bool(infl.affixation);
        addData(infl.syls, data);
    }
}

}  // namespace botok
}  // namespace allcore
