// affixnorm_smoke — BDRC affix-stripping port, driven end-to-end
// through our own proven wylie->unicode converter.
#include <cstdio>
#include <string>

#include "allcore/affixnorm.h"
#include "allcore/engines.h"

static int failures = 0;
#define CHECK(cond, msg)                                        \
    do {                                                        \
        if (cond) std::printf("  [PASS] %s\n", msg);            \
        else { std::printf("  [FAIL] %s\n", msg); ++failures; } \
    } while (0)

static std::string N(const char* wylie) {
    return allcore::stripAffixedParticles(
        allcore::wylieToUnicode(wylie).first);
}
static std::string U(const char* wylie) {
    return allcore::wylieToUnicode(wylie).first;
}

int main() {
    // the canonical documented case: po'i -> po
    CHECK(N("po'i") == U("po"), "po'i -> po");
    CHECK(N("po'o") == U("po"), "po'o -> po");
    CHECK(N("pos") == U("pos"), "pos untouched (no affix)");
    CHECK(N("khyod kyi") == U("khyod kyi"),
          "multi-codepoint syllable without affix untouched");
    // 'is
    CHECK(N("des") == U("des"), "des untouched");
    CHECK(N("de'is") == U("de"), "de'is -> de");
    // 'am / 'ang
    // consonant-final host + 'am: built at codepoint level (EWTS
    // "yin'am" reads as a subjoined-achung stack in converters — the
    // filter sees tokenized Tibetan text, not wylie)
    CHECK(allcore::stripAffixedParticles(U("yin") + "\u0F60\u0F58") ==
              U("yin"),
          "yin+'am (codepoints) -> yin");
    CHECK(N("de'ang") == U("de"), "de'ang -> de");
    // stacked double particles
    CHECK(N("de'i'o") == U("de"), "de'i'o -> de");
    // the needsAA protection: dga' and mkha' keep their own 'a
    CHECK(N("dga'i") == U("dga'"), "dga'i -> dga' (root 'a kept)");
    CHECK(N("mkha'i") == U("mkha'"), "mkha'i -> mkha' (root 'a kept)");
    CHECK(N("dbe'i") == U("dbe"),
          "dbe'i -> dbe (no root 'a needed)");
    // bare trailing 'a on a long syllable is trimmed for search
    CHECK(N("bka'") == U("bka'"),
          "bka' kept (b+k needs the 'a)");
    // idempotence: normalizing twice changes nothing
    CHECK(allcore::stripAffixedParticles(N("po'i")) == N("po'i"),
          "idempotent");
    // non-Tibetan passthrough
    CHECK(allcore::stripAffixedParticles("abc") == "abc",
          "non-Tibetan text passes through");

    std::printf("%s (%d failures)\n",
                failures ? "AFFIXNORM SMOKE FAILED" : "AFFIXNORM SMOKE OK",
                failures);
    return failures ? 1 : 0;
}
