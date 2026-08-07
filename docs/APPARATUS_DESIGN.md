# Apparatus: footnotes, bibliography, quotation detection, scan follow-along
*(banked 2026-08-06 from Adam's directives; STD-008 program)*

## 1. The two-tier apparatus (SHIPPED 2026-08-06)

Adam's ruling, refining GMR's reuse directive: **bibliographical data and
footnotes enter the official documents only once published by our
publisher and 100% approved by Geshe Michael.**

This maps exactly onto the project's existing tier discipline
(curated > glossary > provisional in the dictionary):

| Tier | Contents | Where | Who feeds it |
|---|---|---|---|
| **OFFICIAL** | Apparatus from published, GMR-approved volumes only | `docs/apparatus/MAIN_FOOTNOTES.md` + `MAIN_BIBLIOGRAPHY.md` (committed, versioned) + the extracted JSON banks the app searches | `tools/extract_mixed_nuts_notes.py`, `tools/extract_mixed_nuts_bibliography.py`, then `tools/build_main_apparatus.py` — rerun per release; never hand-edited, never fed from drafts |
| **PENDING** | Candidates translators create while working | `data/candidate_notes.json` (local, gitignored) | Draft pane "Propose footnote (pending GMR approval)" — select `lemma: text` in the draft |

Pending candidates surface in the same apparatus search but are visually
fenced (⚠ PENDING, amber border) and insert with an explicit
"PENDING, not GMR-approved" label. Promotion to official happens only one
way: the note is published in a released volume, and the extractors pick
it up on the next run. There is no in-app "approve" button by design —
approval is GMR's act in the publishing process, not a UI state.

Current official bank: **344 footnotes + 138 bibliography entries** from
the three released volumes (SSL/OSE/SVN). 118 bibliography entries carry
ACIP catalog numbers.

## 2. Quotation detection → auto-recommended citations (TO BUILD)

Adam: *"recognize whenever certain works are being quoted so that we
could auto recommend citation and also bibliography entries."*

Fully deterministic — no guessing involved at any step:

1. **Detection**: the corpus holds 37,807 aligned segments, each tagged
   with its source work. A passage in the text being translated (or an
   English passage in the draft) that string-matches a corpus segment
   (exact / normalized-whitespace exact over a minimum span, e.g. ≥ one
   full line or ≥ N syllables) IS that work being quoted — attested, not
   inferred. FTS5 phrase search on the spine makes this fast.
2. **Citation recommendation**: the matched segment's source id → work.
   If the work's ACIP catalog number appears in the bibliography bank's
   `acip_refs`, the app recommends the ready-made published entry
   verbatim (official tier). Otherwise it reports "quotation detected
   from ‹work›; no published bibliography entry yet" — honest gap, and a
   natural candidate for the pending queue.
3. **Footnote recommendation**: same trigger can surface official notes
   whose lemma matches terms in the quoted span.

UI sketch: "Detect quotations" action in the Overlay/Draft panes; matches
listed with source, folio refs where known, [insert citation] and
[insert bibliography entry] links. Below-threshold partial overlaps are
NOT shown as quotations (rule 3: no guessing) — at most a separate,
clearly-labeled "similar corpus passages" list, which is just the
existing concordance.

## 3. Scan follow-along: folio-linked woodblock images (TO BUILD)

Adam: when a user starts working with a text, recommend they pull the
associated scans; link OUR page numbers to the scan pages so the exact
folio being worked on shows alongside — down to following the line/word.

Deterministic backbone we already have:
- ACIP input files carry `@NNNA/@NNNB` folio markers (already parsed by
  `exportTibetanUnicode` and `formatForTranslation`).
- `decodeAcipFilename` + `bdrcScanUrl` give the BDRC work (KD→MW22084,
  KL→MW26071, TD→MW23703, Tohoku-numbered).
- BDRC serves per-page images over IIIF (`iiif.bdrc.io`), addressable by
  image index within a volume.

Design:
1. **Offer, don't push**: on opening a recognized K/T file in the
   Overlay pane, show "Scans of this text are on BDRC — open / follow
   along" (we already show the scan link; this extends it).
2. **Folio → image mapping**: scan volumes start with front matter, so
   image index = folio-side index + per-volume offset. The offset is NOT
   guessable → the user calibrates once per text ("this image = folio
   1a"), stored with the file; every other folio then maps exactly
   (@012B = images offset + 2·12 − 1 + 1 side arithmetic). Calibration is
   deterministic user input, not a heuristic.
3. **Follow-along pane**: as the cursor moves through the ACIP text, the
   current `@folio` marker selects the scan image (fetched via IIIF,
   cached locally). Line-level following: ACIP files preserve the
   woodblock's line structure per folio side (typically 7 lines) — the
   line number within the folio's text highlights a horizontal band on
   the image. Word-level: only after OCR with coordinates (BDRC
   tibetan-ocr-app outputs line polygons) — stage 2, optional.
4. **Respect the transliteration boundary**: any BDRC query goes through
   acipToEwts first (established rule).

Dependencies: network fetch (IIIF) in-app — first network feature; cache
under library dir; offline = link-out only. OCR tie-in per OCR_DESIGN.md
(user-triggered, models' license question outstanding).
