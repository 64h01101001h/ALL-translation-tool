// Port of engines/pron_engine.py — wylie → GMR-convention phonetics.
// Transcribed table-for-table and branch-for-branch from the canonical file
// (validated 42/42 + card-attested set). Proven by engines_battery C/D:
// full-dictionary diff against the Python engine's reference output.
//
// Internal markers: 'B' final-b placeholder (as in Python); '\x04' stands in
// for Python's 'β' (b-before-s); '\x03' for the U+02D0 vowel-protector.
#include <algorithm>
#include <cctype>
#include <cstring>
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

const char BETA = '\x04';   // Python 'β'
const char MARK = '\x03';   // Python chr(0x2D0)

const map<string, string>& clusters() {
    static const map<string, string> m = {
        {"by", "j"},   {"sby", "j"},  {"dby", "y"},  {"phy", "ch"}, {"py", "ch"},
        {"spy", "ch"}, {"dpy", "ch"}, {"my", "ny"},  {"smy", "ny"}, {"dmy", "ny"},
        {"ky", "ky"},  {"sky", "ky"}, {"bky", "ky"}, {"rky", "ky"}, {"bsky", "ky"},
        {"khy", "ky"}, {"mkhy", "ky"},
        {"gy", "gy"},  {"rgy", "gy"}, {"sgy", "gy"}, {"bgy", "gy"}, {"brgy", "gy"},
        {"bsgy", "gy"},{"mgy", "gy"}, {"dgy", "gy"},
        {"kr", "tr"},  {"bkr", "tr"}, {"skr", "tr"}, {"dkr", "tr"}, {"khr", "tr"},
        {"mkhr", "tr"},{"gr", "dr"},  {"sgr", "dr"}, {"bgr", "dr"}, {"dgr", "dr"},
        {"mgr", "dr"}, {"bsgr", "dr"},{"grw", "dr"},
        {"pr", "tr"},  {"spr", "tr"}, {"dpr", "tr"}, {"phr", "tr"}, {"br", "dr"},
        {"dbr", "dr"}, {"sbr", "dr"}, {"dr", "dr"},  {"sdr", "dr"}, {"mdr", "dr"},
        {"tr", "tr"},
        {"sr", "s"},   {"bsr", "s"},  {"rj", "j"},   {"brj", "j"},  {"zl", "d"},
        {"bzl", "d"},  {"sl", "l"},   {"bsl", "l"},  {"bl", "l"},   {"rl", "l"},
        {"kl", "l"},   {"gl", "l"},
        {"lh", "hl"},  {"db", "w"},   {"dw", "d"},   {"tw", "t"},   {"tsw", "ts"},
        {"rw", "r"},   {"shw", "sh"}, {"sw", "s"},   {"zhw", "sh"},
        {"rm", "m"},   {"sm", "m"},   {"dm", "m"},   {"rn", "n"},   {"sn", "n"},
        {"gn", "n"},   {"mn", "n"},   {"brn", "n"},  {"bsn", "n"},
        {"rny", "ny"}, {"sny", "ny"}, {"gny", "ny"}, {"mny", "ny"}, {"brny", "ny"},
        {"bsny", "ny"},
        {"rng", "ng"}, {"sng", "ng"}, {"dng", "ng"}, {"brng", "ng"},{"bsng", "ng"},
        {"sts", "ts"}, {"rts", "ts"}, {"brts", "ts"},{"bsts", "ts"},
        {"rdz", "dz"}, {"brdz", "dz"},{"rd", "d"},   {"sd", "d"},   {"gd", "d"},
        {"bd", "d"},   {"brd", "d"},  {"bsd", "d"},
        {"rb", "b"},   {"sb", "b"},   {"rg", "g"},   {"sg", "g"},   {"dg", "g"},
        {"bg", "g"},   {"brg", "g"},  {"bsg", "g"},  {"mg", "g"},
        {"rk", "k"},   {"sk", "k"},   {"bk", "k"},   {"brk", "k"},  {"bsk", "k"},
        {"dk", "k"},
        {"rt", "t"},   {"st", "t"},   {"gt", "t"},   {"bt", "t"},   {"brt", "t"},
        {"bst", "t"},  {"mth", "t"},  {"mkh", "k"},  {"gts", "ts"}, {"bts", "ts"},
        {"btz", "ts"},
        {"gz", "s"},   {"bz", "s"},   {"gsh", "sh"}, {"bsh", "sh"}, {"gs", "s"},
        {"bs", "s"},   {"dp", "p"},   {"sp", "p"},   {"dph", "p"},
        {"mch", "ch"}, {"mtsh", "ts"},{"gc", "ch"},  {"bc", "ch"},  {"lc", "ch"},
        {"lj", "j"},   {"mj", "j"},   {"mdz", "dz"}, {"md", "d"},
        {"ld", "d"},   {"lt", "t"},   {"lb", "b"},   {"lp", "p"},   {"lg", "g"},
        {"lk", "k"},   {"lng", "ng"},
    };
    return m;
}

