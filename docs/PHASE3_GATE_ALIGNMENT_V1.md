# Phase 3 Formal Gate: Alignment Layer v1

*Drafted 2026-09-01 for Adam to ratify. Every number below was recomputed
from the repository on that date, or is cited to the commit or file that
carries it. Where a claim could not be verified it says "unverified".
Counts moved while this was written: commit 900d1b6 (2026-09-01 16:42)
landed C03:415-417 mid-draft. Figures are as of that commit.*

## 1. Gate question and scope

**Checkpoint question (TODO plan item):** is alignment layer v1 done and
validated, and should C03:415-620 continue in this Phase 4 loop or open a
new epic?

**What "alignment layer v1" denotes** (the artefacts as they exist):

| artefact | path | live state (2026-09-01) |
|---|---|---|
| alignment pages, seven Wilson depths, both sides wrapped in place from the spine | `data/alignment/pages/` (C02), `pages_c01/`, `pages_c03/` | 23 + 167 + 132 HTML files; 166/22/132 registered in `COURSES` |
| evidence sidecar (depth-5 word pairs, ACIP-keyed, cited) | `data/alignment/alignment_evidence_v1.json` | 5,248 headwords, 10,025 pairs, `acip_index` 5,247 |
| full bank (every span at every depth, notes, trees) | `data/alignment/alignment_full_v1.json` | 25,464 links (d1 983 / d2 1,940 / d3 581 / d4 378 / d5 14,490 / d6 4,779 / d7 2,313), 319 notes |
| dictionary view (depth slider, PROVISIONAL banner from layer meta) | `docs/michael_dictionary.html`, built by `tools/build_dictionary_view.py` | regenerated each batch (commit 15f5127) |
| errata register (generated from sidecar) | `docs/ERRATA_REGISTER.md` from `docs/errata_register.json` | 101 entries: 60 document errata (5 HIGH, 40 MEDIUM, 15 LOW), 17 digitisation artefacts, 14 not-errata, 9 layer artefacts, 1 unverifiable |
| campaign ledgers | `data/alignment/C01_CAMPAIGN.md`, `C02_CAMPAIGN.md`, `C03_CAMPAIGN.md` | C03 ledger's last entry is 406-411 (408/620); commits are at 417/620 |

**Courses covered:** C01 496/496 and C02 68/68, both `complete: True` and
gate-proved (`tools/build_alignment_layer.py` COURSES; spine counts 496/68
confirmed by query). C03 417/620, `complete: False`. C04-C18: zero rows.
C13 under standing order not to be scanned (`docs/upstream/C13_COLUMN_OFFSET.md`).

**Tier, in the layer's own words** (`alignment_evidence_v1.json` meta):
"TENTATIVE (machine-matched, unreviewed)"; "these gates prove the text is
VERBATIM, never that the correspondence is CORRECT; a wrong pairing passes
every one of them (risk R10)".

## 2. Evidence inventory

Legend: **R** = mechanically re-runnable (`cd build && ctest -R <name>`);
**O** = one-off, result banked in a document or commit.

### 2a. Verbatim / soundness evidence (fidelity to the source)

All 14 suites below were run on 2026-09-01: 14/14 passed, 6.6 s. Commit
messages count "nine" (ledger, 408/620), "ten" (fc22386), "fifteen"
(900d1b6) "layer gates"; the set is not fixed in any document. This is the
set registered in `core/CMakeLists.txt` lines 533-627.

