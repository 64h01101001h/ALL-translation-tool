#include "allcore/tibexport.h"

#include <cctype>

#include "allcore/engines.h"

namespace allcore {

namespace {

bool isTokenChar(char c) {
    return std::isalnum((unsigned char)c) || c == '\'' || c == '+' ||
           c == '-' || c == '~' || c == '*' || c == '%' || c == '#' ||
           c == '.';
}

}  // namespace

TibetanExportResult exportTibetanUnicode(const std::string& acip_document,
                                         const TibetanExportOptions& opt) {
    TibetanExportResult res;
    std::string& out = res.unicode;
    const std::string TSHEG = "་", SHAD = "།", NYIS_SHAD = "༎";
    bool pendingTsheg = false;   // a tsheg owed after the last syllable
    std::string lastSyllable;

    auto emitShad = [&](bool dbl) {
        // print rule: no tsheg before shad, EXCEPT after final ང (nga)
        if (pendingTsheg) {
            if (!lastSyllable.empty() && lastSyllable.size() >= 2 &&
                lastSyllable.compare(lastSyllable.size() - 2, 2, "NG") == 0)
                out += TSHEG;
            pendingTsheg = false;
        }
        out += dbl ? NYIS_SHAD : SHAD;
        out += ' ';
    };
    auto emitSyllable = [&](const std::string& tok) {
        if (pendingTsheg) out += TSHEG;
        // curly apostrophes → ' before conversion
        std::string t;
        for (size_t i = 0; i < tok.size();) {
            if (i + 2 < tok.size() && (unsigned char)tok[i] == 0xE2 &&
                (unsigned char)tok[i + 1] == 0x80 &&
                ((unsigned char)tok[i + 2] == 0x98 ||
                 (unsigned char)tok[i + 2] == 0x99)) {
                t += '\'';
                i += 3;
            } else {
                t += tok[i++];
            }
        }
        auto [u, ok] = wylieToUnicode(tokenToEwts(t));
        ++res.syllables;
        if (ok && !u.empty()) {
            out += u;
        } else {
            ++res.failures;
            out += u.empty() ? ("\xE2\x9F\xA8" + t + "\xE2\x9F\xA9") : u;
            bool seen = false;
            for (const auto& f : res.failed) seen |= (f == t);
            if (!seen) res.failed.push_back(t);
        }
        lastSyllable = t;
        pendingTsheg = true;
    };

    const std::string& s = acip_document;
    for (size_t i = 0; i < s.size();) {
        const char c = s[i];
        if (c == '@') {
            // page/folio marker: @NNN, @NNNA, @012B …
            size_t j = i + 1;
            while (j < s.size() && (std::isalnum((unsigned char)s[j])))
                ++j;
            if (opt.keep_page_markers) {
                if (pendingTsheg) { out += TSHEG; pendingTsheg = false; }
                out += "\xE2\x9F\xAA" + s.substr(i + 1, j - i - 1) +
                       "\xE2\x9F\xAB";   // ⟪NNN⟫
            }
            i = j;
            continue;
        }
        if (c == '[') {
            size_t j = i + 1;
            while (j < s.size() && s[j] != ']') ++j;
            if (opt.keep_brackets) {
                if (pendingTsheg) { out += TSHEG; pendingTsheg = false; }
                out += "[" + s.substr(i + 1, j - i - 1) + "]";
            }
            i = (j < s.size()) ? j + 1 : j;
            continue;
        }
        if (c == ',' ) { emitShad(false); ++i; continue; }
        if (c == ';') { emitShad(true); ++i; continue; }
        if (c == '/') { emitShad(false); ++i; continue; }
        if (c == '\n' || c == '\r') {
            if (pendingTsheg) { out += TSHEG; pendingTsheg = false; }
            out += '\n';
            while (i < s.size() && (s[i] == '\n' || s[i] == '\r')) ++i;
            continue;
        }
        if (c == ' ' || c == '\t') { ++i; continue; }
        if (isTokenChar(c) ||
            (unsigned char)c >= 0x80) {   // token (incl. curly apostrophe)
            size_t j = i;
            while (j < s.size() &&
                   (isTokenChar(s[j]) || (unsigned char)s[j] >= 0x80))
                ++j;
            emitSyllable(s.substr(i, j - i));
            i = j;
            continue;
        }
        ++i;   // any other punctuation is dropped
    }
    if (pendingTsheg) out += TSHEG;
    return res;
}

std::string hgmTechnicalSpelling(const std::string& wylie) {
    // split into syllables, join in hyphenated pairs
    std::vector<std::string> syl;
    std::string cur;
    for (char c : wylie + " ") {
        if (c == ' ') {
            if (!cur.empty()) syl.push_back(cur);
            cur.clear();
        } else {
            cur += c;
        }
    }
    std::string out;
    for (size_t i = 0; i < syl.size(); ++i) {
        if (i == 0) {}
        else if (i % 2 == 1) out += '-';   // second of a pair
        else out += ' ';                    // new pair
        out += syl[i];
    }
    return out;
}

std::string bdrcScanUrl(const AcipFileInfo& info) {
    if (!info.recognized || info.number.empty()) return "";
    const char* base = nullptr;
    if (info.collection == "Kangyur (Derge edition)") base = "MW22084_";
    else if (info.collection == "Kangyur (Lhasa edition)") base = "MW26071_";
    else if (info.collection == "Tengyur (Derge edition)") base = "MW23703_";
    else return "";   // Sungbum/Reference/etc.: no Tohoku mapping — honest ""
    // BUDA uses exactly four digits: strip leading zeros first (catalog
    // refs can be five-digit, e.g. TD04158 = Tohoku 4158), then pad
    std::string n = info.number;
    while (n.size() > 1 && n[0] == '0') n.erase(0, 1);
    while (n.size() < 4) n = "0" + n;
    return "https://library.bdrc.io/show/bdr:" + std::string(base) + n;
}

// range punctuation per the DCC style guide (2023-08-25): number
// and folio ranges take the en dash, never a hyphen. Applied only
// between range endpoints — hyphens inside technical spellings
// (Lam-rim) are untouched because titles never pass through here.
static std::string endashRanges(const std::string& in) {
    std::string out = in;
    for (size_t i = 1; i + 1 < out.size(); ++i)
        if (out[i] == '-' &&
            std::isalnum((unsigned char)out[i - 1]) &&
            std::isalnum((unsigned char)out[i + 1]))
            out.replace(i, 1, "\u2013");
    return out;
}

std::string composeBibliographyEntry(const BibliographyFields& f) {
    // DCC style guide (2023-08-25) Tibetan-entry template:
    //   author, date(s).  English title (Tibetan title, ACIP
    //   number), folio range.
    // — two spaces after the dates period (house sentence
    // spacing), en dashes in ranges.
    std::string out;
    if (!f.epithets.empty()) out += "(" + f.epithets + ") ";
    out += f.author;
    if (!f.dates.empty()) out += ", " + endashRanges(f.dates);
    out += ".  " + f.english_title;
    if (!f.tibetan_title.empty() || !f.acip_number.empty()) {
        out += " (";
        out += f.tibetan_title;
        if (!f.acip_number.empty())
            out += (f.tibetan_title.empty() ? "ACIP " : ", ACIP ") +
                   f.acip_number;
        out += ")";
    }
    if (!f.folios.empty()) out += ", ff. " + endashRanges(f.folios);
    out += ".";
    return out;
}

std::string composeSanskritBibEntry(const SanskritBibFields& f) {
    // DCC style guide Sanskrit-entry template, reproduced from its
    // own S1 (Dharmakīrti) example: author (Tib: name), dates.
    // English Title (Sanskrit) (Tib: Tibetan title, Tibetan
    // translation at ACIP number, folio range of Vol. N [letter]
    // in the … Section [Sanskrit, Tibetan] of the collection
    // [edition edition]).
    std::string out = f.author_skt;
    if (!f.author_tib.empty())
        out += " (Tib: " + f.author_tib + ")";
    if (!f.dates.empty()) out += ", " + endashRanges(f.dates);
    out += ".  " + f.english_title;
    if (!f.sanskrit_title.empty())
        out += " (" + f.sanskrit_title + ")";
    const bool tibBlock = !f.tibetan_title.empty() ||
                          !f.acip_number.empty() ||
                          !f.folios.empty();
    if (tibBlock) {
        out += " (Tib: " + f.tibetan_title;
        if (!f.acip_number.empty())
            out += ", Tibetan translation at ACIP " + f.acip_number;
        if (!f.folios.empty())
            out += ", ff. " + endashRanges(f.folios);
        if (!f.vol_num.empty()) {
            out += " of Vol. " + f.vol_num;
            if (!f.vol_letter.empty())
                out += " [" + f.vol_letter + "]";
        }
        if (!f.section_en.empty()) {
            out += " in the " + f.section_en + " Section";
            std::string inner;
            if (!f.section_skt.empty()) inner = f.section_skt;
            if (!f.section_tib.empty())
                inner += (inner.empty() ? "" : ", ") +
                         f.section_tib;
            if (!inner.empty()) out += " [" + inner + "]";
        }
        if (!f.collection.empty())
            out += " of the " + f.collection;
        if (!f.edition.empty())
            out += " [" + f.edition + " edition]";
        out += ")";
    }
    out += ".";
    return out;
}

TranslationPrep formatForTranslation(const std::string& acip_document) {
    TranslationPrep prep;
    std::string& out = prep.text;
    std::string folio;   // current @page marker, for note provenance
    const std::string& s = acip_document;
    auto trimEndSpace = [&] {
        while (!out.empty() && (out.back() == ' ')) out.pop_back();
    };
    for (size_t i = 0; i < s.size();) {
        const char c = s[i];
        if (c == '@') {
            size_t j = i + 1;
            while (j < s.size() && std::isalnum((unsigned char)s[j])) ++j;
            folio = s.substr(i + 1, j - i - 1);
            // GMR's stipulated form is [f. 1a], not [f. 001A]: he
            // replaces "[f. 00" with "[f. " to strip the input centre's
            // zero padding, and "A]" with "a]" (ignore-case OFF) to
            // lowercase the side letter. He leaves the B side to be done
            // by hand only because Word made it awkward - "in this
            // simplified system I've done" - so we normalise both sides.
            std::string ref = folio;
            size_t z = 0;
            while (z + 1 < ref.size() && ref[z] == '0') ++z;
            ref = ref.substr(z);
            if (!ref.empty()) {
                char& side = ref.back();
                if (side >= 'A' && side <= 'Z')
                    side = (char)(side - 'A' + 'a');
            }
            folio = ref;   // notes tag with the same normalised form
            trimEndSpace();
            if (!out.empty() && out.back() != '\n') out += ' ';
            out += "[f. " + ref + "] ";
            i = j;
            continue;
        }
        if (c == '[') {
            size_t j = i + 1;
            while (j < s.size() && s[j] != ']') ++j;
            const std::string content = s.substr(i + 1, j - i - 1);
            prep.notes.push_back(
                (folio.empty() ? "" : "folio " + folio + ": ") + content);
            trimEndSpace();
            out += "[" + std::to_string(prep.notes.size()) + "]";
            i = (j < s.size()) ? j + 1 : j;
            continue;
        }
        if (c == ',') {
            // double shad = paragraph break; single shad flows on.
            // The two halves of a nyis shad are often SPLIT by the input
            // file's hard wrap - ACIP is typed to a fixed width, so a
            // shad pair straddling the line end is ordinary, not exotic.
            // Skip any run of whitespace (newlines included) before
            // deciding, or every such pair silently loses its paragraph
            // break while still emitting ",," into the text.
            trimEndSpace();
            size_t j = i + 1;
            while (j < s.size() && (s[j] == ' ' || s[j] == '\t' ||
                                    s[j] == '\n' || s[j] == '\r'))
                ++j;
            if (j < s.size() && s[j] == ',') {
                // A shad PAIR - but a pair is not automatically a
                // paragraph. GMR's rule, read off the recording: a
                // paragraph ends at "O, ," - a syllable closing in a
                // CAPITAL O (SO, LO, TO), 1,737 of them on the
                // demonstrated text. Every OTHER pair (679) is a line of
                // quoted verse and takes a line break instead, keeping
                // its shad at both ends. Treating all pairs as
                // paragraphs turns the opening homage verses into prose.
                // Capital only: he searches with ignore-case OFF and
                // warns against confusing the letter with a zero.
                if (!out.empty() && out.back() == 'O') {
                    out += ",,\n\n";
                    ++prep.paragraphs;
                } else {
                    out += ",\n,";
                }
                i = j + 1;
            } else {
                out += ", ";
                ++i;
            }
            continue;
        }
        if (c == ';') {
            // bum shad - the final shad of a woodblock page. Verified on
            // screen at 13:18: find ";", replace "^p," - a line break
            // and a leading shad, 429 matches. The narration says "a
            // comma"; the pane says otherwise and the pane wins.
            trimEndSpace();
            out += "\n,";
            ++i;
            continue;
        }
        if (c == '\n' || c == '\r') {
            // input line breaks are flowed away (GMR's ^p^p removal pass)
            trimEndSpace();
            if (!out.empty() && out.back() != '\n') out += ' ';
            while (i < s.size() && (s[i] == '\n' || s[i] == '\r')) ++i;
            continue;
        }
        if (c == ' ' || c == '\t') {
            if (!out.empty() && out.back() != ' ' && out.back() != '\n')
                out += ' ';
            ++i;
            continue;
        }
        out += c;
        ++i;
    }
    trimEndSpace();
    if (!out.empty() && out.find_first_not_of(" \n") != std::string::npos &&
        out.rfind(",,\n\n") != out.size() - 4)
        ++prep.paragraphs;
    return prep;
}

AcipFileInfo decodeAcipFilename(const std::string& filename) {
    AcipFileInfo info;
    // strip any directory part, split name/extension
    std::string name = filename;
    auto slash = name.find_last_of("/\\");
    if (slash != std::string::npos) name = name.substr(slash + 1);
    std::string ext;
    auto dot = name.find_last_of('.');
    if (dot != std::string::npos) {
        ext = name.substr(dot + 1);
        name = name.substr(0, dot);
    }
    for (auto& ch : name)
        if (ch >= 'a' && ch <= 'z') ch = (char)(ch - 'a' + 'A');
    for (auto& ch : ext)
        if (ch >= 'a' && ch <= 'z') ch = (char)(ch - 'a' + 'A');

    // collection prefix (longest first)
    struct Pfx { const char* p; const char* label; };
    static const Pfx kPfx[] = {
        {"KD", "Kangyur (Derge edition)"},
        {"KL", "Kangyur (Lhasa edition)"},
        {"KX", "Kangyur (separate publication)"},
        {"K", "Kangyur"},
        {"TD", "Tengyur (Derge edition)"},
        {"TS", "Tengyur (Serdri edition)"},
        {"T", "Tengyur"},
        {"R", "Reference Materials"},
        {"GS", "Graphics (St. Petersburg seals)"},
        {"G", "Graphics Collection"},
        {"S", "Sungbum Collection"},
    };
    size_t pos = 0;
    for (const auto& p : kPfx) {
        const size_t n = std::string(p.p).size();
        if (name.rfind(p.p, 0) == 0 && name.size() > n &&
            std::isdigit((unsigned char)name[n])) {
            info.collection = p.label;
            pos = n;
            break;
        }
    }
    // Sungbum "S + any other letter" = specific edition/typing
    if (info.collection.empty() && name.size() > 2 && name[0] == 'S' &&
        std::isalpha((unsigned char)name[1]) &&
        std::isdigit((unsigned char)name[2])) {
        info.collection = std::string("Sungbum (edition ") + name[1] + ")";
        pos = 2;
    }
    if (info.collection.empty()) return info;

    // 4-digit core number
    size_t d = pos;
    while (d < name.size() && std::isdigit((unsigned char)name[d])) ++d;
    if (d == pos) return info;
    info.number = name.substr(pos, d - pos);

    // status letter (verification level)
    static const struct { char c; const char* label; } kStatus[] = {
        {'A', "first typing"},
        {'B', "second typing"},
        {'C', "first automated comparison"},
        {'D', "second automated comparison"},
        {'E', "third automated comparison"},
        {'F', "expert manual proofreading"},
        {'G', "converted format (legacy)"},
        {'H', "converted format (legacy)"},
        {'I', "incomplete"},
        {'L', "E-level + page/line statistical checks"},
        {'M', "L-level + automated typo checks"},
        {'N', "F-level + automated typo checks"},
    };
    if (d < name.size()) {
        for (const auto& st : kStatus)
            if (name[d] == st.c) {
                info.status = st.label;
                if (st.c == 'I') info.incomplete = true;
                ++d;
                break;
            }
    }
    // remaining = part/volume designation; trailing P = purposely partial
    if (d < name.size()) {
        info.part = name.substr(d);
        if (!info.part.empty() && info.part.back() == 'P') {
            info.partial = true;
            info.part.pop_back();
        }
    }
    // R0002 + letters = THE GREAT DICTIONARY (CatalogNumber.m special
    // case): the letters are the sub-number, "mostly the letters of the
    // Tibetan alphabet"
    if (info.collection == "Reference Materials" && info.number == "0002" &&
        !info.part.empty()) {
        info.collection = "Reference — The Great Dictionary";
        info.subNumber = info.part + " (alphabet volume)";
    } else if (!info.part.empty()) {
        // decode the sub-number per the toolchain grammar: 1-2 plain
        // digits = 1-99; letter A-T + digit = letter-hundreds
        // (A=100 … T=290 step 10, plus the ones digit)
        const std::string& p = info.part;
        bool allDigits = true;
        for (char c : p) allDigits &= std::isdigit((unsigned char)c) != 0;
        if (allDigits && p.size() <= 2) {
            info.subNumber = std::to_string(std::stoi(p));
        } else if (p.size() >= 1 && p[0] >= 'A' && p[0] <= 'T' &&
                   (p.size() == 1 ||
                    (p.size() == 2 && std::isdigit((unsigned char)p[1])))) {
            int v = 100 + (p[0] - 'A') * 10;
            if (p.size() == 2) v += p[1] - '0';
            info.subNumber = std::to_string(v);
        }
    }
    // extension → language / state (per the toolchain's own table:
    // CatalogNumber.m — TB also covers TXT/ALT/AAT/AT1/AT2/APT/BK!/RAW)
    if (ext == "ACT") info.language = "Tibetan (approved for release)";
    else if (ext == "ACE") info.language = "English (approved for release)";
    else if (ext == "ACS") info.language = "Sanskrit (approved for release)";
    else if (ext == "ACM") info.language = "mixed languages (approved)";
    else if (ext == "INC") { info.language = "Tibetan (text lacking 4+ lines)"; info.incomplete = true; }
    else if (ext == "INE") { info.language = "English (incomplete)"; info.incomplete = true; }
    else if (ext == "RAW") info.language = "Tibetan (unproofread RAW)";
    else if (ext == "TXT" || ext == "ALT" || ext == "AAT" ||
             ext == "AT1" || ext == "AT2" || ext == "APT" || ext == "BK!")
        info.language = "Tibetan (" + ext + " working file)";
    else if (!ext.empty()) info.language = ext;
    info.recognized = true;
    return info;
}

}  // namespace allcore
