// catalog_name.cpp — see allcore/catalog_name.h. The +/META machinery
// engages ONLY when a name exceeds the filesystem limit; a name that fits
// is just a name.
#include "allcore/catalog_name.h"

#include <algorithm>
#include <cctype>

namespace allcore {
namespace {

// common IAST diacritics -> ASCII (filenames carry no diacritics; the
// Mixed Nuts rule writes Guhyasamaja, not Guhyasamāja)
struct Fold { const char* from; const char* to; };
const Fold kFolds[] = {
    {"\xC4\x81", "a"}, {"\xC4\xAB", "i"}, {"\xC5\xAB", "u"},   // ā ī ū
    {"\xE1\xB9\x87", "n"},  // ṇ
    {"\xE1\xB9\xAD", "t"},  // ṭ
    {"\xE1\xB8\x8D", "d"},  // ḍ
    {"\xC5\x9B", "sh"},     // ś
    {"\xE1\xB9\xA3", "sh"}, // ṣ
    {"\xC3\xB1", "ny"},     // ñ
    {"\xE1\xB9\x83", "m"},  // ṃ
    {"\xE1\xB9\x85", "n"},  // ṅ
    {"\xE1\xB8\xA5", "h"},  // ḥ
    {"\xE1\xB9\x9B", "ri"}, // ṛ
    {"\xC4\x80", "A"}, {"\xC4\xAA", "I"}, {"\xC5\xAA", "U"},   // Ā Ī Ū
    {"\xC5\x9A", "Sh"},     // Ś
};

}  // namespace

std::string sanitizeNameField(const std::string& field) {
    std::string s = field;
    for (const auto& f : kFolds) {
        size_t p = 0;
        while ((p = s.find(f.from, p)) != std::string::npos) {
            s.replace(p, std::char_traits<char>::length(f.from), f.to);
            p += std::char_traits<char>::length(f.to);
        }
    }
    std::string out;
    out.reserve(s.size());
    for (size_t i = 0; i < s.size(); ++i) {
        const unsigned char c = static_cast<unsigned char>(s[i]);
        if (c == ':') {
            // "no colon — a single hyphen, no spaces" (session 5)
            while (!out.empty() && out.back() == ' ') out.pop_back();
            out.push_back('-');
            while (i + 1 < s.size() && s[i + 1] == ' ') ++i;
        } else if (c == '_') {
            out.push_back(' ');   // underscore is the field separator only
        } else if (c == '/' || c == '\\' || c < 0x20) {
            // path separators and control characters have no place
        } else if (c >= 0x80) {
            // any remaining non-ASCII is dropped, never guessed at
        } else {
            out.push_back(static_cast<char>(c));
        }
    }
    // collapse whitespace
    std::string collapsed;
    bool sp = true;
    for (char c : out) {
        if (c == ' ') {
            if (!sp) { collapsed.push_back(' '); sp = true; }
        } else {
            collapsed.push_back(c);
            sp = false;
        }
    }
    while (!collapsed.empty() && collapsed.back() == ' ')
        collapsed.pop_back();
    return collapsed;
}

std::string stripBzhugsSo(const std::string& tibetan_title) {
    std::string t = tibetan_title;
    auto upper = [](std::string s) {
        for (char& c : s)
            c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
        return s;
    };
    while (true) {
        std::string u = upper(t);
        size_t e = u.find_last_not_of(" ,.\n\r");
        if (e == std::string::npos) return "";
        u.erase(e + 1);
        t.erase(e + 1);
        if (u.size() >= 9 && u.substr(u.size() - 9) == "BZHUGS SO") {
            t.erase(t.size() - 9);
            continue;
        }
        // "… BZHUGS" alone at the end (clipped forms)
        if (u.size() >= 7 && u.substr(u.size() - 7) == " BZHUGS") {
            t.erase(t.size() - 7);
            continue;
        }
        break;
    }
    return t;
}

ComposedName composeCatalogFilename(const std::string& number,
                                    const std::string& tibetan_title,
                                    const std::string& english_title,
                                    const std::string& author,
                                    const std::string& ext,
                                    size_t max_name) {
    ComposedName r;
    const std::string num = sanitizeNameField(number);
    const std::string tib =
        sanitizeNameField(stripBzhugsSo(tibetan_title));
    const std::string eng = sanitizeNameField(english_title);
    const std::string aut = sanitizeNameField(author);

    std::string stem = num;
    for (const std::string* f : {&tib, &eng, &aut})
        if (!f->empty()) stem += "_" + *f;
    r.full_stem = stem;

    if (stem.size() + ext.size() <= max_name) {
        r.filename = stem + ext;
        return r;   // fits: no '+', no META — the normal case
    }

    // fill to the max and stop EVEN MID-WORD (the May 5 ruling: no
    // padding, so the future rejoin is pure concatenation), '+' marks
    // the continuation
    const size_t keep = max_name - ext.size() - 1;   // room for '+'
    r.truncated = true;
    r.filename = stem.substr(0, keep) + "+" + ext;
    r.meta_filename = num + " META" + ext;
    r.meta_content = stem.substr(keep);
    return r;
}

std::string rejoinMetaName(const std::string& truncated_stem,
                           const std::string& meta_continuation) {
    std::string s = truncated_stem;
    if (!s.empty() && s.back() == '+') s.pop_back();
    return s + meta_continuation;
}

std::string metaContinuationText(const std::string& file_content) {
    std::string body = file_content;
    if (body.rfind("{\\rtf", 0) == 0) {
        // Word-era companion: the payload sits after the last "\cf0 "
        // control run, up to the closing brace.
        const size_t p = body.rfind("\\cf0 ");
        if (p != std::string::npos) {
            body = body.substr(p + 5);
            const size_t e = body.rfind('}');
            if (e != std::string::npos) body.erase(e);
            // RTF escapes: \'xx hex bytes, \\ and \{ \}
            std::string out;
            for (size_t i = 0; i < body.size(); ++i) {
                if (body[i] == '\\' && i + 3 < body.size() &&
                    body[i + 1] == '\'') {
                    const std::string hex = body.substr(i + 2, 2);
                    out.push_back(static_cast<char>(
                        std::stoi(hex, nullptr, 16)));
                    i += 3;
                } else if (body[i] == '\\' && i + 1 < body.size()) {
                    out.push_back(body[i + 1]);
                    ++i;
                } else {
                    out.push_back(body[i]);
                }
            }
            body = out;
        }
    }
    // trim
    const size_t a = body.find_first_not_of(" \t\r\n");
    if (a == std::string::npos) return "";
    const size_t b = body.find_last_not_of(" \t\r\n");
    return body.substr(a, b - a + 1);
}

}  // namespace allcore
