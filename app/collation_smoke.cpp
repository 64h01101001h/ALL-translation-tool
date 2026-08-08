// collation_smoke — Tibetan dictionary-order collation via ICU/CLDR
// (QCollator, locale "bo"; the official CLDR Tibetan rules, in Unicode
// since 2021). Fixtures encode the traditional ordering scheme as taught
// in Wilson's Translating Buddhism from Tibetan (root-letter sections;
// within a section: bare root + vowels a-i-u-e-o, suffixes in alphabet
// order, subjoined stacks — incl. Sanskrit ཀྵ as a ka-stack — then
// prefixed and double-prefixed forms), each asserted pairwise. The probe
// that chose ICU over a hand port is banked in this battery: if a Qt/ICU
// upgrade ever changes the ordering, this fails loudly.
#include <QCollator>
#include <QCoreApplication>
#include <QLocale>

#include <cstdio>

static int failures = 0;
#define CHECK(cond, msg)                                        \
    do {                                                        \
        if (cond) std::printf("  [PASS] %s\n", msg);            \
        else { std::printf("  [FAIL] %s\n", msg); ++failures; } \
    } while (0)

static bool ordered(QCollator& c, const char* const* seq, int n,
                    const char* label) {
    bool ok = true;
    for (int i = 0; i + 1 < n; ++i) {
        if (c.compare(QString::fromUtf8(seq[i]),
                      QString::fromUtf8(seq[i + 1])) >= 0) {
            std::printf("    %s: '%s' !< '%s'\n", label, seq[i], seq[i + 1]);
            ok = false;
        }
    }
    return ok;
}

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);
    QCollator bo(QLocale("bo"));

    // the thirty base letters
    const char* alphabet[] = {"ཀ", "ཁ", "ག", "ང", "ཅ", "ཆ", "ཇ", "ཉ",
                              "ཏ", "ཐ", "ད", "ན", "པ", "ཕ", "བ", "མ",
                              "ཙ", "ཚ", "ཛ", "ཝ", "ཞ", "ཟ", "འ", "ཡ",
                              "ར", "ལ", "ཤ", "ས", "ཧ", "ཨ"};
    CHECK(ordered(bo, alphabet, 30, "alphabet"),
          "the thirty letters in alphabet order");

    // vowels: a (unwritten) then i u e o
    const char* vowels[] = {"ཀ", "ཀི", "ཀུ", "ཀེ", "ཀོ"};
    CHECK(ordered(bo, vowels, 5, "vowels"), "vowel order a i u e o");

    // suffixes in alphabet order, all before the i-vowel forms
    const char* suffixes[] = {"ཀ",  "ཀག", "ཀགས", "ཀང", "ཀངས", "ཀད",
                              "ཀན", "ཀབ", "ཀབས", "ཀམ", "ཀམས", "ཀའ",
                              "ཀར", "ཀལ", "ཀས",  "ཀི"};
    CHECK(ordered(bo, suffixes, 16, "suffixes"),
          "suffix order (ga nga da na ba ma 'a ra la sa), then vowels");

    // one full root-letter section: bare + vowels, subjoined ya/ra/la +
    // the Sanskrit ཀྵ stack, prefixed d/b/r/l/s, double prefixes, then kha
    const char* section[] = {"ཀ",  "ཀི",  "ཀྱ",  "ཀྱི", "ཀྲ", "ཀླ",
                             "ཀྵ", "དཀའ", "བཀའ", "རྐ",  "ལྐོ", "སྐ",
                             "བརྐ", "བསྐ", "ཁ"};
    CHECK(ordered(bo, section, 15, "ka-section"),
          "full ka section: stacks then prefixed forms, dka'/bka' under KA");

    // multi-syllable: a bare first syllable sorts before its suffixed forms
    const char* multi[] = {"ཀ་ཀ", "ཀ་ཁ", "ཀག"};
    CHECK(ordered(bo, multi, 3, "multi"),
          "second syllable decides before first-syllable suffixes");

    // the ཀྵ tailoring is real CLDR work, not codepoint order (U+0F69
    // would sort after ཁ U+0F41 by codepoint)
    CHECK(bo.compare(QString::fromUtf8("ཀྵ"), QString::fromUtf8("ཁ")) < 0,
          "kSSa sorts inside the ka section (CLDR tailoring active)");

    std::printf("collation_smoke: %s (%d failures)\n",
                failures ? "FAIL" : "ALL PASS", failures);
    return failures ? 1 : 0;
}