const map<string, string>& simple() {
    static const map<string, string> m = {
        {"k", "k"},   {"kh", "k"},  {"g", "g"},   {"ng", "ng"}, {"c", "ch"},
        {"ch", "ch"}, {"j", "j"},   {"ny", "ny"}, {"t", "t"},   {"th", "t"},
        {"d", "d"},   {"n", "n"},   {"p", "p"},   {"ph", "p"},  {"b", "b"},
        {"m", "m"},   {"ts", "ts"}, {"tsh", "ts"},{"dz", "dz"}, {"w", "w"},
        {"zh", "sh"}, {"z", "s"},   {"'", ""},    {"y", "y"},   {"r", "r"},
        {"l", "l"},   {"sh", "sh"}, {"s", "s"},   {"h", "h"},   {"", ""},
        {"g-y", "y"},
    };
    return m;
}

const map<string, string>& prenasalSub() {
    static const map<string, string> m = {
        {"'g", "ng"},  {"'j", "nj"},  {"'d", "nd"},  {"'b", "mb"},  {"'dz", "ndz"},
        {"mg", "ng"},  {"mj", "nj"},  {"md", "nd"},  {"mdz", "ndz"},{"ld", "nd"},
        {"lj", "nj"},  {"'gy", "ngy"},{"zl", "nd"},  {"'gr", "ndr"},{"'khr", "tr"},
        {"'kh", "k"},  {"'ph", "p"},  {"'ch", "ch"}, {"'th", "t"},  {"'ts", "ts"},
        {"'dr", "ndr"},{"'by", "nj"}, {"'phy", "ch"},{"'khy", "ky"},
    };
    return m;
}

const map<string, string>& firstSylApos() {
    static const map<string, string> m = {
        {"'g", "g"},   {"'j", "j"},   {"'d", "d"},   {"'b", "b"},   {"'dz", "dz"},
        {"'gy", "gy"}, {"'gr", "dr"}, {"'khr", "tr"},{"'kh", "k"},  {"'ph", "p"},
        {"'ch", "ch"}, {"'th", "t"},  {"'ts", "ts"}, {"'dr", "dr"}, {"'by", "j"},
        {"'phy", "ch"},{"'khy", "ky"},
    };
    return m;
}

const set<string>& particles() {
    static const set<string> s = {"kyi", "gyi",  "gi",   "yi",   "kyis", "gyis",
                                  "gis", "yis",  "dang", "ni",   "yang", "kyang",
                                  "'ang","la",   "nas",  "las",  "na",   "rnams",
                                  "tsam","du",   "ru",   "ste",  "te",   "de"};
    return s;
}
const set<string>& formative() {
    static const set<string> s = {"pa",  "ba",  "po",   "bo",   "mo",  "wa",
                                  "pa'i","ba'i","po'i", "bo'i", "par", "bar",
                                  "pas", "bas", "pa'o"};
    return s;
}
const map<string, string>& sylOverride() {
    static const map<string, string> m = {{"ston", "ton"}, {"dkon", "kon"},
                                          {"thob", "top"}, {"grub", "drup"},
                                          {"sgrib", "drip"}};
    return m;
}

bool isVowel(char c) {
    return c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u';
}

// parse_syl: protect '<vowel> pairs, split at first vowel
bool parseSyl(const string& syl, string& onset, char& vowel, string& coda) {
    string s = syl;
    for (char v : {'a', 'e', 'i', 'o', 'u'}) {
        const string from = string("'") + v, to = string(1, MARK) + v;
        size_t pos = 0;
        while ((pos = s.find(from, pos)) != string::npos) {
            s.replace(pos, 2, to);
            pos += 2;
        }
    }
    size_t vi = string::npos;
    for (size_t i = 0; i < s.size(); ++i)
        if (isVowel(s[i])) { vi = i; break; }
    if (vi == string::npos) return false;
    auto unmark = [](string x) {
        for (auto& c : x)
            if (c == MARK) c = '\'';
        return x;
    };
    onset = unmark(s.substr(0, vi));
    vowel = s[vi];
    coda = unmark(s.substr(vi + 1));
    return true;
}

string pronOnset(const string& onset, bool subsequent) {
    const auto& table = subsequent ? prenasalSub() : firstSylApos();
    auto it = table.find(onset);
    if (it != table.end()) return it->second;
    auto ic = clusters().find(onset);
    if (ic != clusters().end()) return ic->second;
    auto is = simple().find(onset);
    if (is != simple().end()) return is->second;
    for (const char* p : {"b", "d", "g", "m", "'"}) {
        const size_t pl = strlen(p);
        if (onset.size() > pl && onset.compare(0, pl, p) == 0) {
            const string r = onset.substr(pl);
            auto rc = clusters().find(r);
            if (rc != clusters().end()) return rc->second;
            auto rs = simple().find(r);
            if (rs != simple().end()) return rs->second;
        }
    }
    return onset;
}

