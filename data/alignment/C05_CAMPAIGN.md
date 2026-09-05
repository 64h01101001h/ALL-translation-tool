# C05 CAMPAIGN — ACI Course 5 (How Karma Works), full-depth alignment

Opened 2026-09-05, immediately after C04 closed (198/198, 2026-09-04),
under Adam's standing order to scan C05 and then C06. Briefs are the
course-parametrised set in `docs/alignment_briefs/`; the task names the course.

Standing rules (Adam): GMR's English stays VERBATIM, typos flagged never
corrected; nothing binding without human review; every batch records its
producing model (propose: Claude Opus 5 · reconcile/refute: Claude Fable
5.1); completeness is proved by the builder's COVERAGE GATE, never asserted
in prose; a reconciler's deliverable is a spec FILE that exited 0.

## Course shape (spine scan, 511 segments)
- **1–21: the liturgical opening, BYTE-IDENTICAL to C03:1–21** (and so to
  C04:1–21) in both columns; 21/21 verified by SQL join at open. As in C04,
  c5p1–c5p7 are the landed c3p1–c3p7 copied verbatim with course labels
  changed and a CLONED PAGE banner (the phonetics-protocol pages cannot be
  regenerated, see C04 ledger); every sentence's plain text was checked
  against the C05 rows at copy time (21/21 match) and the layer gates
  re-derive every span against the C05 rows on each run.
- **22 and 23: corpus markers.** 22: wylie = the English running head,
  english = the Course V syllabus (531 chars, complete). 23: wylie = FOLIO
  REFERENCES ("127a, 147b-148b, 174a-174b"), english = the Reading Six
  heading and reading list, cut at the 1,500 cap (E-107). Hand-built
  marker blocks per the E-071 protocol, unpaired clause, no pairs.
- **24–511: Abhidharma.** Vasubandhu's *Treasure House of Knowledge*
  (chos mngon pa mdzod, Abhidharmakosha) root verses with the First Dalai
  Lama Gendun Drup's commentary *Illumination of the Path to Freedom*
  (mdzod Tik thar lam gsal byed), chapter IV (karma), plus Changkya's
  *Schools of Philosophy* (24–26) and Chandrakirti/Kedrup on emptiness and
  karma (Reading Six). 156 rows carry double-shad verse lineation `, ,`
  (root verses quoted inside the commentary, often prefixed with a bracketed
  verse number like `[iv.179-80]` — apparatus, not Tibetan); GMR renders the
  verses as capitalised English lines. Row headings ("Reading Two Subject: …")
  fused onto the English of rows 96, 118, 145, 206, 241, 314, 377, 411, 467
  (registered following-heading class — do not file). Row 30–31 carry the
  Sanskrit title in Tibetanised ACIP (`aa bhi dharma ko Sha kA ri kA`).
  Row 219's English is cut at the 1,500 cap (E-107; note, do not file).
  Median wylie length 222 chars, max 756. Parallel witnesses: C16 shares
  263 byte-identical rows (same file re-ingested, NOT independent); C15 54,
  C18 13, C09 9, C17 6.

## Gates
The seventeen standing gates plus `no_supplied_span_head`, which for
`pages_c05` refuses any d=5/d=7 English span beginning with
the/a/an/and/or/his/our/your/I/you unless `data/alignment/span_head_allow.json`
lists it under `pages_c05` with the Tibetan word that licenses it.

## Progress
c5p1–c5p7 (1–21) cloned 2026-09-05; c5p8–c5p15 (22–45) landed 2026-09-05. Coverage 45/511.

## Queues (wylie / english), C05
(opened empty)

### Batches C05-8 … C05-15 (C05:22–45) — 2026-09-05 — propose: Opus 5 · reconcile/refute: Fable 5.1

First proposed batches of the course: 22 segments (22 and 23 are hand-built
markers), 44 proposals, 44/44 exited 0; 22 reconciled specs, 22/22 exited 0
first run. Pages c5p8–c5p15 wrapped and registered; coverage 45/511; layer
6,484 headwords / 13,661 pairs; 17 gates green; `no_supplied_span_head`
reports **0** function-word-initial d5/d7 spans in pages_c05.

**Course shape confirmed on the ground.** C16 rows are byte-identical
re-ingests of most C05 prose (C16:677–697 ↔ C05:25–48), so no C16 match is
an independent witness. Row 29 carries the outline vocabulary ("an
explanation of" ×4) that GMR supplied; the reconciler nulled `don` ×3 and
`'chad pa` rather than bank "explanation" on `don` (Rule 10). Row 30's
phonetic title ("Chu Ngunpay Dzu Kyi Tsikleur Jepa") was NOT banked at d=5
even though the capitalisation exemption would let it pass: GMR supplies the
real English in the same sentence, so a phonetic d=5 link would enter the
bank as a false equivalent; the bracketed translation was linked instead.
Row 36's closing clause ("here first comes the obeisance of the translator.
*********") has no Tibetan under it (the wylie ends at `'chad do`) — left
unwrapped, published reading per C16:685.

**Rule 10 rulings (banked with notes).** 24 `gsung` → "written" (not
"written by"); 28/27 agentive `kyis`/`gyis` → ∅ (the "by" belongs to the
passive frame); 31 `zhes pa'o` → ∅ (not "translates as"), `ni` → ∅; 32
`skad` → ∅ (not "name"); 34 `tshong` → ∅ (trade, not "merchants"; both
analysts had banked it); 35 `'di tsam` split flat, `thad log` → ∅; 36 the
"Why" questions → ∅ (GMR recast purpose-statements as questions); 39 `gang
gis` → ∅ (scaffolding); 41 `rgyu` → ∅ / `byung` → ∅ (converses produced by
GMR's restructuring, both analysts had banked them); 44 `nas` → ∅; 45 `bcas`
→ ∅ (comitative; GMR's "and" rests on `dang` at 47); 46 `bshad` → ∅ (not
"promised"). 47 `chos can` → "Consider" per the C04 convention.

**Errata.** 24 `mtshung med` (ACIP MTSUNG MED) for `mtshungs med` —
TIBETAN_SPELLING, LOW/PROBABLE, no witness anywhere in 42,199 rows, skeptic
passed. 39 `bnas drug pa` for `gnas drug pa` — illegal syllable, skeptic
passed, confidence raised to CERTAIN. 38 lower-case sentence start after a
closing quotation (`stain." the first two`) — first skeptic REFUTED the
record because its evidence called C16:687 byte-identical when C16:687 in
fact reads "The" (capital); evidence corrected to name C16:687 as a
DIFFERING witness and re-sent to a second skeptic (pending at this entry).
31 `ko Sha`/`ko ShA` mixed vowel length within one row — dropped by the
reconciler (cannot be shown to be a keying defect rather than the
blockprint's own variation); WYLIE queue candidate.

**Shape notes.** 37 banked `gnas gang na` → "in each of the eight" at d=3
with no members (phrase-level, outside the dictionary harvest) because "the
eight" substitutes for `gnas` rather than translating it. 51's English-first
analyst had to abandon the flat shape ("formless" ×2) — reconciler to rule.
