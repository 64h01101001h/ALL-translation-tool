# Hackett's *A Tibetan Verb Lexicon* — what was mined (2026-08-09)

Adam supplied the scan (111 pp, no text layer) the weekend before
release. Mined so far:

## 1. Honorific verbs → the approval queue

The lexicon explicitly marks honorific readings "(hon.)". A full OCR
sweep (tesseract, English layer) located every marker; each hit page
was then read by eye for the Tibetan. **Eight explicitly-marked
honorific verbs** (`data/hackett/honorific_verbs.tsv`), of which two
(dgongs, gshegs) were already in the register — the **six new ones are
seeded as pending proposals** in the approval queue, cited to Hackett
with book page:

ljags klog (read) · dgyes (be pleased) · thugs mnyes (be pleased) ·
bka' stsal (say/speak) · zhu 'tshal (please do X) · zhal gyis bzhes
(assert/accept)

Plus four strong candidates the lexicon shows without the explicit
mark (bzhed, mjal, bzhengs, zhu) — listed in the TSV for Adam/GMR to
promote by hand if wanted.

## 2. Wilson-class causative pairs (Table 2)

`data/hackett/wilson_class_pairs.tsv` — 31 tha-mi-dad/tha-dad verb
pairs with Wilson class assignments (III↔V alternations: 'grub/sgrub,
'byung/'byin, …), transcribed from Table 2 (p. 7; Hackett after Das).
Feeds the verb-class machinery when we next extend spotVerb.

## 3. Raw OCR banked for future mining

`data/hackett/ocr_text/` — the full 111-page English-layer OCR. The
Tibetan is garbled (no Tibetan traineddata), but the English glosses,
verb classes (roman numerals), Sanskrit attestations, and case-frame
codes are searchable. Future mining possible: per-verb class + tense
extraction (needs page-by-page reading or Tibetan OCR), the case-frame
codes for richer parse guidance.

## Licensing

The lexicon is a copyrighted published work (Snow Lion). What we take:
**facts with citation** (which verbs are honorific, class assignments,
attested pairs) — the same footing as the Hopkins-tradition reference
material already in the dictionary, of which Hackett's lexicon is a
part. The banked OCR text is an in-house research aid only. **Never
ship lexicon content verbatim in the app**; register rows that reach
the app via approval carry only the fact + citation.
