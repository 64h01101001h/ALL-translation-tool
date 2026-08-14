// THL Simplified Phonetic Transcription of Standard Tibetan
// (David Germano & Nicolas Tournadre, 2003-12-12; © 2003 Germano/
// Tournadre/THL — the rules are implemented as published facts and
// the system is credited by name in the UI).
//
// Implemented rule-for-rule from the captured standard:
// docs/research/THL/language_tools.md §2 (CAPTURED COMPLETE):
// the general principle, all sixteen Special Rules, the a-chung
// chain rules (14–16), Sanskrit folding (§7), diacritics policy
// (é ö ü only, §8), and the complete 19-word List of Exceptions
// (§12). Exceptional-pronunciation classes (§6) enter ONLY via
// that exception list — never generalized (rule 3: never guess).
//
// Battery: core/tests/thl_battery.cpp — the standard's own §11
// example list plus the rule-by-rule examples and the full
// exception list.
//
// DOCUMENTED POLICY (source contradiction): the published example
// list prints thun>thün, contradicting the standard's own rules
// 2+3 (th⇒t; u+n⇒ün) and its other th- examples (thabs>tap,
// thug pa>tukpa). We follow the rules: thun⇒tün.
//
// Unknown structures are flagged ⟨wylie⟩, never approximated.
#include <algorithm>
#include <cctype>
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

// ---- §12 List of Exceptions (complete, 19 entries) ------------
const map<string, string>& exceptions() {
    static const map<string, string> m = {
        {"skyabs 'gro", "kyamdro"},   {"dga' ldan", "ganden"},
        {"rgyal rtse", "gyantsé"},    {"chab mdo", "chamdo"},
        {"rta mgrin", "tamdrin"},     {"rten 'brel", "temdrel"},
        {"rdo rje", "dorjé"},         {"dpal ldan", "penden"},
        {"bla brang", "labrang"},     {"'bras ljongs", "drenjong"},
        {"'bri ru", "biru"},          {"sbra nag zhol", "banakzhöl"},
        {"me mda'", "menda"},         {"lam 'bras", "lamdré"},
        {"lha rje", "lharjé"},        {"lha bris pa", "lhapripa"},
        {"a mdo", "amdo"},            {"u rgyan", "urgyen"},
        {"o rgyan", "orgyen"},
    };
    return m;
}

// consonant units, longest-first for the tokenizer
const vector<string>& units() {
    static const vector<string> u = {
        "tsh", "ts", "dz", "zh", "sh", "ny", "ng", "kh", "ch",
        "th", "ph", "k", "g", "c", "j", "t", "d", "n", "p", "b",
        "m", "w", "z", "y", "r", "l", "s", "h", "'",
    };
    return u;
}

bool isVowelChar(char c) {
    return c == 'a' || c == 'i' || c == 'u' || c == 'e' || c == 'o';
}

// §7 Sanskrit fold: long vowels shorten, retroflexes de-retroflex,
// ś/ṣ ⇒ sh, anusvāra ⇒ m, visarga drops, v ⇒ b (badzra).
string sanskritFold(string s) {
    string out;
    for (size_t i = 0; i < s.size(); ++i) {
        const char c = s[i];
        switch (c) {
            case 'A': out += 'a'; break;
            case 'I': out += 'i'; break;
            case 'U': out += 'u'; break;
            case 'T': out += 't'; break;
            case 'D': out += 'd'; break;
            case 'N': out += 'n'; break;
            case 'S': out += "sh"; break;   // EWTS Sh handled too
            case 'M': out += 'm'; break;
            case 'H': break;                // visarga: kaH thog>Katok
            case 'v': out += 'b'; break;
            case '+': break;                // explicit stacks join
            case '~': break;
            default: out += c;
        }
    }
    // Sh → sh may have produced "shh"
    size_t p;
    while ((p = out.find("shh")) != string::npos) out.erase(p + 2, 1);
    // double vowels (aa ii uu) shorten
    string out2;
    for (size_t i = 0; i < out.size(); ++i) {
        if (!out2.empty() && isVowelChar(out[i]) &&
            out2.back() == out[i])
            continue;
        out2 += out[i];
    }
    return out2;
}

