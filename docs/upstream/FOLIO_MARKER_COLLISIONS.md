# Upstream data defect: folio markers spliced into caption numbers

*Found 2026-08-28 during the C03 alignment campaign. **This is a report for
the HGM data project, not a change to be made here.** The spine is consumed,
never edited, by this repo. Repairing it downstream would put the app's text
out of agreement with the release it claims to import.*

## The defect

A folio marker letter has been spliced into the middle of a caption number in
the Wylie text.

    wylie says     1a0/        should be     10/
    wylie says     2a9/        should be     29/
    wylie says     19a67       should be     1967

The ACIP folio convention writes a page as `<number><side>`, e.g. `5a`, `12b`.
When a segment's own text begins with a number, the marker and the number
collide and the ingest keeps them fused.

## Extent

**119 occurrences across four courses**, listed with their repairs in
`folio_marker_collisions.tsv` (course, seq, found, repair, status).

| course | n | what it is |
|---|---|---|
| C17 | 47 | numbered list captions |
| C07 | 46 | the root downfalls, numbered `(10)`–`(18)` and up |
| C03 | 25 | the elephant-path chart captions, `[10]`–`[33]` |
| C10 | 1 | one caption |

## The repair is mechanically determinable, and provable

Delete the interior `a` or `b`; concatenate the digits. What makes this safe
rather than a guess: **Geshe Michael's English states the number**, in
brackets for C03 and parentheses for C07/C17. So every repair can be checked
against an independent witness in the same record.

    repair confirmed by the number in GMR's English : 115 of 119 (96.6%)
    contradicted                                    : 1
    unverifiable (no number in that English)        : 3

**The one contradiction is not a counterexample — it is a second finding.**
At C03:187 the Wylie reads `2a1/`, repairing to 21, and it sits between
segments numbered 20 and 22. Geshe Michael's English reads `[17]`. The
Tibetan is right and **his bracket number is a typo**. Under the project's
verbatim rule his text stays exactly as it is; this note is the record that
the discrepancy was examined rather than missed.

The three unverifiable cases (C03:185, C10:61, C17:557) are segments whose
English carries no number at all. Their repairs follow the sequence
unambiguously — C03:185 sits between `[18]` and `[20]` — but they have no
independent witness, so they are marked UNVERIFIED rather than confirmed.

## Related artefacts found in the same pass

- **`shogces pa'i`** (C03:166) — illegal Tibetan syllabification; `shog` and
  `ces` have run together. Almost certainly the tail of a quoted title,
  `...ces pa'i`. Not a folio collision, but the same ingest.
- **`[33}`** (C03:199) — a closing curly brace where a bracket belongs.
  Geshe Michael's, and stays verbatim.

## Recommended action

1. Repair the 115 confirmed collisions in the corpus builder, upstream, where
   the folio markers are parsed. The TSV gives every one with its repair.
2. Treat the 3 unverified ones as a separate, human-reviewed batch.
3. Leave C03:187's English alone and record the discrepancy in the release
   notes rather than in the text.
4. Add an ingest check: a token matching `\d+[ab]\d+` in Wylie is never
   legitimate and should fail the build.

Until then the alignment campaign leaves these tokens **unspanned and
unparsed**, flagged in the page notes. Guessing at them would violate the
never-guess rule, and repairing them locally would fork the data.
