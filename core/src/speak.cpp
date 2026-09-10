// allcore/speak.cpp — see speak.h.
//
// Recordings are keyed on the SPELLING, never on the engine's reading. The
// reading is the printed-phonetics convention, which flattens aspiration and
// voicing on purpose - ka and kha both read "ka", sa and za both read "sa".
// The recordings do not: those are different sounds, recorded separately.
// Keying audio on the reading made 71.4% of spellings play some other
// syllable's recording and left 1,816 of 2,860 recordings unreachable.
#include "allcore/speak.h"

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <fstream>
#include <sstream>

#include "allcore/engines.h"

namespace allcore {
namespace {

bool isVowel(char c) {
    return c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u';
}

std::vector<std::string> splitCsvLine(const std::string& line) {
    std::vector<std::string> out;
    std::string cur;
    bool quoted = false;
    for (char c : line) {
        if (c == '"') { quoted = !quoted; continue; }
        if (c == ',' && !quoted) { out.push_back(cur); cur.clear(); continue; }
        cur += c;
    }
    out.push_back(cur);
    return out;
}

// A post-suffix (the final d or s of e.g. thabs, dbyibs) is written and not
// said. Strip it so the syllable can find its recording.
std::string dropPostSuffix(const std::string& syl) {
    if (syl.size() < 3) return syl;
    const char last = syl[syl.size() - 1];
    if (last != 'd' && last != 's') return syl;
    const std::string head = syl.substr(0, syl.size() - 1);
    // only when what remains still ends in a real suffix consonant
    static const std::string kSuffix = "gnbmrl";
    if (kSuffix.find(head[head.size() - 1]) == std::string::npos) return syl;
    bool hasVowel = false;
    for (char c : head) if (isVowel(c)) hasVowel = true;
    return hasVowel ? head : syl;
}

// wa-zur is written under a letter and never pronounced.
std::string dropWazur(const std::string& syl) {
    const size_t v = syl.find_first_of("aeiou");
    if (v == std::string::npos || v < 2) return syl;
    std::string onset = syl.substr(0, v);
    const size_t w = onset.find_first_of("vw", 1);
    if (w == std::string::npos) return syl;
    onset.erase(w, 1);
    return onset + syl.substr(v);
}

}  // namespace

const char* tierLabel(SpeakTier t) {
    switch (t) {
        case SpeakTier::Exact:         return "exact";
        case SpeakTier::SilentDropped: return "exact (silent letter dropped)";
        case SpeakTier::BaseEnding:    return "two recordings";
        case SpeakTier::Reduced:       return "approximate (reduced stack)";
        default:                       return "no recording";
    }
}

SpeakBank::SpeakBank(const std::string& root) {
    root_ = root.empty() ? std::string("data/audio/kawachen") : root;
#if !defined(ALL_HAS_AUDIO)
    // PUBLIC build. The bank is not merely empty here — the code that would
    // open a recording is not compiled, so a public binary cannot reach one
    // even if the files were placed beside it. That is the licensing
    // guarantee Kawachen's in-house permission asks of us.
    return;
#else
    std::ifstream in(root_ + "/manifest.csv");
    if (!in) return;                       // absent bank: a private build
                                           // whose assets are not harvested yet

    std::string line;
    if (!std::getline(in, line)) return;
    const std::vector<std::string> head = splitCsvLine(line);
    auto col = [&](const char* name) {
        for (size_t i = 0; i < head.size(); ++i)
            if (head[i] == name) return (int)i;
        return -1;
    };
    const int cFile = col("file"), cDir = col("dir"), cKind = col("kind");
    const int cWylie = col("wylie"), cId = col("identified");
    if (cFile < 0 || cDir < 0 || cWylie < 0 || cId < 0) return;

    while (std::getline(in, line)) {
        if (line.empty()) continue;
        const std::vector<std::string> f = splitCsvLine(line);
        if ((int)f.size() <= cId) continue;
        if (f[cId] != "True") continue;
        ++recordings_;
        const std::string path = root_ + "/" + f[cDir] + "/" + f[cFile];
        if (firstPath_.empty()) firstPath_ = path;
        if (f[cDir] == "syllable_mp3") {
            // every spelling the recording serves, not just the first: a
            // merged class is several spellings of ONE sound, and each must
            // find it
            size_t b = 0;
            while (b <= f[cWylie].size()) {
                const size_t e = f[cWylie].find('/', b);
                const std::string sp = f[cWylie].substr(
                    b, e == std::string::npos ? std::string::npos : e - b);
                if (!sp.empty()) byReading_.emplace(sp, path);
                if (e == std::string::npos) break;
                b = e + 1;
            }
        } else if (cKind >= 0 && f[cKind] == "ending name") {
            endings_.emplace(f[cWylie], path);
        } else {
            terms_.emplace(f[cWylie], path);
        }
    }
    // Counting manifest rows is not evidence that a single recording is on
    // disk. The manifest is TRACKED and the audio is NOT, so a fresh clone has
    // every row and no sound — and would have reported the bank available and
    // then handed out paths to nothing. Availability means a file opens.
    if (recordings_ > 0 && !firstPath_.empty()) {
        std::ifstream probe(firstPath_, std::ios::binary);
        available_ = probe.good();
    }
#endif
}

std::string SpeakBank::lookup(const std::string& syl) const {
    if (syl.empty()) return std::string();
    const auto it = byReading_.find(syl);
    return it == byReading_.end() ? std::string() : it->second;
}

SpokenSyllable SpeakBank::resolve(const std::string& wylie) const {
    SpokenSyllable out;
    out.wylie = wylie;
    if (!available_ || wylie.empty()) return out;

    if (const std::string f = lookup(wylie); !f.empty()) {
        out.files.push_back(f);
        out.tier = SpeakTier::Exact;
        return out;
    }

    // Silent letters first — dropping them is not an approximation. A
    // prefix is never pronounced; a superscript over an UNVOICED root is not
    // either, and Kawachen's own reader serves rka from the ka recording.
    // Over a voiced root a superscript DOES change the sound, but those
    // stacks (rga, rnga, rda ...) carry their own spellings in the bank, so
    // they matched exactly above and never reach here. Each candidate is
    // accepted only if what remains is itself a recorded syllable, so a root
    // consonant is never mistaken for a silent one.
    std::string probe = dropWazur(dropPostSuffix(wylie));
    if (probe != wylie) {
        if (const std::string f = lookup(probe); !f.empty()) {
            out.files.push_back(f);
            out.tier = SpeakTier::SilentDropped;
            return out;
        }
    }
    {
        static const std::string kPrefix = "gdbm'";
        static const std::string kSuper = "rls";
        std::vector<std::string> cand;
        if (probe.size() > 2 && kPrefix.find(probe[0]) != std::string::npos)
            cand.push_back(probe.substr(1));
        if (probe.size() > 2 && kSuper.find(probe[0]) != std::string::npos)
            cand.push_back(probe.substr(1));
        if (probe.size() > 3 && kPrefix.find(probe[0]) != std::string::npos &&
            kSuper.find(probe[1]) != std::string::npos)
            cand.push_back(probe.substr(2));       // bstan -> stan -> tan
        for (const std::string& c : cand) {
            if (const std::string f = lookup(c); !f.empty()) {
                out.files.push_back(f);
                out.tier = SpeakTier::SilentDropped;
                return out;
            }
        }
    }

    // a genitive contraction is the base syllable plus its ending
    if (wylie.size() > 2) {
        const std::string tail = wylie.substr(wylie.size() - 2);
        if (tail == "'i" || tail == "'u" || tail == "'o") {
            const auto e = endings_.find(tail);
            if (e != endings_.end()) {
                const std::string base = wylie.substr(0, wylie.size() - 2);
                if (const std::string f = lookup(base); !f.empty()) {
                    out.files.push_back(f);
                    out.files.push_back(e->second);
                    out.tier = SpeakTier::BaseEnding;
                    return out;
                }
            }
        }
    }

    // last resort: drop a subscript. This CHANGES the sound, so the caller is
    // told, and the label travels with it to the interface.
    const size_t v = probe.find_first_of("aeiou");
    if (v != std::string::npos && v >= 2) {
        const char sub = probe[v - 1];
        if (sub == 'y' || sub == 'r' || sub == 'l') {
            const std::string reduced = probe.substr(0, v - 1) + probe.substr(v);
            if (const std::string f = lookup(reduced); !f.empty()) {
                out.files.push_back(f);
                out.tier = SpeakTier::Reduced;
                return out;
            }
        }
    }
    return out;
}

std::vector<SpokenSyllable> SpeakBank::readAloud(const std::string& wylie) const {
    std::vector<SpokenSyllable> out;
    std::istringstream in(wylie);
    std::string tok;
    while (in >> tok) {
        // split on the wylie syllable separators the corpus actually uses
        std::string cur;
        for (size_t i = 0; i <= tok.size(); ++i) {
            const char c = i < tok.size() ? tok[i] : '/';
            if (c == '/' || c == '|') {
                if (!cur.empty()) { out.push_back(resolve(cur)); cur.clear(); }
            } else {
                cur += c;
            }
        }
    }
    return out;
}

std::string SpeakBank::termFile(const std::string& termWylie) const {
    const auto it = terms_.find(termWylie);
    if (it != terms_.end()) return it->second;
    const auto e = endings_.find(termWylie);
    return e == endings_.end() ? std::string() : e->second;
}

}  // namespace allcore