| claim | where proven | kind |
|---|---|---|
| every wrapped Tibetan/English fragment is a literal substring of the spine segment it cites; build refuses otherwise | `alignment_builder` (#78), builder docstring | R |
| pages are generated from spine text, spans found in order by a forward cursor; refusal on miss | `gen_alignment_page` (#77) | R |
| span-level ACIP recovered positionally and round-trips through `acip_to_ewts`; one known miss pinned | `acip_recovery` (#76); TODO.md 2026-08-28 (4,038/4,039) | R |
| no span cuts a word or a syllable; no depth-7 member is degenerate | `no_broken_words` (#79), `no_split_syllables` (#80), `no_degenerate_members` (#82) | R |
| phonetics (d=7 sound-spelling pairs) never enter the evidence layer | `no_phonetics_in_layer` (#81); C03 PHONETICS PROTOCOL | R |
| every banked field still occurs in the cited segment (NOT-IN-CORPUS hard zero) | `layer_matches_spine` (#83), commit 15d64de: 42,587/42,648 exact, 0 fabrications | R |
| evidence layer English, wylie and ACIP all occur at every cited segment | `evidence_matches_spine` (#84), commit 5cb9050 | R |
| the published dictionary view shows only what the layer banked (exact or prefix) | `view_matches_layer` (#86), commit 517ef51: 14,647 rows, 0 invented | R |
| the builder parses every span/note/tree the pages carry (closes the 2.7% silent drop) | `builder_sees_every_span` (#88), commits d6b3ac1, 4e8e984 | R |
| errata quotes still hold against the spine; entries with a parallel witness name it; register prose matches sidecar | `errata_quotes_hold` (#85), `errata_name_their_witnesses` (#89), `generated_docs_in_sync` (#87) | R |
| gates can fail: each suite mutation-verified | ADR-007; per-commit mutation notes (15d64de, 5cb9050, 517ef51, 4e8e984) | O, periodic (`tools/mutate.py`) |
| wylie column is a proven derivation of ACIP: 8,961/8,961 segments C01-C18 | TODO.md 2026-08-28 (DIFF-TEST RUN) | O |

### 2b. Coverage evidence

| claim | where proven | kind |
|---|---|---|
| a course marked complete has a page for every spine segment; the build refuses otherwise; incomplete courses print their gap on every run | `build_alignment_layer.py` lines 377-405 (COVERAGE GATE); liveness test recorded TODO.md 2026-08-28 (C01 falsely complete refuses naming all 25) | R (runs on every build) |
| C01 496/496, C02 68/68 registered, no duplicates | recomputed 2026-09-01 from `COURSES`; spine counts by query | R |
| C03 417/620 registered | same; commit 900d1b6 | R |
| no registered page is missing on disk | builder refuses (`lines 407-416`) | R |

### 2c. Correctness evidence (is the mapping right)

| claim | where proven | kind |
|---|---|---|
| 4.0% confirmed intra-segment defect rate, Wilson 95% CI [1.6%, 9.8%], n=100 unstratified triples, seed 20260828 | `docs/AUDIT_2026-08-29_LAYER_CORRECTNESS.md`; sample banked at `data/alignment/audit_sample_20260828.json`; commit 2dad4fb | O (sample reproducible; the audit itself is a model-read run) |
| single-auditor rate 9.0% [4.8%, 16.2%]; refute pass killed 5 of 9 flags | same, section 2 | O |
| earlier audit (60 high-risk mappings, 37.5% headline) was contaminated by hand-typed inputs and is withdrawn | `docs/AUDIT_2026-08-28_CONTAMINATED.md` | O |
| five mechanical correctness gates falsified with pre-registered criteria (110 fires; 43.5%; 2,322 fires; 1.9%; 0/2; 975 fires) | `docs/GATE_TRIALS.md`; TODO.md CLOSED 2026-08-28 | O, tools kept (`tools/detect_*.py`) |
| the manual `hgm_gloss` dictionary agrees with the scan on 41.5% exactly and 29.1% partly of 2,178 shared headwords; 641 disagree | `docs/TWO_DICTIONARIES_INTEGRATION.md` (measured 2026-08-28 at 4,039 headwords) | O, stale by ~1,200 headwords |

## 3. What the evidence does not show

1. **The 4.0% is a floor, not a rate.** Adversarial pressure was applied
   only to the 9 DEFECTIVE calls; 91 SOUND calls were never challenged by a
   second reader (audit section 4). The same auditor misread licensor
   distribution five times in one direction; the other direction is
   unmeasured.
2. **It is a C01 figure.** The population resolves 66.1% C01, 28.8% C03,
   5.1% C02; roughly 66 of 100 draws were C01. No per-course rate is
   interpretable, and C03's is the one the epic decision turns on.
3. **It counts occurrences, not headwords.** 1,072 records carry 3,418
   occurrences (27.7% of the population). `grub` shows five renderings and
   one is false: 20% of that card. The headword-level rate a translator
   experiences cannot be derived from this sample.
4. **It is blind to whole-segment mispairing.** Every check available to the
   auditors and to all 14 suites is internal to the segment. The C13:63-82
   column offset (twenty segments pairing Tibetan with a different text's
   English) passes every one; five detection approaches failed against that
   known positive (`docs/COLUMN_OFFSET_UNDETECTABLE.md`). "No offset
   detected" has never been demonstrated for C01, C02 or C03.
5. **Nothing has had the human review Ruling 2 requires.** Adam, 2026-08-28:
   "nothing is binding without human review... nothing enters the binding
   dictionary until a review pass confirms the mapping." The 100 audited
   triples were model-read. The 641-headword disagreement queue from
   `TWO_DICTIONARIES_INTEGRATION.md` has not been worked. Zero pairs in the
   layer carry a human-review mark; the schema has no field for one.
6. **C03 is 417/620.** The course is 67% scanned; the remaining 203 segments
   are the depth of the prose section. The layer meta says three courses,
   which is 3 of 18.
7. **The layer is PROVISIONAL/TENTATIVE by its own meta**, and the app
   renders it so. No artefact anywhere claims otherwise; this gate must not
   be the first to.
8. **The ledger is behind the commits.** `C03_CAMPAIGN.md` ends at 406-411
   (408/620); commits fc22386 and 900d1b6 landed 412-417. Ledger prose still
   says "Nine layer gates green".
9. **Model provenance is not per-batch in the ledger for the Fable pages.**
   TODO.md states all C01/C02/C03 work through C03 batch 32 was Fable; the
   C03 ledger carries 40 "model: Opus" headings and 0 "model: Fable"
   headings. The prose claim stands; per-batch backfill: unverified.
10. **The Fable re-audit C01-C18 (TODO item B) has not run.** The 2026-08-29
    audit is a 100-triple sample, not the full span-by-span pass Adam ordered.
11. **Errata are not a proofread.** `ERRATA_REGISTER.md` section 2: "An
    unflagged segment is not a clean segment; it is an unexamined one."
12. **Two-dictionaries figures are stale.** Measured at 4,039 headwords; the
    layer is at 5,248. Agreement rates at the current size: unverified.

## 4. Architecture and test-design retrospective

What Phase 3 would have produced up front, and where each was learned
instead. "Findable" = a new contributor reading CLAUDE.md, README.md and
the roadmap would reach it; none of those three files points at any
alignment document today (grep 2026-09-01).

| lesson | Phase 3 artefact it should have been | learned where | written down now | findable |
|---|---|---|---|---|
| Tibetan and English spans cross; one shared forward cursor refuses every real page; spec needs `eng_order` and two cursors | page-spec schema + generator design note | C03 batches 54-55, ledger line ~1020; first generator run refused 6/6 | `tools/gen_alignment_page.py` docstring ("TWO ORDERINGS"); pins 7-11 in `tools/test_gen_alignment_page.py` | only via the tool |
| a d=7 member needs a d=5 parent to nest inside; d=7 exempt from word-boundary rule, so containment alone is wrong | Wilson-depth nesting rules in the planning spec | c3p131 refused twice (fc22386); c3p118 prep-script break (ledger 368-379) | comment inside `COURSES` (`build_alignment_layer.py` lines 210-217); commit message | no: a comment in a data table |
| a parser that silently accepts less than it is given is invisible downstream; 1,260 spans (2.7%) across 141 pages dropped because `class="u"` was matched exactly | test-design principle: every gate must compare against the artefact upstream of the one it reads | d6b3ac1, found by an implausible 3-links-for-122-spans count | `builder_sees_every_span` + CMake comment lines 612-616 | via CMake only |
| surface signals cannot detect semantic defects; five gates falsified | Phase 3 test-design decision: model-read audit with sibling spans + refute pass is the correctness method; no mechanical correctness gate | `docs/GATE_TRIALS.md`, 2026-08-28 | GATE_TRIALS.md; TODO.md CLOSED block; R10 in RISK_REGISTER.md | TODO.md only |
| the refute pass is load-bearing: single-auditor rate 2.25x the defensible one; it refuted the author's own hand-checked finding | audit protocol in the test design | audit 2026-08-29 section 2; commit 2dad4fb | AUDIT_2026-08-29 doc; not a house skill | doc only |
| the transport schema stripped `eng_order`; three specs reported REFUSED were never refused | interface contract between reconciler output and generator input | 900d1b6 (found 2026-09-01, four days after) | commit message only | no |
| completeness cannot be asserted in prose; C01 declared complete with 25 unpaged segments | coverage requirement in the PRD | TODO.md 2026-08-28 | COVERAGE GATE code + comment; TODO.md | TODO.md |
| Wylie capitals are consonants; case-folding in the gate normaliser hid 16 wrong headwords | data-model note: Wylie is case-significant | ledger 406-411 | ledger prose; `bare()` case-preserving | no doc |
| the layer is attestation, not definition; multiplicity is the point (38.1% of renderings of well-attested keys share no content word) | product brief distinction | GATE_TRIALS Gate 5; PRESERVE_ALL_RENDERINGS.md; TWO_DICTIONARIES_INTEGRATION.md | those three docs | TODO.md |
| the verbatim rule and never-guess collide on an outright source error (C03:286) | rule set in the PRD | `docs/NEVER_BANK_A_FALSEHOOD.md` (Rule 10) | that doc | no pointer from CLAUDE.md |
| whole-segment mispairing (C13) is undetectable downstream; the check belongs at ingest | ingest-side requirement in the architecture | 019bd50, 32d13d3 | `docs/upstream/C13_COLUMN_OFFSET.md`, `COLUMN_OFFSET_UNDETECTABLE.md` | no |

Net: the technical content of a Phase 3 package exists, scattered across
one tool docstring, one data-table comment, five commit messages, five
docs and TODO.md. No single document states the page-spec schema, the
depth-nesting rules, the gate inventory, or the audit protocol.

## 5. Gate decision (recommendation, for Adam to ratify)

### 5a. Is v1 "done and validated"?

**Recommend: PASS WITH CONDITIONS**, on the following reading of the words.

- *Sound* (verbatim, cited, coverage-proved for C01 and C02): **yes**,
  mechanically, re-run 2026-09-01 (14/14).
- *Correct*: **measured, not established.** One 100-sample audit, C01-
  weighted, floor-biased, blind to segment-level mispairing.
- *Done*: **no for C03**; yes for C01 and C02 as scanned courses.
- *Validated* in Ruling 2's sense: **no.** Zero human review.

Conditions for the PASS to stand:

1. The gate document (this file) states, and Adam signs, that "v1 validated"
   means *verbatim and coverage-proved for C01+C02, correctness sampled at
   4.0% [1.6%, 9.8%] intra-segment, tier unchanged: PROVISIONAL*.
2. `C03_CAMPAIGN.md` is brought level with HEAD (412-417) before the gate
   closes; ledger and commits must agree on the segment count and gate count.
3. A single `docs/ALIGNMENT_LAYER_SPEC.md` (or equivalent) is written from
   section 4 above: page-spec schema incl. `eng_order`, depth-nesting rules,
   the 14-suite inventory with what each proves and cannot prove, the audit
   protocol (siblings + refute), Rule 10. CLAUDE.md gains one line pointing
   at it.
4. The per-batch model provenance for Fable-era pages is either backfilled
   into the ledger headings or the TODO.md claim is marked prose-only.

### 5b. Same epic or new epic?

**C03:415-620: close this epic at C01+C02+C03:1-417 and open a new epic
for C03:418-620.** Reasons: the current epic has no written acceptance
criteria and its scope grew from "C02 demo" to "main dictionary" by ruling
mid-run; the c3p128-132 failures (spend-limit losses, a schema that dropped
`eng_order`, a false "REFUSED" report) are integration defects a new epic
can start with fixed; and closing here gives a clean baseline for the
C03-only defect rate the next audit should stratify on. The work itself is
unchanged: same generator, same gates, same three-per-page cadence.

**C04-C18: a new epic per course, each gated on a Phase 3 spec, not Phase 4
momentum.** C13 stays blocked until the upstream offset is resolved and an
ingest-side row-count check exists. Before C04 opens: the full-scan
estimate should be stated (C01-C03 = 1,184 segments in five days across
~320 pages; 18 courses are 8,961 segments), and the C03-stratified audit
should run so that the first non-C01 correctness number exists.

### 5c. Before any part of the layer is called BINDING

Explicit preconditions, none met today:

1. A human review pass over the mapping, recorded per pair in the schema
   (reviewer, date, verdict), per Ruling 2 (2026-08-28). The 641-headword
   disagreement queue in `TWO_DICTIONARIES_INTEGRATION.md`, recomputed at
   current size, is the proposed first queue.
2. A stratified correctness audit with a C03 stratum, headword-level
   sampling, and a second reader on SOUND calls, so both directions of
   error are measured.
3. A segment-level integrity check at ingest (row counts per column; the
   parenthetical-anchor method of 019bd50) for every course before it is
   scanned, and a recorded result for C01-C03 retroactively.
4. The Fable re-audit of all scanned courses (TODO item B), with dispositions
   filed in the ledgers.
5. GMR's English still byte-exact (Ruling 1); the errata for his text
   (E-numbers in `ERRATA_REGISTER.md`) resolved by his ruling, not by edit.
6. Rule 10 (`NEVER_BANK_A_FALSEHOOD.md`) applied and each withheld span
   noted on its page.
7. Both dictionaries kept separate and ACIP-keyed, corroboration as a field
   not a merge (Adam 2026-08-28, "keep BOTH dictionaries"); no promotion of
   any scan pair into `hgm_gloss` (ADR-003).

## 6. Open items for the backlog

- Bring `data/alignment/C03_CAMPAIGN.md` level with commits fc22386 and 900d1b6 (412-417; gate count).
- Write `docs/ALIGNMENT_LAYER_SPEC.md` from section 4; add a pointer line in `CLAUDE.md` and `README.md`.
- Move the d=7-needs-d=5-parent rule out of the `COURSES` comment (`tools/build_alignment_layer.py` 210-217) into the generator docstring and pin it in `tools/test_gen_alignment_page.py`.
- Fix or document the reconciler-to-generator transport so `eng_order` survives (900d1b6); add a pin that a spec with `eng_order` round-trips.
- Re-run C03:418-422 (900d1b6 says in progress); confirm pages stay in triples.
- Recompute `docs/TWO_DICTIONARIES_INTEGRATION.md` at 5,248 headwords; publish the 641-queue successor as the first human-review queue.
- Run the C03-stratified, headword-sampled correctness audit with a second reader on SOUND calls; bank the sample beside `audit_sample_20260828.json`.
- Add a `review` field to the evidence-layer record schema (reviewer, date, verdict) before any review pass starts, so review is data not prose.
- Backfill "model:" into C01/C02/C03 Fable-era ledger headings, or mark the TODO.md provenance claim as prose-only.
- Send `docs/ERRATA_REGISTER.md` to the Director split by owner (TODO.md 2026-08-28, still open); route the 119 folio collisions separately (`docs/upstream/FOLIO_MARKER_COLLISIONS.md`).
- Specify the ingest-side row-count/anchor check for column offsets (`docs/COLUMN_OFFSET_UNDETECTABLE.md` recommendation) as an upstream work order.
- Fix the entry-card cap of 4 renderings (`app/main.cpp:1378`) after the design pass CLAUDE.md requires; measurements in `docs/PRESERVE_ALL_RENDERINGS.md`.
- Adam's ruling still needed: prayer phonetics vs card phonetics (`docs/PRON_ENGINE_VS_C03_LITURGY.md`).
- Fix the stale figure in `docs/RISK_REGISTER.md` R10 ("~200 shipped pages"; now 322).
- Record in `docs/ADR.md` the decision that mechanical correctness gates are closed and model-read audit with refute pass is the correctness method (currently in GATE_TRIALS.md and TODO.md only).

---

## Addendum 2026-09-04 — status of the conditions after C03 closed

*Appended, not edited: the body above is the 2026-09-01 draft Adam has not
yet ratified, and a reader may hold that version.*

- **C03 is 620/620, locked `complete: True` on 2026-09-03** (commit 1d829a1;
  200 pages c3p1–c3p200; markers 22/294/303/430/476/513/558). The §5b
  recommendation to open a new epic for C03:418–620 was followed in
  practice: batches 132–200 ran under the written briefs and the model
  policy, 185/185 merged specs exited 0 first run, and every kept erratum
  passed a skeptic. Register at 137.
- **Condition 2 (ledger level with HEAD): met.** `C03_CAMPAIGN.md` carries an
  entry for every batch through 200; counts agree with the commits.
- **Condition 3 (single spec document): met.** `docs/ALIGNMENT_LAYER_SPEC.md`
  written 2026-09-04; `CLAUDE.md` and `README.md` point at it.
- **Condition 4 (per-batch model provenance): resolved as prose-only.** C03
  batches 1–32 carry no model heading; the TODO.md statement that they were
  Fable is a prose claim without a per-batch record and is marked so there.
  Batches 33–131 say Opus; 132 onward name both models per the policy.
- **Condition 1 (Adam signs the reading of "validated"): open.**
- **Full-scan estimate (§5b precondition for C04):** C01–C03 = 1,184 segments
  in 368 pages between 2026-08-26 and 2026-09-03 (9 calendar days, ~132
  segments/day at the three-per-page cadence with two analysts, one
  reconciler and skeptics on demand per segment). The remaining 15 courses
  hold 7,777 segments: ~59 working days at that pace, or ~2,600 pages. C13
  (1,041) stays blocked, leaving 6,736 segments / ~51 days.
- **C03-stratified audit (§5b precondition for C04): drawn 2026-09-04**, seed
  20260904, headword-level (uniform over the C03-cited headwords, then
  uniform over that headword's C03 occurrences), n=100, with a second reader
  on SOUND calls per §5c.2. Result banked when it lands as
  `docs/AUDIT_2026-09-04_C03_STRATIFIED.md` and
  `data/alignment/audit_sample_20260904.json`.
- Backlog items closed today: ADR-012 recorded; RISK_REGISTER R10 figure
  corrected (368 pages); the `eng_order` pin already exists in
  `tools/test_gen_alignment_page.py` (§8 of that file) and the d=7 parent
  rule is pinned there (§15).