string pronSyllable(const string& syl, bool subsequent, bool prenasal_ok) {
    string onset, coda;
    char vowel;
    if (!parseSyl(syl, onset, vowel, coda)) return syl;
    string on = pronOnset(onset, subsequent && prenasal_ok);
    bool gen = false;
    if (coda == "'i") { coda.clear(); gen = true; }
    if (coda == "'o" || coda == "'u" || coda == "'a" || coda == "'e") coda.clear();
    if (!coda.empty() && coda[0] == '\'') coda = coda.substr(1);
    string suf1 = (coda.compare(0, 2, "ng") == 0) ? "ng"
                  : (coda.empty() ? "" : coda.substr(0, 1));
    string suf2 = coda.substr(suf1.size());
    char v = vowel;
    if (suf1 == "d" || suf1 == "n" || suf1 == "l" || suf1 == "s") {
        if (v == 'a') v = 'e';
        else if (v == 'o') v = 'u';
    }
    static const map<string, string> sufmap = {
        {"g", "k"}, {"b", "B"}, {"d", ""}, {"s", ""},  {"n", "n"},
        {"m", "m"}, {"r", "r"}, {"l", "l"},{"ng", "ng"},{"", ""}};
    auto sm = sufmap.find(suf1);
    string sfx = sm != sufmap.end() ? sm->second : "";
    if (sfx == "B" && suf2.compare(0, 1, "s") == 0) sfx = string(1, BETA);
    if (on == "w" && onset == "db" && v == 'u') on.clear();
    return on + string(1, v) + sfx + (gen ? "y" : "");
}

vector<vector<string>> segment(const vector<string>& syls) {
    vector<vector<string>> words;
    vector<string> cur;
    int content = 0;
    const size_t n = syls.size();
    size_t i = 0;
    while (i < n) {
        const string& s = syls[i];
        if (i > 0 && s == "tu" && !cur.empty()) {          // ENCLITIC_BACK
            cur.push_back(s);
            words.push_back(cur);
            cur.clear();
            content = 0;
            ++i;
            continue;
        }
        if (i > 0 && s == "su" && !cur.empty()) {          // ENCLITIC_HYPH
            cur.push_back("-" + s);
            words.push_back(cur);
            cur.clear();
            content = 0;
            ++i;
            continue;
        }
        if ((s == "gyis" || s == "gyi") && !cur.empty() &&
            (cur.back() == "rim" || cur.back() == "zil")) {  // ADV_GYIS_HEADS
            cur.push_back(s);
            words.push_back(cur);
            cur.clear();
            content = 0;
            ++i;
            continue;
        }
        if (particles().count(s) && i > 0) {
            if (!cur.empty()) { words.push_back(cur); cur.clear(); content = 0; }
            words.push_back({s});
            ++i;
            continue;
        }
        if ((s == "ma" || s == "mi") && i + 1 < n) {       // NEG_FORWARD
            if (!cur.empty()) { words.push_back(cur); cur.clear(); content = 0; }
            vector<string> neg = {s, syls[i + 1]};
            i += 2;
            while (i < n && formative().count(syls[i])) neg.push_back(syls[i++]);
            words.push_back(neg);
            continue;
        }
        cur.push_back(s);
        ++content;
        if (s.size() >= 2 && s.compare(s.size() - 2, 2, "'i") == 0) {
            words.push_back(cur);
            cur.clear();
            content = 0;
            ++i;
            continue;
        }
        const string* nxt = (i + 1 < n) ? &syls[i + 1] : nullptr;
        if (content >= 2) {
            const bool nxtBinds =
                nxt && ((nxt->size() >= 2 &&
                         nxt->compare(nxt->size() - 2, 2, "'i") == 0) ||
                        formative().count(*nxt)) &&
                cur.size() < 3;
            if (!nxtBinds) {
                words.push_back(cur);
                cur.clear();
                content = 0;
            }
        }
        ++i;
    }
    if (!cur.empty()) words.push_back(cur);
    return words;
}

bool inVoiceless(const string& x) {
    static const set<string> V = {"p", "t", "k", "ts", "ch", "s", "sh", "h"};
    return V.count(x) > 0;
}

