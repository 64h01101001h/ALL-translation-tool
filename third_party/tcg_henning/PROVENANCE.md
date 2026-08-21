# TCG — Tibetan Calendar software for grub rtsis (Edward Henning)

Downloaded 2026-08-14 from
http://www.kalacakra.org/calendar/tcg1309.zip (v1.06, 2013-09-03),
linked from os_tib.htm ("Open source software for traditional
Tibetan calendars") — found via Adam's directed re-scour of
DigitalTibetan's calendar page (a genuine gap in the first pass).

License: MIT (verbatim in every source header),
© 2009–2013 Edward Henning.

**What it is — and why it matters beside our vendored KCK**: KCK
(`third_party/kck_henning`) computes the KALACAKRA KARANA (byed
rtsis). TCG computes the SIDDHANTA systems (grub rtsis): the five
epochs are 1. Generalised Phugpa (−1000), 2. Generalised Tsurphu
(−1000), 3. Generalised Error Correction (−2000), 4. mkhas pa'i
snying nor (1796), 5. New Genden (1747). This is the reference C
implementation of TRUE PHUGPA — the published-almanac standard —
from the same author whose KCK we port-and-proved.

Builds on macOS with the KCK shims:
  cc -w -I. -std=gnu89 -x c -o tcg t1.c t2.c t3.c t4.c utils.c bcd.c tests.c
(`tests.c` is required — t1 calls srch_cyc; `void main` → `int
main` first.) Drive: epoch key, then menu 2, start year, finish
year, 9. Output PL_YYYY.TXT per year.

Verified at banking time: Phugpa Losar 2025 = Fri 28 Feb 2025 —
identical to Espel's independent Perl Phugpa AND the published
almanac (docs/research/KCK_VS_PHUGPA.md, now double-confirmed).
Role: oracle for the KCK-vs-Phugpa question TODAY; the
port-and-prove SOURCE for a true Phugpa lane WHENEVER WANTED
(fixtures-first, exactly like the KCK port). PL_2024/25/26.TXT
banked as first fixtures.
