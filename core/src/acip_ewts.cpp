// Port of engines/hgm_tools.py::acip_to_ewts — transcribed line for line.
// Protection markers ⟦⟧ from the Python become \x01/\x02 bytes here.
#include <cctype>
#include <string>

#include "allcore/engines.h"

namespace allcore {
namespace {

const char OPEN = '\x01', CLOSE = '\x02';
const char ACHEN_LONG = '\x03';  // stands in for a standalone long a-chen
const char VISARGA = '\x04';     // stands in for EWTS 'H'; see acipToEwts

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

    // VISARGA: ACIP ':' is EWTS 'H'. It follows a VOWEL or the anusvara 'm',
    // and that restriction is what separates it from punctuation — "THE FIRST
    // PATH:", "Includes:", a folio marker [107a]:, editorial ':-'. Editorial
    // {...} blocks are skipped: the strip above only removes {TIB}-shaped
    // ones, and {%Levi: GYIS} has a vowel before its colon too. Protected,
    // because EWTS visarga is an UPPERCASE H and the case-fold below would
    // otherwise make it the letter ha. Parity: engines/hgm_tools.py.
    {
        std::string o;
        bool inBrace = false;
        for (size_t i = 0; i < s.size(); ++i) {
            if (s[i] == '{') inBrace = true;
            else if (s[i] == '}') inBrace = false;
            if (s[i] == ':' && !inBrace && i > 0 &&
                std::string("AEIOUaeioum").find(s[i - 1]) != std::string::npos) {
                // A SENTINEL, not prot("H"). The long-vowel apostrophe rule
                // further down has the closing marker ⟧ in its own lookbehind
                // class, so a protected visarga reads to it as a consonant and
                // ka:'i came out kaHI — the genitive swallowed into a long
                // vowel — instead of kaH'i. Expanded after case folding.
                o += VISARGA;
            } else {
                o += s[i];
            }
        }
        s = o;
    }

    // A-CHEN'S OWN LONG VOWEL. ACIP marks a long vowel with an apostrophe
    // before it (K'A is kA), but the apostrophe rule further down needs a
    // CONSONANT before it, and for a-chen the letter IS the vowel — so A'A
    // fell through as a'a and rendered ཨའ, a-chen plus an a-chung, silently.
    // A'A is ཨཱ. Only where it OPENS a syllable: mid-word, vowel-apostrophe-
    // vowel is the achung particle (PA'AM, LA'ANG, and PA'I the genitive,
    // 53,651 occurrences). Must run AFTER the visarga rule above, so that the
    // colon in A'A: still sees a vowel before it.
    {
        std::string o;
        for (size_t i = 0; i < s.size();) {
            const bool atStart =
                (i == 0) || !std::isalpha((unsigned char)s[i - 1]);
            if (atStart && s.compare(i, 3, "A'A") == 0) {
                o += prot("A");
                i += 3;
            } else {
                o += s[i++];
            }
        }
        s = o;
    }

    // V is the ACIP code for WA-ZUR, and wa-zur is SUBJOINED: it cannot open
    // a syllable. A V with a letter before it is wa-zur and becomes EWTS w;
    // a V that opens a syllable is left alone, so stray Latin in the corpus
    // is not read as Tibetan. Runs BEFORE the protection markers below, so
    // the lookbehind still sees a real letter. Parity: engines/hgm_tools.py.
    {
        std::string o;
        for (size_t i = 0; i < s.size(); ++i) {
            // The preceding letter must be a CONSONANT: wa-zur subjoins to a
            // consonant and cannot attach to a vowel. "A letter" alone was a
            // Rule 3 regression — the acip column also holds ALL-CAPS English,
            // and EVERY became EWERY -> ཨེཝེརཡ where it had honestly flagged.
            if (s[i] == 'V' && i > 0 &&
                std::string("BCDGHJKLMNPRSTVWYZbcdghjklmnprstvwyz")
                        .find(s[i - 1]) != std::string::npos)
                o += 'W';
            else
                o += s[i];
        }
        s = o;
    }

    // Three ACIP forms this function never learned to read — ported with the
    // Python oracle, 2026-09-09. Adam saw all three sitting untranslated among
    // Tibetan drill options: G-YAS, G-YON and DVAGS. Not display bugs: the
    // engine was handing wylieToUnicode something it correctly refused.
    //   G-Y  CLOSED 5c3d77f8, at the other end: this still emits g-yon, and
    //        ewtsToUnicode now reads `-` as the prefix mark it always was.
    //   V    CLOSED 2026-09-16 — handled just above.
    //   :    OPEN. visarga; EWTS writes H. Next one to take.
    // docs/FINDING_ACIP_EWTS_GAPS.md carries the measurements.
    // Protected, so the lowercasing pass below leaves them alone.
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
        if (c == VISARGA) { out += 'H'; continue; }   // after folding, so it stays H
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
    // VISARGA: EWTS uppercase H is visarga and ACIP writes it ':'. It had no
    // inverse at all, so hoH went to ACIP HOH and came back hoh — the letter
    // ha. Lowercase h is the letter and is untouched.
    replaceAll(s, "H", prot(":"));
    // A-CHEN'S LONG VOWEL. The pass below turns EWTS A into ACIP 'A, which is
    // right after a consonant (kA -> K'A) and wrong on its own: a standalone
    // long a-chen is A'A in ACIP, not 'A. Handle that case first.
    {
        std::string o;
        for (size_t i = 0; i < s.size(); ++i) {
            // Look PAST protection markers for the real preceding letter.
            // The retroflex pass above has already wrapped N as \x01n\x02, so
            // the raw s[i-1] of the A in "NA" is a marker byte, not a letter,
            // and a naive test fired here: ba ra NA si went out as nA'A and
            // came back Na'a. Vararanasi is not a standalone a-chen.
            size_t j = i;
            while (j > 0 && (s[j - 1] == OPEN || s[j - 1] == CLOSE)) --j;
            const bool standalone =
                s[i] == 'A' &&
                (j == 0 || !std::isalpha((unsigned char)s[j - 1]));
            // A SENTINEL, not the literal A'A. prot() only marks a region;
            // replaceAll below is a plain string replace and walks straight
            // through markers, so a literal A'A here would have its own A's
            // rewritten by the long-vowel pass — which is the same ordering
            // hazard this file already records for TS/TZ. Expanded at the end.
            if (standalone) o += ACHEN_LONG;
            else o += s[i];
        }
        s = o;
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
        if (c == ACHEN_LONG) { out += "A'A"; continue; }
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
