#include "allcore/lexicon.h"

#include <fstream>
#include <sstream>

namespace allcore {

namespace {

void appendUtf8(std::string& out, char32_t cp) {
    if (cp < 0x80) out.push_back(static_cast<char>(cp));
    else if (cp < 0x800) {
        out.push_back(static_cast<char>(0xC0 | (cp >> 6)));
        out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
    } else if (cp < 0x10000) {
        out.push_back(static_cast<char>(0xE0 | (cp >> 12)));
        out.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
        out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
    } else {
        out.push_back(static_cast<char>(0xF0 | (cp >> 18)));
        out.push_back(static_cast<char>(0x80 | ((cp >> 12) & 0x3F)));
        out.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
        out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
    }
}

// upstream ships one list as UTF-16LE (with BOM, sometimes doubled) and the
// other as UTF-8 — normalize both to UTF-8 text before line-splitting.
// (Bug fixed 2026-08-07: the previous byte-wise reader stored the UTF-16
// list as garbage, so its 107k forms could never attest anything.)
std::string normalizedText(const std::string& path) {
    std::ifstream f(path, std::ios::binary);
    if (!f) return {};
    std::stringstream buf;
    buf << f.rdbuf();
    std::string raw = buf.str();
    if (raw.size() >= 2 && static_cast<unsigned char>(raw[0]) == 0xFF &&
        static_cast<unsigned char>(raw[1]) == 0xFE) {
        // UTF-16LE
        std::string out;
        out.reserve(raw.size());
        for (size_t i = 0; i + 1 < raw.size(); i += 2) {
            char32_t u = static_cast<unsigned char>(raw[i]) |
                         (static_cast<unsigned char>(raw[i + 1]) << 8);
            if (u == 0xFEFF) continue;  // BOM(s)
            if (u >= 0xD800 && u <= 0xDBFF && i + 3 < raw.size()) {
                char32_t lo = static_cast<unsigned char>(raw[i + 2]) |
                              (static_cast<unsigned char>(raw[i + 3]) << 8);
                if (lo >= 0xDC00 && lo <= 0xDFFF) {
                    u = 0x10000 + ((u - 0xD800) << 10) + (lo - 0xDC00);
                    i += 2;
                }
            }
            appendUtf8(out, u);
        }
        return out;
    }
    // UTF-8: strip a BOM if present
    if (raw.size() >= 3 && static_cast<unsigned char>(raw[0]) == 0xEF &&
        static_cast<unsigned char>(raw[1]) == 0xBB &&
        static_cast<unsigned char>(raw[2]) == 0xBF)
        raw.erase(0, 3);
    return raw;
}

bool hasAsciiLetter(const std::string& s) {
    for (unsigned char c : s)
        if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) return true;
    return false;
}

}  // namespace

bool RefLexicon::load(const std::string& path, const std::string& label) {
    std::string text = normalizedText(path);
    if (text.empty()) return false;
    auto& set = labelA_.empty() || labelA_ == label ? a_ : b_;
    (labelA_.empty() || labelA_ == label ? labelA_ : labelB_) = label;
    std::stringstream ss(text);
    std::string line;
    while (std::getline(ss, line)) {
        while (!line.empty() &&
               (line.back() == '\r' || line.back() == ' '))
            line.pop_back();
        // ASCII-lettered lines are headers/comments ("word"), not Tibetan
        if (!line.empty() && !hasAsciiLetter(line)) set.insert(line);
    }
    return !set.empty();
}

std::string RefLexicon::attested(const std::string& w) const {
    std::string out;
    if (a_.count(w)) out = labelA_;
    if (b_.count(w)) out += (out.empty() ? "" : " + ") + labelB_;
    return out;
}

void RefLexicon::eachWord(
    const std::function<void(const std::string&)>& fn) const {
    for (const auto& w : a_) fn(w);
    for (const auto& w : b_) fn(w);
}

}  // namespace allcore
