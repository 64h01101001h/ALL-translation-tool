#pragma once
// tibcal_phugpa.h — day-level TRUE PHUGPA calendar (grub rtsis).
//
// A faithful C++ port of the Generalised Phugpa epoch (epoch 1,
// year -1000) of Edward Henning's TCG (third_party/tcg_henning,
// MIT): true month (zla dag) with the Phugpa intercalation indices
// 48/49 and delayed months at 50/51, mean sun/weekday from the grub
// rtsis motions, true sun (nyi dag) and true weekday (gza' dag)
// with the grub correction tables, and the general day (spyi zhag)
// that yields the Julian day. Ported from the files, not from spec;
// proven month-by-month and day-by-day (tithi sequence, omitted and
// duplicated days, weekday, Gregorian date) against 21 full oracle
// years the unmodified original printed (fixtures/pl_2015..2035).
//
// This is THE published-almanac system: Losar 2024-26 match the
// almanacs and Espel's independent Phugpa (double-confirmed —
// docs/research/KCK_VS_PHUGPA.md). Beside it, allcore's KCK port
// remains the Kalacakra karana; the two intercalate differently by
// design.
//
// Scope note (honest): computation is defined for years >= 1027 CE
// (the first rab byung); the PROVEN span is 2015-2035. Callers
// outside the proven span get arithmetic per the original with no
// further claim.

#include <vector>

namespace allcore {

struct PhugpaDay {
    int tt = 0;            // tithi 1..30
    bool omitted = false;  // chad — no solar day bears this tithi
    bool duplicated = false;  // lhag — two solar days bear it
    long jd = 0;           // Julian day (for duplicated: the SECOND;
                           // the first is jd-1). 0 when omitted.
};

struct PhugpaMonth {
    int month = 0;             // 1..12 (display number)
    bool intercalary = false;  // zla bshol (first of the pair)
    bool delayed = false;      // the delayed regular month
    std::vector<PhugpaDay> days;
};

struct PhugpaYear {
    bool valid = false;
    std::vector<PhugpaMonth> months;
    long losar_jd = 0;  // JD of tithi 1 of the year's first month
};

// The Tibetan year beginning in western year `y` — the exact month
// and day sequence the oracle prints for that year.
PhugpaYear phugpaYear(int y);

}  // namespace allcore
