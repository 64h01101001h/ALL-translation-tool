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
spellcheck-flagged syllables get the review treatment.

**License status — RESOLVED (2026-08-07):** Adam reports BDRC has
given the okay for our OCR usage (models included). Action: save the
written confirmation into `docs/licenses/BDRC_OCR_permission.*` when
available — the project records permissions the way it records
standards: the actual words, who, when. Stage 2 is now unblocked.

**Stage 2 spike (2026-08-07, same day as permission): FIRST INFERENCE
RAN.** BDRC/PhotiLines (line detection, 90MB ONNX, CC BY-NC 4.0 on the
HF card) downloaded to `library/ocr_models/BDRC_PhotiLines/` and run
via onnxruntime (python, as the pipeline probe) on our cached
Pramanavarttika folio 94a: input NCHW RGB, 512-patch tiling per the
app's own preprocessing (BDRC/Inference.py), logits → sigmoid → mask;
the model traces the title line's glyphs even on a 676px microfilm
title page (the hardest case — sparse ornate folio). Model inventory
on HF (author=BDRC): Photi/PhotiLines (layout/lines), Woodblock,
DergeTenjur, LhasaKanjur, LithangKanjur, GoogleBooks_{E,C,T},
Norbuketaka_{E,C}, ScriptClassifier.

**Stage 2 increment A SHIPPED (2026-08-07, Adam: "start integrating the
OCR functionality"): LINE DETECTION IN C++.** New `allocr` target
(`ocr/`, links brew onnxruntime; skipped gracefully when absent; ML
never enters allcore). `allocr::LineDetector` ports the canonical chain
verbatim — clamp-resize, pad-255 to patch multiples, 512-tiling,
PER-TILE adaptive-Gaussian binarize (cv2 semantics: fixed-point luma,
ksize-51 sigma-8 replicate-border kernel, mean−C compare), normalize,
NCHW, ONNX, sigmoid, threshold 0.9, stitch, crop, resize-to-original —
plus row-band line extraction. Battery `ocr_smoke` (30th suite) diffs
against the CANONICAL python pipeline run on two banked public-domain
Pramanavarttika fixtures (tools/build_ocr_reference.py; PySide6 shimmed
— the oracle needs no Qt): dense folio 99a = 98.6% pixel agreement with
IDENTICAL 8 line bands; sparse title 94a = 99.9%, 3 bands; residue is
cv2 fixed-point vs float rounding at glyph edges.

**Increment B plan (settled 2026-08-07, GATED ON DISK SPACE):** the
line-BUILDING stage (mask → rotation angle → contours → Line bboxes →
reading-order sort → per-line images, canonical line_detection.py) is
wall-to-wall OpenCV: findContours/minAreaRect/approxPolyDP/warpAffine/
dilate/drawContours. Decision: allocr will LINK OpenCV C++ — the same
native code the canonical cv2 calls, so the geometry stage is
bit-identical by construction rather than approximated by hand-rolled
equivalents (the increment-A tolerance batteries then tighten to exact
for this stage). BLOCKED 2026-08-07: brew opencv (~400MB w/ deps)
cannot be responsibly installed with the data volume at 100% (892MB
free); likewise the CTC recognition models (BDRC/Woodblock etc.,
100-300MB each). Both resume the moment Adam clears disk space. The
CTC increment then follows: OCRModel port (pad_ocr_line/_prepare/
_pre_pad/greedy CTC decode with each model's charset from
model_config.json), oracle = canonical pipeline text output per line,
then Unicode via OUR proven chain + syllable-legality QC.

**Stage 2 build plan (remaining):**
1. Dependency: onnxruntime (brew; C++ API) — linked by a NEW `allocr`
   target, never allcore (ML stays out of the deterministic core).
2. Model manager: first-run download from HF into
   `library/ocr_models/<repo>/` with license + BDRC credit shown
   (permission on record: docs/licenses/BDRC_OCR_permission.md).
3. Pipeline port from BDRC/Inference.py (MIT): patch-512 tiling →
   LineDetection mask → line extraction (contour/row bands) → CTC
   recognition (charset from each model's config) → Tibetan Unicode →
   our proven unicode→wylie/ACIP chain + syllable-legality QC pass.
4. Oracle: the BDRC app itself runs locally (python) — capture its
   text output on fixture folios; the C++ port diffs against it
   (same discipline as every other port).
5. UI (after the app-file lane is free): Scan pane — page image,
   detected lines, per-line recognition w/ confidence, everything
   marked ocr-derived; feeds the existing ocr_out review flow; also
   unlocks word-level scan follow-along coordinates.

**Original license check (2026-08-06):**
- The app/pipeline CODE is **MIT-licensed** — we may incorporate, modify,
  and redistribute it inside our application freely, preserving the license
  text and attribution. Fully compatible with our codebase.
- The MODELS are distributed "open access" via BDRC/OpenPecha HuggingFace,
  but the repo page states no explicit model license. Two clean paths:
  (a) the app downloads models from BDRC's own release at first use
  (user-initiated fetch — no redistribution question at all), or
  (b) confirm bundling rights with BDRC directly — an easy ask given that
  the Asian Legacy Library is itself credited among the training-data
  sources. Verify the HuggingFace model cards either way before bundling.

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
