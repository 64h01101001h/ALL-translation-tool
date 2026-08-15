#pragma once
// tibcal_day.h — day-level Tibetan calendar: the Modern Karana epoch.
//
// A faithful C++ port of the traditional karana arithmetic in Edward
// Henning's KCK (third_party/kck_henning, MIT): true month (zla dag),
// mean/true weekday (gza' dag), true sun (nyi dag), and the general
// day count (spyi zhag) that yields the Julian day — for the Modern
// Karana epoch (24 Feb 2009). Radix arithmetic is places
// (units; 60; 60; 6; 13) exactly as in the original; every quantity is
// diff-proven against 76 fixture outputs captured from the original
// program running unmodified (kck_fixtures.tsv).
//
// Scope notes (honest): epoch 3 only; dates before the epoch return
// valid=false (the original refuses them too). Intercalary-month
// bookkeeping IS proven: kckYearMonths + kckDayForTrueMonth are
// diff-proven against 26 full oracle years (kc_2010..kc_2035:
// 321 month headers, 9,479 day dates, 210 duplicated-day pairs —
// tools/gen_kck_year_fixtures.py → kck_year_fixtures.tsv). The epoch
// year 2009 itself is REFUSED by kckYearMonths (the original's own
// print of it is boundary-corrupted; pre-epoch months mix in) —
// refused rather than guessed. KCK computes the KALACAKRA karana;
// correspondence with published Phugpa almanacs is now QUANTIFIED
// (docs/research/KCK_VS_PHUGPA.md, Espel Phugpa oracle): identical
// Losar and ~90% identical days in leap-free years, but the two
// systems INTERCALATE DIFFERENTLY — in leap-affected stretches
// dates differ by a month (2025: KCK Losar Jan 30 vs published/
// Phugpa Feb 28). For published-almanac dating, Phugpa is the
// standard — NOW PORTED: allcore/tibcal_phugpa.h (proven against
// 21 full TCG oracle years, suite phugpa_smoke).

#include <array>
#include <vector>

namespace allcore {

struct KckDay {
    bool valid = false;
    long zla0 = 0, zla1 = 0;          // true-month count ; 65-fraction
    std::array<long, 5> gza{};        // gza' dag: 7; 60,60,6,13
    std::array<long, 5> nyi{};        // nyi dag: 27; 60,60,6,13
    long spyi = 0;                    // spyi zhag (general day)
    long jd = 0;                      // Julian day at noon UT
};

// tshes (1..30), Tibetan month (1..12), western year the Tibetan year
// begins in — Modern Karana epoch (2009 onward).
KckDay kckModernKarana(long year, long month, long tshes);

// Gregorian (proleptic-correct incl. the 1582 changeover, ported from
// gregd2jul) -> Julian day at noon
long julianDay(long day, long month, long year);
// Julian day -> Gregorian {day, month, year} (ported jul2date logic)
struct WesternDate { long day = 0, month = 0, year = 0; };
WesternDate westernFromJd(long jd);

// inverse: all Tibetan dates whose gza-dag day falls on this Julian
// day. 0 results = an omitted (chad) lunar day; 2 = a duplicated
// (lhag) one — both real calendar phenomena, reported honestly.
struct KckMatch { long year, month, tshes; KckDay day; };
std::vector<KckMatch> kckFromJulianDay(long jd);

// day computed directly from the TRUE month count (zla0) — the only
// way to reach an intercalary month's days (kckModernKarana's
// (year, month) always resolves to the regular sibling)
KckDay kckDayForTrueMonth(long true_month, long tshes);

// the months of a Tibetan year with the original's display
// convention: an intercalation appears as "(display m) intercalary"
// BEFORE regular month m (fixture kc_2012: 3, 3-intercalary, 4).
// Month names: animals are fixed to the display month (1 = Tiger);
// elements advance one step per pair continuously across years —
// both proven against the banked full-year fixtures.
struct KckMonth {
    int display_month = 0;      // 1..12
    bool intercalary = false;
    long true_month = 0;        // zla0 — feed to kckDayForTrueMonth
    std::string element_en;
    std::string animal_en;
    bool female = false;
};
std::vector<KckMonth> kckYearMonths(long year);

}  // namespace allcore
