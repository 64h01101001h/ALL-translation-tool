// textdna.cpp — see allcore/textdna.h.
#include "allcore/textdna.h"

#include <cctype>
#include <sstream>

#include "allcore/verse.h"
#include "allcore/lattice.h"

namespace allcore {
namespace {

// the citations lane's announcement shape, reduced to one line:
// ZHES/CES/SHES (quote end) with a speech verb within four tokens
bool lineQuoteMark(const std::string& line) {
    std::istringstream is(line);
    std::vector<std::string> toks;
    std::string t;
    while (is >> t) {
        std::string up;
        for (char c : t)
            up += static_cast<char>(
                std::toupper(static_cast<unsigned char>(c)));
        // strip trailing punctuation
        while (!up.empty() &&
               !(std::isalpha(static_cast<unsigned char>(up.back())) ||
                 up.back() == '\''))
            up.pop_back();
        toks.push_back(up);
    }
    for (size_t i = 0; i < toks.size(); ++i) {
        if (toks[i] != "ZHES" && toks[i] != "CES" && toks[i] != "SHES")
            continue;
        for (size_t j = i + 1; j < toks.size() && j <= i + 4; ++j)
            if (toks[j] == "GSUNGS" || toks[j] == "BSHAD" ||
                toks[j] == "SMRAS" || toks[j] == "GSUNG" ||
                toks[j] == "BYA" || toks[j] == "GRAGS")
                return true;
    }
    return false;
}

}  // namespace

DnaReport textDna(const std::string& acip) {
    DnaReport out;
    // the analyzer supplies the document's dominant meter; the strip
    // itself classifies SOURCE lines (analyzeVerse renumbers by
    // non-empty shad segments, which is the wrong axis for a strip
    // the reader clicks to jump by line)
    const VerseReport vr = analyzeVerse(acip);
    out.is_verse = vr.is_verse;
    out.meter = vr.meter;
    std::vector<std::string> raw;
    {
        std::istringstream is(acip);
        std::string l;
        while (std::getline(is, l)) raw.push_back(l);
    }
    int number = 0;
    for (const auto& line : raw) {
        ++number;
        DnaLine d;
        d.number = number;
        // segment the line the same way the analyzer does (shad and
        // comma delimit verse feet inside one source line)
        std::vector<std::string> segs;
        std::string cur;
        for (char c : line + ",") {
            if (c == ',' || c == ';' || c == '/') {
                segs.push_back(cur);
                cur.clear();
            } else {
                cur += c;
            }
        }
        bool anySyl = false, allMeter = true;
        for (const auto& sg : segs) {
            std::vector<std::string> toks;
            std::vector<bool> bars;
            tokenizeDocument(sg, toks, bars);
            if (toks.empty()) continue;
            anySyl = true;
            d.syllables += (int)toks.size();
            if ((int)toks.size() != vr.meter) allMeter = false;
        }
        if (!anySyl)
            d.cls = DnaClass::Blank;
        else if (!vr.is_verse)
            d.cls = DnaClass::Prose;
        else if (allMeter)
            d.cls = DnaClass::Verse;
        else
            d.cls = DnaClass::VerseIrregular;
        d.quote_mark = anySyl && lineQuoteMark(line);
        out.lines.push_back(d);
    }
    return out;
}

}  // namespace allcore
