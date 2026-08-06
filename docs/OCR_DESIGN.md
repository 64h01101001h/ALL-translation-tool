# Tibetan OCR for Pecha Scans — design note
*(banked 2026-08-06; Adam's request: eventual OCR for pecha scans sourced
from BDRC, https://www.bdrc.io/)*

## The landscape (verified 2026-08-06)
The problem is largely solved upstream, by the right people:

- **BDRC's own open-source OCR app** (`buda-base/tibetan-ocr-app`, released
  March 2025, developed by Eric Werner with Élie Roux and the Monlam AI
  team): free, offline, desktop (macOS + Windows), batch OCR over images
  and PDFs. **Five models by script style — Betsug, Drutsa, Modern,
  Woodblock, Woodblock-Stacks** — plus optional dewarping. Exports plain
  text or PageXML, and can emit **Wylie** directly.
- Models grew out of the BDRC OCR Project with OpenPecha/Monlam AI
  annotation; this is the current state of the art for pecha.
- BDRC's library (library.bdrc.io) serves the scans themselves (IIIF).
- Apple's Vision framework does NOT support Tibetan; Tesseract's `bod`
  model is far weaker on pecha. Neither is the right base.

## Integration plan (three stages)

**Stage 0 — works today (no code):** user scans/downloads pecha images from
BDRC, runs BDRC's OCR app, exports text/Wylie, and imports the result into
our Library ("Import my materials…"). Document this in the user guide.

**Stage 1 — hand-off integration (small):** a "Send to OCR…" action in the
Library pane for image/PDF selections that launches the installed BDRC app
(or points the user at its download page if absent), plus a watched
`library/ocr_out/` folder whose text outputs auto-import with an
`ocr-derived` tag.

**Stage 2 — embedded pipeline (the real feature):** run BDRC's released
models in-process (they ship as ONNX; ONNX Runtime links cleanly into our
C++ core): line detection → per-line recognition → Tibetan Unicode →
unicode→wylie→ACIP through our proven converters. A Scan pane shows the
page image with recognized lines side by side; low-confidence lines and
spellcheck-flagged syllables get the review treatment. **License check on
the models/app repo is step one of this stage.**

## Honesty rules for OCR (non-negotiable, inherited)
- OCR output is ALWAYS tagged `ocr-derived` with model + confidence; it
  renders visually distinct until a human verifies it.
- OCR text never enters the corpus or dictionary without verification —
  the same discipline as ACIP's own dual-entry/triple-comparison procedure
  (Release IV pp. 165–167), which is the house standard for a reason.
- Uncertain lines are flagged, never silently smoothed. Our spellcheck +
  syllable-legality layer runs over all OCR output as a first-pass QC.

## Sources
- BDRC OCR app: https://github.com/buda-base/tibetan-ocr-app ·
  https://buda-base.github.io/tibetan-ocr-app/
- Announcement: https://www.bdrc.io/blog/2025/03/14/bdrc-announces-the-release-of-ocr-app-for-tibetan/
- OCR project background: https://www.bdrc.io/blog/2024/08/28/transforming-tibetan-text-digitization-bdrcs-groundbreaking-ocr-project/
