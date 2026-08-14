# KCK (Modern Karana) vs published Phugpa — the answer (2026-08-14)

*The open question documented in `tibcal_day.h` since 2026-08-07
("correspondence with published Phugpa almanacs is a separate,
documented question"), answered with an independent oracle found
via DigitalTibetan: Roger Espel Llima's `Calendar-Phugpa` Perl
module (lotsawahouse.org — no explicit license statement, so used
as a NON-vendored diff-oracle only; download recipe below).*

## Method

Full-year day tables from both implementations for three years,
joined on (month, leap-flag, tshes), date deltas histogrammed.
Our side: `kckYearMonths` + `kckDayForTrueMonth` → JD → western.
Espel's side: `year_calendar(tib_year)` (tib_year = western+127;
his missing `Time::JulianDay` dep shimmed with the standard
Fliegel–Van Flandern algorithms).

## Results

| Year | KCK leap | Phugpa leap | Losar KCK | Losar Phugpa | identical days |
|---|---|---|---|---|---|
| 2024/2151 | none | month 6 | 2024-02-10 | 2024-02-10 | 37.3% (all pre-leap) |
| 2025/2152 | month 9 | none | **2025-01-30** | **2025-02-28** | 21.8% |
| 2026/2153 | none | none | 2026-02-18 | 2026-02-18 | **89.9%** |

Real-world check — the published Losars were Feb 10 2024,
**Feb 28 2025**, Feb 18 2026: **Phugpa matches all three; KCK
misses 2025 by a month.**

## The finding, plainly

1. **The two systems intercalate differently.** Phugpa doubled
   month 6 of 2151; Modern Karana doubled month 9 of 2152. From a
   leap point until the systems re-align, every date differs by
   ~one month (the −29/−30 delta bands).
2. **In leap-free years they nearly agree**: same Losar, ~90% of
   days identical, the rest ±1 day from differing duplicated/
   omitted-day (lhag/chad) placement.
3. **Consequence for the tool** (already stated honestly in the
   UI, now with numbers): the day-level lane computes the
   KALACAKRA KARANA — perfect for karana-tradition work and
   proven 26 years against Henning's own program — but for
   dating against PUBLISHED almanacs (practice days, modern
   colophons), Phugpa is the standard. A true Phugpa port would
   start from Svante Janson's "Tibetan calendar mathematics"
   (Uppsala) — the math is fully specified there and Espel's
   module can serve as its oracle at port time.

## Reproduce

```
curl -sL -o phugpa.zip https://www.lotsawahouse.org/Static/Calendar-Phugpa-dev.zip
unzip phugpa.zip && perl -I shim -I Calendar-Phugpa/lib dump_year.pl 2153
```
(shim = the two-function Time::JulianDay replacement; the dump
driver is 15 lines — both preserved in the session scratchpad
pattern and reproducible from this doc.)

## Addendum (same day): double confirmation + the port source

Adam's directed re-scour caught a real gap: kalacakra.org's
os_tib.htm — **Henning's OWN open-source C implementation of the
grub-rtsis systems** (TCG v1.06, MIT), including **Generalised
Phugpa and Tsurphu** — distinct from the KCK karana program we
had vendored. Banked at `third_party/tcg_henning` with build
recipe. Verified immediately: TCG's Phugpa Losar 2025 = **Fri 28
Feb 2025**, identical to Espel's independent Perl and the
published almanac — the Phugpa side of this comparison is now
confirmed by two unrelated implementations. Consequence: a true
Phugpa lane in allcore is no longer hypothetical — TCG is the
port source, Janson the spec, Espel + TCG prints the oracles,
fixtures-first exactly like the proven KCK port. (Also banked
from the same page family: the intercalation and epochs
companion pages, Yongdan's and Jo's calendar-history papers,
and kalacakra.org/calendar/kcal.htm as the source collection.)
