// text_util.h — the small UTF-8 and whitespace helpers the text tools share.
// INTERNAL to core/src (not installed, not part of the public allcore API):
// these were file-static in textdiff.cpp until the analysis-suite F3 batch
// needed the same codepoint walker for textnorm. Promoted, not copied — the
// plan's rule — so there is one definition of "what a codepoint is" and one
// definition of "collapse the whitespace", and a fix lands in both callers.
//
// Bodies are the textdiff ones verbatim; textdiff_smoke pins that nothing
// changed. Deliberately tiny and header-only: no new translation unit, no
// link order to think about.
#pragma once

#include <cctype>
#include <string>
#include <utility>
#include <vector>

namespace allcore {
namespace textutil {

// (byte offset, code point) per character; malformed bytes pass as-is —
// this walker never rejects input, because the input centres' files are
// exactly where malformed bytes turn up and dropping them silently would
// be a guess.
inline std::vector<std::pair<int, unsigned>> codepoints(const std::string& s) {
    std::vector<std::pair<int, unsigned>> v;
    for (size_t i = 0; i < s.size();) {
        const unsigned char c = (unsigned char)s[i];
        unsigned cp = c; size_t len = 1;
        if (c >= 0xF0 && i + 3 < s.size() + 0) { cp = ((c & 7u) << 18) | (((unsigned char)s[i+1] & 0x3Fu) << 12) | (((unsigned char)s[i+2] & 0x3Fu) << 6) | ((unsigned char)s[i+3] & 0x3Fu); len = 4; }
        else if (c >= 0xE0 && i + 2 < s.size()) { cp = ((c & 0xFu) << 12) | (((unsigned char)s[i+1] & 0x3Fu) << 6) | ((unsigned char)s[i+2] & 0x3Fu); len = 3; }
        else if (c >= 0xC0 && i + 1 < s.size()) { cp = ((c & 0x1Fu) << 6) | ((unsigned char)s[i+1] & 0x3Fu); len = 2; }
        v.push_back({(int)i, cp});
        i += len;
    }
    return v;
}

// Tibetan punctuation: the shad family and the ornaments. NOT the tsheg
// U+0F0B, which is a letter-level separator owned by the converters.
inline bool isTibetanPunctCp(unsigned cp) {
    return (cp >= 0x0F01 && cp <= 0x0F14) || (cp >= 0x0F3A && cp <= 0x0F3D) || cp == 0x0F85 || (cp >= 0x0FD0 && cp <= 0x0FD4);
}

// True when any byte pair opens U+0F00–U+0FFF (E0 BC / E0 BD).
inline bool hasTibetanUnicode(const std::string& s) {
    for (size_t i = 0; i + 2 < s.size(); ++i)
        if ((unsigned char)s[i] == 0xE0 && ((unsigned char)s[i+1] & 0xFC) == 0xBC) return true;
    return false;
}

// Every run of whitespace becomes one space; leading and trailing runs go.
// This is the cleanup a removal leaves behind, not a formatting rule.
inline std::string collapseWs(const std::string& s) {
    std::string o; bool ws = false;
    for (unsigned char c : s) {
        if (std::isspace(c)) { ws = true; continue; }
        if (ws && !o.empty()) o += ' ';
        ws = false; o += (char)c;
    }
    return o;
}

}  // namespace textutil
}  // namespace allcore
