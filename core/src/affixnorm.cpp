#include "allcore/affixnorm.h"

#include "allcore/engines.h"

#include <vector>

namespace allcore {

namespace {

// UTF-8 <-> UTF-32 helpers for the small Tibetan block
std::vector<char32_t> decode(const std::string& s) {
    std::vector<char32_t> out;
    for (size_t i = 0; i < s.size();) {
        unsigned char c = s[i];
        char32_t cp = 0;
        int n = 1;
        if (c < 0x80) { cp = c; }
        else if ((c >> 5) == 6) { cp = c & 0x1F; n = 2; }
        else if ((c >> 4) == 14) { cp = c & 0x0F; n = 3; }
        else { cp = c & 0x07; n = 4; }
        for (int k = 1; k < n && i + k < s.size(); ++k)
            cp = (cp << 6) | (s[i + k] & 0x3F);
        out.push_back(cp);
        i += n;
    }
    return out;
}

std::string encode(const std::vector<char32_t>& v, size_t len) {
    std::string s;
    for (size_t i = 0; i < len; ++i) {
        char32_t cp = v[i];
        if (cp < 0x80) s += (char)cp;
        else if (cp < 0x800) {
            s += (char)(0xC0 | (cp >> 6));
            s += (char)(0x80 | (cp & 0x3F));
        } else {
            s += (char)(0xE0 | (cp >> 12));
            s += (char)(0x80 | ((cp >> 6) & 0x3F));
            s += (char)(0x80 | (cp & 0x3F));
        }
    }
    return s;
}

// port of needsAASuffix(prefix, mainStack) — true when the bare
// prefix+main combination is only legal with a final འ (so the འ is
// the word's own, not the particle's carrier)
bool needsAA(char32_t p, char32_t m) {
    switch (p) {
        case U'ག':
            switch (m) {
                case U'ཅ': case U'ཉ': case U'ཏ': case U'ད':
                case U'ན': case U'ཙ': case U'ཞ': case U'ཟ':
                case U'ཡ': case U'ཤ': case U'ས': return true;
                default: return false;
            }
        case U'ད':
            switch (m) {
                case U'ཀ': case U'ག': case U'ང': case U'པ':
                case U'བ': case U'མ': return true;
                default: return false;
            }
        case U'བ':
            switch (m) {
                case U'ཀ': case U'ག': case U'ཅ': case U'ཏ':
                case U'ད': case U'ཙ': case U'ཞ': case U'ཟ':
                case U'ཤ': case U'ས': return true;
                default: return false;
            }
        case U'མ':
            switch (m) {
                case U'ཁ': case U'ག': case U'ང': case U'ཆ':
                case U'ཇ': case U'ཉ': case U'ཐ': case U'ད':
                case U'ན': case U'ཚ': case U'ཛ': return true;
                default: return false;
            }
        case U'འ':
            switch (m) {
                case U'ཁ': case U'ག': case U'ཆ': case U'ཇ':
                case U'ཐ': case U'ད': case U'ཕ': case U'བ':
                case U'ཚ': case U'ཛ': return true;
                default: return false;
            }
        default: return false;
    }
}

constexpr char32_t AA = U'འ', I = U'ི', O = U'ོ', U_ = U'ུ';
constexpr char32_t NG = U'ང', M = U'མ', S = U'ས', R = U'ར';

}  // namespace

std::string stripAffixedParticles(const std::string& syl) {
    auto b = decode(syl);
    size_t len = b.size();

    // stacked double particles འིའོ / འིའམ / འིའང / འོའམ / འོའང
    if (len > 4 && b[len - 4] == AA &&
        ((b[len - 3] == I && b[len - 2] == AA &&
          (b[len - 1] == O || b[len - 1] == NG || b[len - 1] == M)) ||
         (b[len - 3] == O && b[len - 2] == AA &&
          (b[len - 1] == NG || b[len - 1] == M)))) {
        if (len == 6 && needsAA(b[len - 6], b[len - 5]))
            return encode(b, len - 3);
        return encode(b, len - 4);
    }
    // འིས
    if (len > 3 && b[len - 3] == AA && b[len - 2] == I &&
        b[len - 1] == S) {
        if (len == 5 && needsAA(b[len - 5], b[len - 4]))
            return encode(b, len - 2);
        return encode(b, len - 3);
    }
    // འི / འོ / འམ / འང
    if (len > 2 && b[len - 2] == AA &&
        (b[len - 1] == I || b[len - 1] == O || b[len - 1] == M ||
         b[len - 1] == NG)) {
        if (len == 4 && needsAA(b[len - 4], b[len - 3]))
            len = len - 1;
        else
            len = len - 2;
    }
    // འུར / འུས (only when something precedes the འུ)
    if (len > 4 && b[len - 3] == AA && b[len - 2] == U_ &&
        (b[len - 1] == R || b[len - 1] == S)) {
        len = len - 1;
    }
    // trailing bare འ that the syllable does not need
    if ((len > 3 || len == 2) && b[len - 1] == AA)
        len = len - 1;
    else if (len == 3 && b[len - 1] == AA && !needsAA(b[0], b[1]))
        len = len - 1;

    return encode(b, len);
}

std::string stripDaDrag(const std::string& syl) {
    auto b = decode(syl);
    const size_t len = b.size();
    if (len >= 3 && b[len - 1] == U'ད' &&
        (b[len - 2] == U'ན' || b[len - 2] == R ||
         b[len - 2] == U'ལ'))
        return encode(b, len - 1);
    return syl;
}

std::string stripDaDragWylie(const std::string& w) {
    const size_t n = w.size();
    if (n >= 4 && w[n - 1] == 'd' &&
        (w[n - 2] == 'n' || w[n - 2] == 'r' || w[n - 2] == 'l'))
        return w.substr(0, n - 1);
    return w;
}

std::string stripAffixedParticlesWylie(const std::string& w) {
    static const char* kSuff[] = {"'i'o", "'i'am", "'i'ang", "'o'am",
                                  "'o'ang", "'is",  "'i",    "'o",
                                  "'am",   "'ang",  "'ur",   "'us"};
    for (const char* suf : kSuff) {
        const size_t n = std::string(suf).size();
        if (w.size() <= n || w.compare(w.size() - n, n, suf) != 0)
            continue;
        // candidate strip — the unicode port decides (incl. the
        // needsAA guard and the 'ur/'us keep-final-letter rule)
        const std::string last =
            w.find(' ') == std::string::npos
                ? w
                : w.substr(w.rfind(' ') + 1);
        const auto [u, ok] = wylieToUnicode(last);
        if (!ok) return w;
        const std::string stripped = stripAffixedParticles(u);
        if (stripped == u) return w;   // port says: no strip (dga'i …)
        // project back: find the wylie prefix whose conversion equals
        // the stripped unicode
        for (size_t cut = w.size() - 1; cut > 0; --cut) {
            const std::string candWord = w.substr(0, cut);
            const std::string candLast =
                candWord.find(' ') == std::string::npos
                    ? candWord
                    : candWord.substr(candWord.rfind(' ') + 1);
            const auto [cu, cok] = wylieToUnicode(candLast);
            if (cok && cu == stripped) return candWord;
        }
        return w;
    }
    return w;
}

}  // namespace allcore
