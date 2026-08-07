// tibcal_smoke — the colophon-year engine, pinned by documented
// anchor years from independent published sources.
#include <cstdio>

#include "allcore/tibcal.h"

static int failures = 0;
#define CHECK(cond, msg)                                        \
    do {                                                        \
        if (cond) std::printf("  [PASS] %s\n", msg);            \
        else { std::printf("  [FAIL] %s\n", msg); ++failures; } \
    } while (0)

int main() {
    using allcore::tibetanYear;
    using allcore::yearsFor;

    // anchor: 1027 = fire-hare, year 1 of rabjung 1 (the epoch itself)
    auto y = tibetanYear(1027);
    CHECK(y.element_en == "fire" && y.animal_en == "hare" &&
              y.rabjung == 1 && y.year_in_cycle == 1 && y.female,
          "1027 = fire-female-hare, rabjung 1 year 1");
    // anchor: 1987 = fire-hare, start of rabjung 17 (widely documented)
    y = tibetanYear(1987);
    CHECK(y.element_en == "fire" && y.animal_en == "hare" &&
              y.rabjung == 17 && y.year_in_cycle == 1,
          "1987 = fire-hare, rabjung 17 year 1");
    // anchor: 2000 = iron-dragon (universally documented Losar year)
    y = tibetanYear(2000);
    CHECK(y.element_en == "iron" && y.animal_en == "dragon" &&
              !y.female,
          "2000 = iron-male-dragon");
    // anchor: 1959 = earth-pig
    y = tibetanYear(1959);
    CHECK(y.element_en == "earth" && y.animal_en == "pig" && y.female,
          "1959 = earth-female-pig");
    // anchor: 1357, Je Tsongkapa's birth year = fire-bird (classical
    // biographies; the dates 1357-1419 appear in our own volumes)
    y = tibetanYear(1357);
    CHECK(y.element_en == "fire" && y.animal_en == "bird",
          "1357 (Je Tsongkapa's birth) = fire-bird");
    // and his death 1419 = earth-pig
    y = tibetanYear(1419);
    CHECK(y.element_en == "earth" && y.animal_en == "pig",
          "1419 (his passing) = earth-pig");
    CHECK(y.element_wylie == "sa" && y.animal_wylie == "phag",
          "wylie names carried (sa phag)");

    // reverse: fire-bird within rabjung 6 must be exactly 1357
    auto v = yearsFor("fire", "bird", 6);
    CHECK(v.size() == 1 && v[0] == 1357,
          "fire-bird of rabjung 6 -> 1357 uniquely");
    // reverse by wylie, unconstrained: 60-year spacing
    v = yearsFor("me", "bya", 0, 1900, 2100);
    CHECK(v.size() == 3 && v[0] == 1957 && v[1] == 2017 &&
              v[2] == 2077,
          "me bya in 1900-2100 -> 1957, 2017, 2077");
    // parity: iron-hare exists (1951), but fire-dragon-in-rabjung-17
    // vs an impossible pairing: wood-hare occurs (odd parity ok) —
    // use a genuinely impossible combo: elements pair with fixed
    // parity, so e.g. "fire rat" (rat is even slot) must never occur
    // with female parity mismatch — verify the grid is half-empty:
    int count = 0;
    for (const char* el : {"fire", "earth", "iron", "water", "wood"})
        for (const char* an :
             {"hare", "dragon", "snake", "horse", "sheep", "monkey",
              "bird", "dog", "pig", "rat", "ox", "tiger"})
            count += (int)!yearsFor(el, an, 1).empty();
    CHECK(count == 60,
          "exactly 60 of 60 element-animal pairs occur per rabjung");

    // pre-epoch honesty
    CHECK(tibetanYear(1000).rabjung == 0,
          "years before 1027 say so (rabjung 0), never guessed");

    std::printf("%s (%d failures)\n",
                failures ? "TIBCAL SMOKE FAILED" : "TIBCAL SMOKE OK",
                failures);
    return failures ? 1 : 0;
}
