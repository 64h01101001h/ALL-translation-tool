# Pecha vs print — measured woodblock geometry
*2026-08-13 · unblocks the pecha-v3 items "syllable budget vs
prints" and "margin presets" using our own scan store as the
reference prints (no waiting on physical materials).*

## Source
14 sides sampled evenly from the 872-page scan volume in the
offline store (`library/scans/rnam 'grel/` — the rnam bshad snying
po'i rgyan print blocks Adam browsed on BUDA). Measurement:
binarize, ink-density profiles → text-block bounding box, smoothed
row profile → line count (script inline in session; medians below,
one outlier side excluded per metric).

## Measurements (medians)
| metric | value |
|---|---|
| lines per side | **8** (range 6–9; 8 clearly modal) |
| text-block aspect (w:h) | **8.1 : 1** |
| block width / folio width | **0.85** |
| block height / folio height | **0.59** |
| implied folio aspect | **≈5.6 : 1** |

Note: scans may crop the physical folio edge, so the folio aspect
is a scan-frame figure; the block geometry and line count are
solid.

## What changed in the app
Pecha Maker gained the preset **"Measured woodblock (46 × 8.2 cm,
8 lines)"** — 460×82 mm (5.6:1), 8 lines per side, matching the
measured print. The four existing presets are untouched.

## Syllable budget — MEASURED (same day, no OCR needed)
The eKangyur's own `[folio.line]` markers make the Degé print's
line lengths exact text statistics: **47,728 full lines across 11
sampled volumes → median 54 syllables/line (mean 53.7, p10 45,
p90 62)** on the 680mm Degé folio (≈578mm text width → ≈10.7mm
per syllable). This is the calibration target for the Degé
preset's type sizing: a healthy line should carry ≈54 syllables;
under ~45 the type is too large, over ~62 too small. (The
measured-woodblock preset's source volume is Sungbum — not in the
eKangyur — so its budget awaits the OCR-statistics lane.)

## Still open in pecha v3
Type-size autofit to the 54-syllable target; dbu-khyud/double-shad
conventions; the eyeball QA pass against prints.