struct CodaItem {
    bool achung = false;   // a-chung chain item ('i 'u 'ang …)
    string cons;           // consonant unit (suffix), or
    string chainVowel;     // the chain's vowel
    string chainTail;      // consonants closing a chain (ng in 'ang)
};

struct Syl {
    bool ok = true;
    bool gDotPrefix = false;   // raw wrote g.y — g is a prefix
    string raw;
    vector<string> onset;      // consonant units before the vowel
    string vowel;              // a i u e o
    vector<CodaItem> coda;
    // resolved during onset mapping:
    string onsetPhon;
    bool achungPrefix = false;   // syllable begins with silent '
    string root;                 // root unit after strips
    string sub;                  // subscript y/r/l/w if any
};

bool tokenizeUnits(const string& s, size_t& pos,
                   vector<string>& out, bool stopAtVowel) {
    while (pos < s.size()) {
        if (s[pos] == '.') { ++pos; continue; }   // g.y separator
        if (stopAtVowel && isVowelChar(s[pos])) return true;
        bool matched = false;
        for (const auto& u : units()) {
            if (s.compare(pos, u.size(), u) == 0) {
                // avoid eating a digraph whose 2nd char is the
                // vowel's consonant? units are longest-first and
                // EWTS is unambiguous with '.' handled
                out.push_back(u);
                pos += u.size();
                matched = true;
                break;
            }
        }
        if (!matched) return false;
    }
    return true;
}

bool parseSyl(const string& raw0, Syl& sy) {
    sy.raw = raw0;
    string s = sanskritFold(raw0);
    sy.gDotPrefix = s.rfind("g.", 0) == 0;
    size_t pos = 0;
    if (!tokenizeUnits(s, pos, sy.onset, true)) return false;
    if (pos >= s.size()) {
        // no explicit vowel: EWTS always writes the vowel; a bare
        // consonant run is not a syllable we can speak
        return false;
    }
    sy.vowel = string(1, s[pos]);
    ++pos;
    // coda: consonant units and a-chung chains
    while (pos < s.size()) {
        if (s[pos] == '\'') {
            ++pos;
            if (pos < s.size() && isVowelChar(s[pos])) {
                CodaItem it;
                it.achung = true;
                it.chainVowel = string(1, s[pos]);
                ++pos;
                // chain-closing consonants ('ang, 'am, 'is …)
                vector<string> tail;
                size_t p2 = pos;
                size_t stop = s.find('\'', pos);
                string seg = s.substr(
                    pos, stop == string::npos ? string::npos
                                              : stop - pos);
                size_t sp = 0;
                vector<string> segUnits;
                if (!tokenizeUnits(seg, sp, segUnits, false))
                    return false;
                for (const auto& u : segUnits) it.chainTail += u;
                pos = p2 + seg.size();
                sy.coda.push_back(it);
            } else {
                // a-chung as plain (silent) suffix: bka' — record
                CodaItem it;
                it.cons = "'";
                sy.coda.push_back(it);
            }
            continue;
        }
        bool matched = false;
        for (const auto& u : units()) {
            if (s.compare(pos, u.size(), u) == 0 && u != "'") {
                CodaItem it;
                it.cons = u;
                sy.coda.push_back(it);
                pos += u.size();
                matched = true;
                break;
            }
        }
        if (!matched) return false;
    }
    return true;
}

