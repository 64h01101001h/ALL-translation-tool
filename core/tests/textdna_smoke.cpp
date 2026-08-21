// textdna_smoke — the Text DNA classifier on constructed documents
// (the verse arithmetic underneath carries its own battery; these
// pins prove the CLASSES and the quote-mark shape).
#include <cstdio>
#include <string>

#include "allcore/textdna.h"

static int failures = 0;
#define CHECK(cond, msg)                                        \
    do {                                                        \
        if (cond) std::printf("  [PASS] %s\n", msg);            \
        else { std::printf("  [FAIL] %s\n", msg); ++failures; } \
    } while (0)

int main() {
    setvbuf(stdout, nullptr, _IONBF, 0);
    using allcore::DnaClass;
    {   // a verse text: 7-syllable lines, one broken line, a blank
        const std::string doc =
            "SEMS CAN THAMS CAD BDE BA SHOG\n"
            "SDUG BSNGAL KUN LAS THAR BAR SHOG\n"
            "\n"
            "BDE BA'I RGYU LA,\n"
            "SANGS RGYAS BSTAN PA DAR BAR SHOG\n"
            "CHOS KYI 'KHOR LO BSKOR BAR SHOG\n"
            "DGE 'DUN THUGS MTHUN BYUNG BAR SHOG\n";
        const auto r = allcore::textDna(doc);
        CHECK(r.is_verse && r.meter == 7,
              "a 7-syllable text reads as verse, meter 7");
        CHECK(r.lines.size() == 7, "every line is reported");
        CHECK(r.lines[2].cls == DnaClass::Blank,
              "the blank line classifies Blank");
        CHECK(r.lines[3].cls == DnaClass::VerseIrregular,
              "the broken line classifies VerseIrregular");
        CHECK(r.lines[0].cls == DnaClass::Verse,
              "a regular line classifies Verse");
    }
    {   // prose with a quote announcement
        const std::string doc =
            "DE LA 'DIR RGYAL BA'I BKA' DANG BSTAN BCOS RNAMS LAS JI "
            "LTAR 'BYUNG BA BZHIN BSHAD PAR BYA STE,\n"
            "SEMS CAN THAMS CAD BDE BA DANG LDAN PAR GYUR CIG CES "
            "GSUNGS PA LTAR RO,\n";
        const auto r = allcore::textDna(doc);
        CHECK(!r.is_verse, "irregular long lines read as prose");
        CHECK(r.lines[0].cls == DnaClass::Prose &&
                  !r.lines[0].quote_mark,
              "a plain prose line carries no quote mark");
        CHECK(r.lines[1].quote_mark,
              "CES + GSUNGS within reach marks the quote line");
    }
    std::printf("%s: %d failure(s)\n", failures ? "FAIL" : "OK",
                failures);
    return failures ? 1 : 0;
}
