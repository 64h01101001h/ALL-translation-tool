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

    // wylie projection: defined by the unicode port, so they must agree
    using allcore::stripAffixedParticlesWylie;
    CHECK(stripAffixedParticlesWylie("po'i") == "po",
          "wylie: po'i -> po");
    CHECK(stripAffixedParticlesWylie("sems dpa'i") == "sems dpa'",
          "wylie: sems dpa'i -> sems dpa' (root 'a kept)");
    CHECK(stripAffixedParticlesWylie("dga'i") == "dga'",
          "wylie: dga'i -> dga' (guard projected)");
    CHECK(stripAffixedParticlesWylie("byang chub kyi") ==
              "byang chub kyi",
          "wylie: no affix -> unchanged");
    CHECK(stripAffixedParticlesWylie("de'ang") == "de",
          "wylie: de'ang -> de");

    // Old-Tibetan da-drag: archaic second-suffix ད after ན/ར/ལ
    using allcore::stripDaDrag;
    using allcore::stripDaDragWylie;
    CHECK(stripDaDragWylie("gyurd") == "gyur",
          "da-drag wylie: gyurd -> gyur");
    CHECK(stripDaDragWylie("stond") == "ston",
          "da-drag wylie: stond -> ston");
    CHECK(stripDaDragWylie("brtsald") == "brtsal",
          "da-drag wylie: brtsald -> brtsal");
    CHECK(stripDaDragWylie("kun") == "kun",
          "da-drag wylie: kun unchanged");
    CHECK(stripDaDragWylie("byed") == "byed",
          "da-drag wylie: byed unchanged (e is not n/r/l)");
    CHECK(stripDaDrag("གརད") == "གར",
          "da-drag unicode: gard-class strips final da");
    CHECK(stripDaDrag("ཀུན") ==
              "ཀུན",
          "da-drag unicode: kun unchanged");

    {   // SQA TEST-1 survivor AFFIXNORM-BOUNDARY (2026-08-24). The
        // stacked double-particle rule strips the LAST FOUR
        // codepoints, and its guard `len > 4` is what guarantees
        // there is a root syllable left underneath. Flipping it to
        // `len > 3` survived the whole battery.
        //
        // The damage is total rather than subtle. A syllable that is
        // EXACTLY the four particle codepoints and nothing else -
        // 'i'o, U+0F60 U+0F72 U+0F60 U+0F7C - would match at len == 4
        // and strip to the empty string. Normalisation would
        // annihilate the syllable rather than normalise it, and a
        // word would vanish from a search index without a word said.
        const std::string bare = N("'i'o");
        CHECK(!bare.empty(),
              "stripping particles never annihilates the syllable: a "
              "bare 'i'o keeps a root (TEST-1 AFFIXNORM-BOUNDARY)");
    }

    std::printf("%s (%d failures)\n",
                failures ? "AFFIXNORM SMOKE FAILED" : "AFFIXNORM SMOKE OK",
                failures);
    return failures ? 1 : 0;
}