// resolve the onset to its THL phonetic value (rules 1–2, 7–12,
// general principle: silent prefixes/superscripts drop)
bool mapOnset(Syl& sy) {
    vector<string> u = sy.onset;
    if (u.empty()) {
        // vowel-initial syllable (a mdo, o rgyan, u…)
        sy.onsetPhon = "";
        sy.root = "'";
        return true;
    }
    if (u.front() == "'") {
        if (u.size() == 1) {
            sy.onsetPhon = "";   // a-chung root carries the vowel
            sy.root = "'";
            return true;
        }
        sy.achungPrefix = true;
        u.erase(u.begin());
    }
    // db special family (rule 12)
    if (u.size() >= 2 && u[0] == "d" && u[1] == "b") {
        string tail;
        for (size_t i = 2; i < u.size(); ++i) tail += u[i];
        if (tail.empty()) { sy.onsetPhon = "w"; sy.root = "b"; return true; }
        if (tail == "y") { sy.onsetPhon = "y"; sy.root = "b"; sy.sub = "y"; return true; }
        if (tail == "r") { sy.onsetPhon = "r"; sy.root = "b"; sy.sub = "r"; return true; }
        return false;
    }
    // orthographic legality (the classical combinatorics): a
    // leading letter strips as prefix/superscript ONLY where
    // Tibetan orthography allows that combination — otherwise it
    // is the root of a subscript cluster (sr = s+r-sub, not
    // s-super+r). Sound rules then apply to what remains.
    static const map<string, set<string>> prefixRoots = {
        // g before y ONLY via the g.y dot (EWTS: gya = cluster,
        // g.ya = prefix) — handled by the gDotPrefix flag below
        {"g", {"c", "ny", "t", "d", "n", "ts", "zh", "z",
               "sh", "s"}},
        {"d", {"k", "g", "ng", "p", "b", "m", "ky", "gy", "py"}},
        {"b", {"k", "g", "c", "t", "d", "ts", "zh", "z", "sh",
               "s", "r", "l"}},   // b before r/l only as b+super
        {"m", {"kh", "g", "ng", "ch", "j", "ny", "th", "t", "d",
               "n", "tsh", "dz"}},
    };
    static const map<string, set<string>> superRoots = {
        {"r", {"k", "g", "ng", "j", "ny", "t", "d", "n", "b",
               "m", "ts", "dz", "l"}},
        {"l", {"k", "g", "ng", "c", "j", "t", "d", "p", "b",
               "h"}},
        {"s", {"k", "g", "ng", "ny", "t", "d", "n", "p", "b",
               "m", "ts", "l"}},   // sr = s + r-SUBSCRIPT (srog)
    };
    // l+h = the lh root (rule 11)
    if (u.size() >= 2 && u[0] == "l" && u[1] == "h") {
        sy.onsetPhon = "lh";
        sy.root = "h";
        return true;
    }
    auto nextIsLegal = [&](const map<string, set<string>>& table)
        -> bool {
        auto it = table.find(u[0]);
        if (it == table.end()) return false;
        // legality is judged against the immediate next unit, or
        // the unit after a legal superscript (brgyad: b before
        // r-super+g)
        if (it->second.count(u[1])) return true;
        if (u.size() >= 3) {
            auto s2 = superRoots.find(u[1]);
            if (s2 != superRoots.end() && s2->second.count(u[2]))
                return true;
        }
        return false;
    };
    // the g.y written prefix (yön class)
    if (sy.gDotPrefix && u.size() >= 2 && u[0] == "g")
        u.erase(u.begin());
    // strip one prefix (only when the remainder is legal)
    if (u.size() >= 2 && prefixRoots.count(u[0]) &&
        nextIsLegal(prefixRoots)) {
        // exception to the exception: b directly before r/l with a
        // vowel next is a SUBSCRIPT cluster (bras, blo) — only
        // strip b there when a further consonant follows (brgyad)
        const bool bCluster =
            u[0] == "b" && u.size() == 2 &&
            (u[1] == "r" || u[1] == "l");
        if (!bCluster) u.erase(u.begin());
    }
    // strip one superscript (only where that pairing exists)
    if (u.size() >= 2) {
        auto it = superRoots.find(u[0]);
        if (it != superRoots.end() && it->second.count(u[1]) &&
            !(u[0] == "l" && u[1] == "h"))
            u.erase(u.begin());
    }
    if (u.empty()) return false;
    string root = u[0];
    string sub = u.size() >= 2 ? u[1] : "";
    if (u.size() > 2) return false;   // no legal third element left
    sy.root = root;
    sy.sub = sub;
    // base values (general principle + rules 1–2)
    auto base = [](const string& r) -> string {
        if (r == "c" || r == "ch") return "ch";
        if (r == "th") return "t";
        if (r == "ph") return "p";
        if (r == "tsh") return "ts";
        if (r == "'") return "";
        return r;
    };
    if (sub.empty()) {
        sy.onsetPhon = base(root);
        return true;
    }
    if (sub == "y") {   // rules 7–8
        if (root == "p" || root == "ph") { sy.onsetPhon = "ch"; return true; }
        if (root == "b") { sy.onsetPhon = "j"; return true; }
        if (root == "m") { sy.onsetPhon = "ny"; return true; }
        if (root == "k" || root == "kh" || root == "g") {
            sy.onsetPhon = root + "y";
            return true;
        }
        return false;
    }
    if (sub == "r") {   // rule 9
        if (root == "k" || root == "p" || root == "t" ||
            root == "kh" || root == "ph" || root == "th") {
            sy.onsetPhon = "tr";
            return true;
        }
        if (root == "g" || root == "b" || root == "d") {
            sy.onsetPhon = "dr";
            return true;
        }
        if (root == "h") { sy.onsetPhon = "hr"; return true; }
        // unpronounced r-clusters just drop the r (mr, sr, …)
        sy.onsetPhon = base(root);
        return true;
    }
    if (sub == "l") {   // rule 10
        sy.onsetPhon = root == "z" ? "d" : "l";
        return true;
    }
    if (sub == "w") {   // wazur is silent
        sy.onsetPhon = base(root);
        return true;
    }
    return false;
}

