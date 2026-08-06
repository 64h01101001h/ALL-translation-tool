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

}  // namespace allcore
