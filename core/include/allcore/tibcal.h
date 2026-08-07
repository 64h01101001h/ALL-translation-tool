#pragma once
// tibcal.h — Tibetan calendar: the colophon-dating helper (year level).
//
// The 60-year rabjung cycle: cycle 1 begins 1027 CE (fire-hare, me yos).
// Element-animal naming is exact modular arithmetic — element pairs
// advance on the even year of each pair (fire-male-tiger 1026,
// fire-female-hare 1027). Pinned by documented anchors in the battery
// (1027 fire-hare · 1357 fire-bird · 1959 earth-pig · 1987 fire-hare ·
// 2000 iron-dragon).
//
// Year level ONLY: month/day conversion (Phugpa/Tsurphu lunar
// arithmetic) is a future port from Janson's mathematics with
// Henning's MIT KCK sources (third_party/kck_henning) as reference.
// NOTE: the Tibetan year straddles the western new year (Losar falls
// in Feb/Mar); "gregorian year Y" here means the western year in which
// that Tibetan year BEGINS.

#include <string>
#include <vector>

namespace allcore {

struct TibetanYear {
    int gregorian = 0;        // western year the Tibetan year begins in
    int rabjung = 0;          // 60-year cycle number (1 = 1027..1086)
    int year_in_cycle = 0;    // 1..60
    std::string element_en;   // fire / earth / iron / water / wood
    std::string element_wylie;
    std::string animal_en;    // hare / dragon / …
    std::string animal_wylie;
    bool female = false;      // odd years female, even male
};

// gregorian year -> its Tibetan year designation (valid 1027 onward;
// gregorian < 1027 returns rabjung 0 = "before the first rabjung")
TibetanYear tibetanYear(int gregorian);

// element + animal (english or wylie, case-insensitive) -> all matching
// gregorian years in [from, to]; with rabjung > 0 the unique year of
// that cycle (empty vector = no such combination: only half the
// element-animal grid occurs, parity must match)
std::vector<int> yearsFor(const std::string& element,
                          const std::string& animal, int rabjung = 0,
                          int from = 1027, int to = 2100);

}  // namespace allcore
