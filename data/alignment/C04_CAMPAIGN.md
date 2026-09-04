# C04 CAMPAIGN — ACI Course 4 (The Proof of Future Lives), full-depth alignment

Opened 2026-09-04, after the C03-stratified correctness audit
(`docs/AUDIT_2026-09-04_C03_STRATIFIED.md`, 19/100 confirmed defects) and
its two preconditions: the rule-7 hard check in RECONCILE_BRIEF and the
gate `no_supplied_span_head` (zero-or-allowlisted for this directory).
Briefs are course-parametrised from today; the task names the course.

Standing rules (Adam): GMR's English stays VERBATIM, typos flagged never
corrected; nothing binding without human review; every batch records its
producing model (propose: Claude Opus 5 · reconcile/refute: Claude Fable
5.1); completeness is proved by the builder's COVERAGE GATE, never asserted
in prose; a reconciler's deliverable is a spec FILE that exited 0.

## Course shape (spine scan, 198 segments)
- **1–21: the liturgical opening, BYTE-IDENTICAL to C03:1–21** in both the
  wylie and the english columns (mandala offering, refuge and the Wish,
  dedication, the Buddhist grace; english = GMR phonetics, with the English
  stanza fused onto rows 6, 11, 16, 21). Not re-proposed: c4p1–c4p7 are
  the landed c3p1–c3p7 copied verbatim with course labels changed and a
  CLONED PAGE banner. A regeneration-based clone was tried first and
  refused by the generator: the C03 phonetics pages carry d=7 links with
  no d=5 parent (the phonetics protocol), a shape the generator's later
  d=7-parent rule no longer accepts, so those pages cannot be regenerated
  by anyone. Identity proof: every sentence's plain text was checked
  against the C04 rows at copy time (21/21), and the layer gates re-derive
  every span against the C04 rows on each run. Citations become C04:n;
  the pairs are the same pairs.
- **22 and 28: corpus markers** (wylie = an English running head; english =
  the Course IV syllabus / the English introduction to Jewel of the True
  Thought, both cut at the 1,500-char cap, E-107). Hand-built marker
  blocks per the E-071 protocol, unpaired clause, no dictionary pairs.
- **23–198: prose**, Buddhist logic. Gyaltsab Je's *Light on the Path to
  Freedom* (rgyal tshab dar ma rin chen), Geshe Yeshe Wangchuk's *Jewel of
  the True Thought* (ser smad spom ra dge bshes ye shes dbang phyug), Kedrup
  Tenpa Dargye's *Tik chen*; debate format — `chos can` (subject), `thal`
  (it follows), `phyir` (because), `ma grub na` (if not established), `skyon
  med de` (no fault); GMR renders the syllogism frame with a fixed English
  register ("It follows that…", "because…", "Suppose you say…"). Row
  headings like "Reading Two Subject: …" are fused onto the English of rows
  27, 29, 37, 49, 62, 78, 86, 93, 117, 133, 168 (the following-heading
  class, registered — do not file). Median wylie length 180 chars, max 560.
- Verse citations from Dharmakirti's *Commentary on Valid Perception* in
  rows 137–145, 182, 186, 188, 194, 197 (double-shad lineation `, ,`).

## Gates
The seventeen standing gates plus `no_supplied_span_head`, which for this
directory refuses any d=5/d=7 English span beginning with
the/a/an/and/or/his/our/your/I/you unless `data/alignment/span_head_allow.json`
lists it under `pages_c04` with the Tibetan word that licenses it. The
reconciler names the licensor or trims the span; there is no third option.

## Progress
c4p1–c4p11 (1–33) landed; 165 segments remain (34–198).

## Queues (wylie / english), C04
(opened empty)

### Batches C04-8 … C04-11 (C04:22–33) — 2026-09-04 — propose: Opus 5 · reconcile/refute: Fable 5.1

First proposed batch of the course: 10 segments (22 and 28 are hand-built
markers), 20 proposals, 20/20 exited 0 on re-run; 10 reconciled specs,
10/10 exited 0 first run. Pages c4p8–c4p11 wrapped and registered;
coverage 33/198; layer 5,963 headwords / 12,132 pairs; 16 gates green and
`no_supplied_span_head` reports **0** function-word-initial d5/d7 spans in
pages_c04 — the rule-7 hard check held on the first batch under it.

**Course shape confirmed on the ground.** C16:516–522 are byte-identical
parallel witnesses of C04:26, 27, 33, 34, 35, 36 (same file re-ingested,
so not independent). C04:29 is an **UNPAIRED ROW**: its English column is a
page number plus the running head ("13 The Asian Classics Institute Course
IV … Reading 2B: What it Was that Master Dignaga Stated") while the wylie
carries the two inference definitions; both analysts and the reconciler
banked nulls only (69 spans, zero pairs). Six Tibetan-side errata filed
from that row (fused `rjes dpggrags`, `sbyis` for `sbyin`, missing `mi`,
`tsham` for `tsam`, `rtag` for `rtags`) — ACIP column carries the same
readings, so they are source-of-record keyings; sent to the refute pass
before anything is registered.

**Rule 10 rulings.** 23 `ma dmigs pa` → ∅ (both analysts had
"non-existence"; dmigs pa is observe, not exist); 24 `gsung` → "written"
(not "written by"; the "by" is the genitive's hinge); 26 `gnas 'di la`
split flat, `bya'o` → "should"; 27 `sbyong bar` → ∅ (train, not "remain"),
`skyon brjod kyi smra ba` → "speak badly of others" as one idiom; 30
`dgod pa` → ∅ (GMR's frame "What it Was that" has no single word under
it); 31 title compounds kept with d=7 members; 32 `tshad ma` → "correct
person" (the Pramanasiddhi sense, GMR's fixed rendering); 33 `rtsa ba'i`
→ "entire work" over "major". Reconciler 25 named licensors for three
one-word spans (`nga` → "I" ×2, `'am` → "or"); these are whole-span
pairings, not span heads, so the gate does not list them.

**Errata 24.** Both analysts flagged "Light **for** the Path to Freedom"
against "Light **on**" elsewhere; the reconciler dropped it (a title
variant in GMR's own prose, no witness says which is canonical) — noted
here, not filed.