const set<string>& legalSuffix() {
    static const set<string> s = {"g", "ng", "d", "n", "b",
                                  "m", "'", "r", "l", "s"};
    return s;
}

struct Assembled {
    string body;        // onset+vowel(+chains)
    string suffix;      // pronounced suffix, post-devoicing
    bool endsOpenE = false;   // final sound is plain e (for é)
    bool plainBa = false;     // rule 6 candidate (bare b root)
};

// build one syllable's sound; wordFinal drives rules 4/6/16 finals
bool assemble(Syl& sy, bool last, bool nasalNext, char nasal,
              Assembled& out) {
    if (!mapOnset(sy)) return false;
    string v = sy.vowel;
    // gather suffix consonants (first coda cons + silent post s/d)
    string suf, suf2;
    vector<const CodaItem*> chains;
    for (const auto& it : sy.coda) {
        if (it.achung) {
            chains.push_back(&it);
        } else if (suf.empty()) {
            suf = it.cons;
        } else if (suf2.empty()) {
            suf2 = it.cons;
        } else {
            return false;
        }
    }
    if (!suf.empty() && !legalSuffix().count(suf)) return false;
    if (!suf2.empty() && suf2 != "s" && suf2 != "d") return false;
    // rules 3–4: umlaut before d n l s
    const bool umlaut =
        (suf == "d" || suf == "n" || suf == "l" || suf == "s");
    string vres = v;
    if (umlaut) {
        if (v == "o") vres = "ö";
        else if (v == "u") vres = "ü";
        else if (v == "a") vres = "e";
    }
    // suffix pronunciation (rule 5 devoicing; d ' s silent)
    string sufPhon;
    if (suf == "g") sufPhon = "k";
    else if (suf == "b") sufPhon = "p";
    else if (suf == "ng" || suf == "n" || suf == "m" ||
             suf == "r" || suf == "l")
        sufPhon = suf;
    // rules 14–16: a-chung chains modify the vowel
    string vseq = vres;
    for (const auto* ch : chains) {
        const string cv = ch->chainVowel;
        if (cv == "i") {
            // rule 16 (genitive) / 15 (i'i): transform last vowel
            char lastV = 0;
            // find last vowel-ish in vseq (é ö ü are multibyte)
            if (vseq.size() >= 2 &&
                (vseq.substr(vseq.size() - 2) == "é" ||
                 vseq.substr(vseq.size() - 2) == "ö" ||
                 vseq.substr(vseq.size() - 2) == "ü"))
                lastV = 'X';
            else
                lastV = vseq.back();
            if (lastV == 'a') vseq.replace(vseq.size() - 1, 1, "é");
            else if (lastV == 'o') vseq.replace(vseq.size() - 1, 1, "ö");
            else if (lastV == 'u') vseq.replace(vseq.size() - 1, 1, "ü");
            else if (lastV == 'e') vseq.replace(vseq.size() - 1, 1, "é");
            else if (lastV == 'i') { /* gri'i>dri: merge */ }
            else return false;
        } else if (!vseq.empty() &&
                   vseq.back() == cv[0]) {
            // rule 15: same vowel merges (pa'ang>pang)
        } else {
            vseq += cv;   // rule 14: le'u>leu
        }
        if (!ch->chainTail.empty()) {
            if (!sufPhon.empty()) return false;
            sufPhon = ch->chainTail;   // 'ang, 'am closers
        }
    }
    // rule 13: nasalization replaces this syllable's suffix
    if (nasalNext) sufPhon = string(1, nasal);
    out.body = sy.onsetPhon + vseq;
    out.suffix = sufPhon;
    // rule 6 concerns only ba/bo (with their r / 'i extensions) —
    // never bu/be/bi
    out.plainBa = sy.onset.size() == 1 && sy.onset[0] == "b" &&
                  (sy.vowel == "a" || sy.vowel == "o");
    // é candidacy: final sound is a plain e with nothing after it
    out.endsOpenE = sufPhon.empty() && !vseq.empty() &&
                    vseq.back() == 'e';
    (void)last;
    return true;
}