string pronWord(const vector<string>& wsyls) {
    vector<string> res;
    for (size_t j = 0; j < wsyls.size(); ++j) {
        const string& s = wsyls[j];
        string prev = res.empty() ? "" : res.back();
        const bool prev_open =
            !prev.empty() && prev.back() != 'p' && prev.back() != 'B' &&
            prev.back() != BETA;
        string p;
        if (j > 0 && (s == "ba" || s == "bo" || s == "bar" || s == "bas" ||
                      s == "ba'i" || s == "bo'i")) {
            static const map<string, string> M = {{"ba", "wa"},   {"bo", "wo"},
                                                  {"bar", "war"}, {"bas", "we"},
                                                  {"ba'i", "way"},{"bo'i", "woy"}};
            p = M.at(s);
        } else {
            const bool hy = !s.empty() && s[0] == '-';
            const string s0 = hy ? s.substr(1) : s;
            auto ov = sylOverride().find(s0);
            if (ov != sylOverride().end() && j != 0) p = ov->second;
            else p = pronSyllable(s0, j > 0, prev_open);
            if (ov != sylOverride().end() && j == 0) p = ov->second;
            if (hy) p = "-" + p;
        }
        if (!res.empty()) {
            if (!prev.empty() && (prev.back() == 'B' || prev.back() == BETA)) {
                const string p2 = p.substr(0, 2);
                const string nxt0 =
                    (p2 == "ts" || p2 == "ch" || p2 == "sh") ? p2 : p.substr(0, 1);
                res.back() = prev.substr(0, prev.size() - 1) +
                             (inVoiceless(nxt0) ? "p" : "b");
                prev = res.back();
            }
            const bool prevK = !prev.empty() && prev.back() == 'k';
            if (prevK && p.compare(0, 2, "gy") == 0) {
                // pass
            } else if (prevK && p.compare(0, 1, "g") == 0) {
                p = "k" + p.substr(1);
            }
            if (!prev.empty() && prev.back() == 'k' && p.compare(0, 1, "k") == 0) {
                res.back() = prev.substr(0, prev.size() - 1);
                prev = res.back();
            }
            if (prev.size() >= 2 && prev.compare(prev.size() - 2, 2, "ng") == 0 &&
                p.compare(0, 2, "gy") == 0) {
                p = p.substr(1);
            }
            if (!prev.empty() && prev.back() == 'k' && p.compare(0, 2, "ng") == 0) {
                res.back() = prev.substr(0, prev.size() - 1);
                prev = res.back();
            }
        }
        res.push_back(p);
    }
    if (!res.empty() && !res.back().empty() && res.back().back() == 'B')
        res.back() = res.back().substr(0, res.back().size() - 1) + "b";
    else if (!res.empty() && !res.back().empty() && res.back().back() == BETA)
        res.back() = res.back().substr(0, res.back().size() - 1) + "p";

    static const set<std::pair<string, string>> HY = {
        {"chos", "sku"}, {"gzugs", "sku"}, {"sprul", "sku"}, {"longs", "sku"}};
    const bool hyph =
        wsyls.size() == 2 && HY.count({wsyls[0], wsyls[1]}) > 0;
    string out;
    for (size_t i = 0; i < res.size(); ++i) {
        if (hyph && i) out += "-";
        out += res[i];
    }
    return out;
}

}  // namespace

namespace {
// pronounce()'s input normalization, shared verbatim with the
// segmented view below
vector<string> normalizeSyls(const std::string& wylie) {
    string w;
    w.reserve(wylie.size());
    for (size_t i = 0; i < wylie.size(); ++i) {
        unsigned char c = wylie[i];
        if (c == '+') { w += ' '; continue; }
        if (c == '/') continue;
        // U+00B7 middle dot → separator (python splits on ·)
        if (c == 0xC2 && i + 1 < wylie.size() &&
            (unsigned char)wylie[i + 1] == 0xB7) {
            w += ' ';
            ++i;
            continue;
        }
        w += (char)std::tolower(c);
    }
    // split on whitespace
    vector<string> syls;
    string cur;
    for (char c : w) {
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
            if (!cur.empty()) { syls.push_back(cur); cur.clear(); }
        } else {
            cur += c;
        }
    }
    if (!cur.empty()) syls.push_back(cur);
    return syls;
}
}  // namespace

std::string pronounce(const std::string& wylie) {
    string out;
    for (const auto& word : segment(normalizeSyls(wylie))) {
        const string p = pronWord(word);
        if (p.empty()) continue;
        if (!out.empty()) out += ' ';
        out += p;
    }
    return out;
}

std::vector<PronSegWord> pronounceSegmented(const std::string& wylie) {
    std::vector<PronSegWord> out;
    int consumed = 0;
    for (const auto& word : segment(normalizeSyls(wylie))) {
        PronSegWord w;
        w.pron = pronWord(word);
        w.syl_beg = consumed;
        consumed += (int)word.size();
        w.syl_end = consumed;
        out.push_back(w);   // empty prons kept: every syllable stays
                            // accounted for
    }
    return out;
}

}  // namespace allcore
