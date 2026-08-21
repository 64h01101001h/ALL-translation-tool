// Port of engines/hgm_tools.py::acip_to_ewts — transcribed line for line.
// Protection markers ⟦⟧ from the Python become \x01/\x02 bytes here.
#include <cctype>
#include <string>

#include "allcore/engines.h"

namespace allcore {
namespace {

const char OPEN = '\x01', CLOSE = '\x02';

void replaceAll(std::string& s, const std::string& from, const std::string& to) {
    size_t pos = 0;
    while ((pos = s.find(from, pos)) != std::string::npos) {
        s.replace(pos, from.size(), to);
        pos += to.size();
    }
}

std::string prot(const std::string& x) { return OPEN + x + CLOSE; }

}  // namespace

std::string acipToEwts(const std::string& acip) {
    // s = acip.strip().strip(',').strip()
    auto trim = [](std::string v, const char* set) {
        size_t b = v.find_first_not_of(set);
        if (b == std::string::npos) return std::string();
        size_t e = v.find_last_not_of(set);
        return v.substr(b, e - b + 1);
    };
    std::string s = trim(trim(trim(acip, " \t\r\n"), ","), " \t\r\n");

    // re.sub(r'\{[a-zA-Z+]+\}', '', s)
    {
        std::string o;
        for (size_t i = 0; i < s.size();) {
            if (s[i] == '{') {
                size_t j = i + 1;
                while (j < s.size() &&
                       (std::isalpha((unsigned char)s[j]) || s[j] == '+'))
                    ++j;
                if (j < s.size() && s[j] == '}' && j > i + 1) { i = j + 1; continue; }
            }
            o += s[i++];
        }
        s = o;
    }

    replaceAll(s, "sh", prot("Sh"));
    replaceAll(s, "th", prot("Th"));
    replaceAll(s, "t", prot("T"));
    replaceAll(s, "d", prot("D"));
    replaceAll(s, "n", prot("N"));
    replaceAll(s, "s", prot("S"));
    replaceAll(s, "m", prot("M"));
    replaceAll(s, "TS", prot("tsh"));
    replaceAll(s, "TZ", prot("ts"));

    // (?<=[BCDGHJKLMNPRSTVWYZ⟧])'([AEIOU]) → protected vowel (drop apostrophe)
    {
        static const std::string CONS = "BCDGHJKLMNPRSTVWYZ";
        std::string o;
        for (size_t i = 0; i < s.size(); ++i) {
            if (s[i] == '\'' && i + 1 < s.size() && i > 0 &&
                std::string("AEIOU").find(s[i + 1]) != std::string::npos &&
                (CONS.find(s[i - 1]) != std::string::npos || s[i - 1] == CLOSE)) {
                o += OPEN;
                o += s[i + 1];
                o += CLOSE;
                ++i;
            } else {
                o += s[i];
            }
        }
        s = o;
    }

    // case-fold outside protection markers
    std::string out;
    bool inProt = false;
    for (char c : s) {
        if (c == OPEN) { inProt = true; continue; }
        if (c == CLOSE) { inProt = false; continue; }
        out += inProt ? c : (char)std::tolower((unsigned char)c);
    }

    // collapse whitespace, trim
    std::string collapsed;
    bool space = false;
    for (char c : out) {
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r') { space = true; continue; }
        if (space && !collapsed.empty()) collapsed += ' ';
        space = false;
        collapsed += c;
    }
    return collapsed;
}



// ewtsToAcip — the INVERSE of acipToEwts (a new engine, not a port:
// no canonical reverse exists). Proven by ROUND-TRIP battery in
// engines_battery: acipToEwts(ewtsToAcip(w)) must reproduce w across
// the full dictionary. Mirrors the forward transform's steps inverted,
// in the inverse order.
std::string ewtsToAcip(const std::string& ewts) {
    std::string s = ewts;
    // ORDER MATTERS (battery-caught): the capital-letter passes must run
    // BEFORE tsh/ts protection — otherwise the capitals inside the
    // freshly inserted TS/TZ markers get re-replaced and corrupted.
    // EWTS capital (retroflex/Sanskrit) letters -> ACIP lowercase.
    // "Sh"/"Th" before the bare letters.
    replaceAll(s, "Sh", prot("sh"));
    replaceAll(s, "Th", prot("th"));
    const char* caps = "TDNSM";
    for (const char* c = caps; *c; ++c) {
        std::string hi(1, *c);
        std::string lo(1, (char)std::tolower((unsigned char)*c));
        replaceAll(s, hi, prot(lo));
    }
    // EWTS long vowels A/I/U (kA -> K'A)
    replaceAll(s, "A", prot("'A"));
    replaceAll(s, "I", prot("'I"));
    replaceAll(s, "U", prot("'U"));
    // affricates (tsh before ts, exactly inverse of TS/TZ); their
    // capitals are inserted inside markers, untouched from here on
    replaceAll(s, "tsh", prot("TS"));
    replaceAll(s, "ts", prot("TZ"));
    // case-fold: uppercase everything outside protection markers
    std::string out;
    bool inProt = false;
    for (char c : s) {
        if (c == OPEN) { inProt = true; continue; }
        if (c == CLOSE) { inProt = false; continue; }
        out += inProt ? c : (char)std::toupper((unsigned char)c);
    }
    return out;
}

std::string tokenToEwts(const std::string& tok) {
    bool upper = false;
    for (char c : tok) upper |= (c >= 'A' && c <= 'Z');
    return upper ? acipToEwts(tok) : tok;
}

bool looksLikeWylie(const std::string& text) {
    size_t upper = 0, lower = 0;
    for (unsigned char c : text) {
        if (c >= 'A' && c <= 'Z') ++upper;
        else if (c >= 'a' && c <= 'z') ++lower;
    }
    // ACIP body text is uppercase-dominant even with lowercase
    // Sanskrit passages mixed in; wylie is lowercase throughout
    return lower > upper;
}

}  // namespace allcore