string flagged(const string& wylie) { return "⟨" + wylie + "⟩"; }

string lower(string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
        return std::tolower(c);
    });
    return s;
}

}  // namespace

std::string thlPhonetics(const std::string& wylie, bool wordFinal) {
    // Sanskrit folds FIRST (they live in the capitals), then
    // normalize: collapse whitespace, lowercase, strip shads
    string w;
    for (char c : sanskritFold(wylie)) {
        if (c == '/' || c == '|' || c == ',' || c == ';') continue;
        w += c;
    }
    w = lower(w);
    // trim + collapse
    string clean;
    bool sp = true;
    for (char c : w) {
        if (c == ' ' || c == '\t' || c == '\n') {
            if (!sp) clean += ' ';
            sp = true;
        } else {
            clean += c;
            sp = false;
        }
    }
    while (!clean.empty() && clean.back() == ' ') clean.pop_back();
    if (clean.empty()) return "";
    // §12 exceptions first — whole-word lookup
    auto ex = exceptions().find(clean);
    if (ex != exceptions().end()) return ex->second;

    // split syllables
    vector<string> syls;
    string cur;
    for (char c : clean) {
        if (c == ' ') {
            if (!cur.empty()) syls.push_back(cur);
            cur.clear();
        } else {
            cur += c;
        }
    }
    if (!cur.empty()) syls.push_back(cur);

    // parse all
    vector<Syl> parsed(syls.size());
    for (size_t i = 0; i < syls.size(); ++i)
        if (!parseSyl(syls[i], parsed[i])) return flagged(clean);

    // rule 13 pre-pass: which syllables nasalize their predecessor
    vector<char> nasalAfter(syls.size(), 0);
    for (size_t i = 0; i + 1 < syls.size(); ++i) {
        Syl& nx = parsed[i + 1];
        if (nx.onset.size() >= 2 && nx.onset[0] == "'") {
            // choose m for ph/b roots WITHOUT y/r subscript
            const string& r = nx.onset[1];
            const string s2 =
                nx.onset.size() >= 3 ? nx.onset[2] : "";
            const bool labial = (r == "ph" || r == "b") &&
                                s2 != "y" && s2 != "r";
            nasalAfter[i] = labial ? 'm' : 'n';
        }
    }

    string out;
    for (size_t i = 0; i < syls.size(); ++i) {
        Assembled a;
        if (!assemble(parsed[i], i + 1 == syls.size(),
                      nasalAfter[i] != 0, nasalAfter[i], a))
            return flagged(clean);
        const bool isLast = (i + 1 == syls.size());
        string body = a.body, sufPhon = a.suffix;
        if (wordFinal && isLast) {
            // rule 6: final ba/bo family speaks as w — but only
            // as a NON-INITIAL syllable: a monosyllable's b is
            // word-initial and stays b (bod = bö, never wö).
            // Found by the Espel cross-oracle (2026-08-14): a
            // 21-word divergence class; the 139-example standard
            // battery contained no bare monosyllabic b-word.
            const string raw = parsed[i].raw;
            if (a.plainBa && i > 0 && !body.empty() &&
                body[0] == 'b')
                body[0] = 'w';
            // rule 4/16: final open e ⇒ é
            if (sufPhon.empty() && a.endsOpenE)
                body.replace(body.size() - 1, 1, "é");
        }
        out += body + sufPhon;
    }
    return out;
}

}  // namespace allcore
