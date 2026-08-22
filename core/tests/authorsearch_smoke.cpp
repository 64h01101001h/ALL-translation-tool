// authorsearch_smoke — finding a catalog author when you can only type
// the name one way, or only know how it sounds (backlog #33).
//
// The authors below are REAL strings from data/extracted/
// persons_bdrc.json, kept here verbatim so the battery does not need a
// JSON parser and does not drift when the bank is refreshed.
//
// Two defects found while building this are pinned as regressions:
//   * a phonetic query is ENGLISH PHONETICS, not wylie. Folding
//     "tsongkapa" through pronounce() as if it were wylie returned the
//     stub "tsong", which substring-matched an unrelated name.
//   * that same stub path made "sakya pandita" (fold "sapen") match
//     SHING BZA' PAndI TA ("shingsapenta"). A pronunciation that
//     collapses to under half the typed length is a stub, not a
//     reading, and is refused.
#include <cstdio>
#include <string>
#include <vector>

#include "allcore/authorsearch.h"

static int failures = 0;
#define CHECK(cond, msg)                                        \
    do {                                                        \
        if (cond) std::printf("  [PASS] %s\n", msg);            \
        else { std::printf("  [FAIL] %s\n", msg); ++failures; } \
    } while (0)

using namespace allcore;

static const std::vector<std::string> kAuthors = {
    "RJE TZONG KHA PA BLO BZANG GRAGS PA",
    "RGYAL BA DGE 'DUN GRUB",
    "LCANG SKYA NGAG DBANG BLO BZANG CHOS LDAN",
    "LCANG SKYA ROL PA'I RDO RJE",
    "SHING BZA' PAndI TA",
    "'GOS LO TZ'A BA GZHON NU DPAL",
};

static bool hasAuthor(const std::vector<AuthorHit>& h, const std::string& a) {
    for (const auto& x : h)
        if (x.author == a) return true;
    return false;
}
static const AuthorHit* find(const std::vector<AuthorHit>& h,
                             const std::string& a) {
    for (const auto& x : h)
        if (x.author == a) return &x;
    return nullptr;
}

int main() {
    // ---- canonical space: ACIP and wylie land in the same place ----
    CHECK(authorCanonical("TZONG KHA PA") == authorCanonical("tsong kha pa"),
          "ACIP and wylie normalize to one canonical form");
    CHECK(authorCanonical("RGYAL BA DGE 'DUN GRUB (1391-1474)") ==
              authorCanonical("RGYAL BA DGE 'DUN GRUB"),
          "life-dates are stripped from banked author strings");
    CHECK(authorCanonical("RJE TZONG  KHA PA") ==
              authorCanonical("RJE TZONG KHA PA"),
          "doubled spaces in the bank collapse");
    CHECK(authorCanonical("").empty() && authorCanonical("   ").empty(),
          "an empty name canonicalizes to nothing");

    // ---- Adam's core requirement: either script, same answer ----
    const auto acip = matchAuthors("TZONG KHA PA", kAuthors);
    const auto wylie = matchAuthors("tsong kha pa", kAuthors);
    CHECK(acip.size() == wylie.size() && !acip.empty() &&
              acip[0].author == wylie[0].author,
          "ACIP and wylie queries return the identical result");
    CHECK(acip[0].author == "RJE TZONG KHA PA BLO BZANG GRAGS PA" &&
              acip[0].tier == AuthorTier::Contains,
          "a name inside a longer name matches at the Contains tier");

    // ---- exact outranks everything ----
    const auto ex = matchAuthors("RGYAL BA DGE 'DUN GRUB", kAuthors);
    CHECK(!ex.empty() && ex[0].tier == AuthorTier::Exact,
          "an exact name match is reported as Exact and sorts first");

    // ---- pronunciation-approximate, the point of the feature ----
    const auto ph = matchAuthors("tsongkapa", kAuthors);
    CHECK(hasAuthor(ph, "RJE TZONG KHA PA BLO BZANG GRAGS PA"),
          "a phonetic spelling finds the name it sounds like");
    {
        const auto* h = find(ph, "RJE TZONG KHA PA BLO BZANG GRAGS PA");
        CHECK(h && h->tier >= AuthorTier::Phonetic,
              "a phonetic hit is tiered BELOW every orthographic tier");
        CHECK(h && h->evidence.find("sounds like") != std::string::npos,
              "a phonetic hit says out loud that it matched on sound");
    }
    CHECK(hasAuthor(matchAuthors("gendun drup", kAuthors),
                    "RGYAL BA DGE 'DUN GRUB"),
          "the 1st Dalai Lama is found from his English phonetic name");

    // ---- REGRESSION: the stub-pronunciation false positive ----
    CHECK(!hasAuthor(matchAuthors("sakya pandita", kAuthors),
                     "SHING BZA' PAndI TA"),
          "a stub pronunciation cannot match an unrelated author "
          "(regression: \"sapen\" vs \"shingsapenta\")");

    // ---- homonyms stay separate people ----
    const auto ck = matchAuthors("changkya", kAuthors);
    CHECK(hasAuthor(ck, "LCANG SKYA NGAG DBANG BLO BZANG CHOS LDAN") &&
              hasAuthor(ck, "LCANG SKYA ROL PA'I RDO RJE"),
          "both Changkyas come back as distinct people, never merged");

    // ---- never guess ----
    CHECK(matchAuthors("zzzznotaname", kAuthors).empty(),
          "a name nobody bears returns nothing rather than a guess");
    CHECK(matchAuthors("", kAuthors).empty() &&
              matchAuthors("   ", kAuthors).empty(),
          "an empty query returns nothing");
    CHECK(matchAuthors("a", kAuthors).empty(),
          "a fold shorter than the minimum never substring-matches "
          "everything");

    // ---- tier ordering holds across a mixed result ----
    {
        bool ordered = true;
        const auto mixed = matchAuthors("lcang skya", kAuthors);
        for (size_t i = 1; i < mixed.size(); ++i)
            if (mixed[i].tier < mixed[i - 1].tier) ordered = false;
        CHECK(ordered, "results are ordered strongest tier first");
    }

    std::printf(failures ? "AUTHORSEARCH SMOKE: %d failure(s)\n"
                         : "AUTHORSEARCH SMOKE OK (%d failures)\n",
                failures);
    return failures ? 1 : 0;
}
