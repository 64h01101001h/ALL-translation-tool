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

## Still open in pecha v3
Per-side syllable budget calibration (needs OCR line-length
statistics against these same scans), dbu-khyud/double-shad
conventions, and the eyeball QA pass — one honest step at a time.
