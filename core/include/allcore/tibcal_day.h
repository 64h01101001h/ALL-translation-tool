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
// valid=false (the original refuses them too); intercalary-month
// bookkeeping (adj_zla state) is not yet ported — zla dag itself is.
// KCK computes the KALACAKRA karana; correspondence with published
// Phugpa almanacs is a separate, documented question.

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

}  // namespace allcore
