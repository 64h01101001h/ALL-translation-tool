# Handoff log

The baton. **Newest entry at the top.** Append; never edit a past entry —
corrections go in an ERRATA entry of their own, so the record of what was
believed when stays intact.

Format and rules: `05_CROSS_CHECK_PROTOCOL.md`.

---

## 2026-09-12 — Codex — continuation checkpoint, C05 through 189

- Added 184–186 in `bb77f01` and 187–189 in `883a4ee`: six segments, 168 independently reviewed analyst spans and six sentence anchors. C05 reaches **189/511**, pages c5p1–c5p63. All eight pages since 165 add 731 analyst spans and 24 anchors. Separate proposals, reconciliation, semantic review, accepted specs and exact generator proofs remain preserved.
- Both new batches passed all **17 targeted fidelity, export and constitution suites** (10.46s and 10.51s). Exact reviewed tuples/ranges, prior links/notes/trees/evidence/ACIP mappings, previous main CSV content/citations, and all course/depth groups/counts/metadata were verified. The course export regression is now part of the recurring check set; 118 is a registered suite count, not a full-suite claim.
- Current bank: **42,514 links, 11,864 null exponents, 1,525 notes, 11 trees**. Evidence: **6,812 headwords/14,696 pairs**. CSV rows: main 21,229; reverse 20,889; course/depth 22,539; changes 31 versus bb77f01. Master and original corpus hashes remain unchanged; no source text or hgm_gloss edits.
- Upheld semantic review narrowed 185q1 to la/to; de remains omitted with a distributed-reference note. Its initial proposal/review and corrected spec/body are preserved, and scoped re-review approved the final files. Occurrence-level 'gyur/lead(s) approvals in 187–189 and conservative omissions in 185–186 were independently adjudicated with the same glossary counterevidence. No automatic cross-batch semantic substitution was made.
- E-186 records 'BRAL DU→'PHRAL DU at 187 as LOW/PROBABLE. The skeptical reviewer re-read the source and local raw commentary witnesses, verified all five grounds and checked 185 prior register records. Two byte-identical SE files read BRAL; SL reads PHRAL. Publication independence is unproved. Exact contexts, hashes and folio are committed in `data/alignment/reviews_c05/c5p63/commentary-local-witnesses.json`. The disputed text stays unpaired and unchanged.
- A fresh independent reviewer reproduced both banks, HTML and all four CSVs exactly at both fixed data commits (changes seeded from 70804c9 for bb77f01 and from bb77f01 for 883a4ee). All 168 tuples match canonical generator ranges and all six individual bodies/two page manifests agree. The committed 189 reproduction records 183,321 checks with zero failures. Final documentation review and read-only full-patch applicability evidence belong in sibling `campaign-artifacts/integration/continuation-through189-review.md`; no main integration is inferred.
- Next is **c5p64, C05:190–192**, with source/context snapshots only. The native/installed-phone checkpoint remains 171, locked-device launch remains unverified, and browser-policy rendering limits persist. No new device execution or main integration occurred. No additional source files are needed. Hourly continuation remains ACTIVE through 2026-09-15 at 07:00 America/Denver; this is an unfinished campaign.

## 2026-09-12 — Codex — ERRATA: course CSV depth provenance repaired

The continuation review found a defect beyond the earlier CSV retention checks:
the course CSV selected depth metadata globally by Tibetan/English pair. Adding
C05:178's depth-6 yin pa'i/is relabelled the unchanged C04 depth-5 row as depth 6.
Within a course, occurrences at several depths were also collapsed into a single
arbitrarily labelled count. The bank and source alignment records were intact.

The exporter now groups by course, Tibetan, English, and depth. Its existing
scalar depth and occurrence-count columns describe that exact group. The corrected
course CSV has **22,451 rows** instead of 21,743: 689 course/pair groups contain
several depths, requiring 708 additional rows. All previous aggregate course/pair
occurrence counts are retained; every new row's depth and count matches the main
CSV's source refs. The C04 yin pa'i/is row is depth 5, one occurrence. Column order,
verbatim text, UTF-8 BOM and CRLF remain unchanged.

The real exporter regression failed four assertions before the fix and passes
all five tests after it. Private CMake configuration confirms 118 registered and
written suites; the new test and three fixture-free supporting suites passed.
This is not a full 118-suite run. The full-data regeneration used the real view
builder in a temporary directory with `2f16dee`'s main CSV as the previous export.
Main/reverse/changes CSVs and dictionary HTML reproduce byte-for-byte, preserving
the accepted 55-row changes report. Both bank files and source/master hashes are
unchanged. Evidence is in sibling `campaign-artifacts/integration/course-csv-depth-fix/`.
After installing the corrected CSV, the export regression, view/layer agreement,
generated-doc synchronization and constitution suites all passed (4/4, 0.62 seconds).

C05 remains through 183; next is 184–186. Native/device/rendering and main-integration
limits from the preceding checkpoint remain in force. Independent source review
approved the repair and its tests without further findings; final committed-output
reproduction and read-only main apply checks are the remaining checkpoint steps.

## 2026-09-12 — Codex — continuation checkpoint, C05 through 183

### Done and verified

- Added C05:178–180 in `2f16dee` and 181–183 in `f30209b`: six source segments, 205 independently reviewed analyst spans, and six sentence anchors. C05 is now **183/511**, pages c5p1–c5p61. All source specs, review decisions, witness evidence and generation/test records are preserved.
- Each batch passed all **16 targeted fidelity and constitution suites**, in 10.45 and 10.22 seconds. Exact review tuples, source ranges, prior links/notes/trees/evidence/index mappings and earlier CSV content/citations were checked. One previous phonetic row gains its previously empty ACIP spelling from the new cited C05:182 word; that enrichment has an exact source witness.
- Bank: **42,340 links, 11,857 null exponents, 1,519 note blocks, 11 trees**. Evidence: **6,799 headwords / 14,646 pairs**. Since the handover at 165, the campaign has added 18 segments, 563 reviewed analyst spans and 18 sentence anchors. The master dictionary and original corpus hashes still match the earlier checkpoint; no source wording or hgm_gloss changed.
- The semantic review tightened 182 bsdums to agreement, leaving supplied participant wording outside the span. Both proposed 183 spelling corrections were refuted: the glossary supports snying rdze/compassion, and dod has a defensible wanting sense. Driving and the unresolved rescue construction remain unpaired.
- E-185 records MYON MONGS PA → NYON MONGS PA at 180 as LOW/PROBABLE. All five skeptical checks passed; the distinct digital reading is not claimed to be an independent publication or a checked blockprint. Its evidence now cites the exact committed witness file. The earlier two probable candidates remain provisional.

### Continuation and limits

- Next is **c5p62, C05:184–186**. Source/context snapshots are prepared; proposals have not started. All six new pages remain provisional machine attestations. C13 is still blocked; no additional source files are needed.
- No new native or phone execution occurred in this continuation. The earlier native/installed-phone checkpoint remains through 171, with phone launch unverified after the locked-device refusal. Browser rendering remains unverified under the existing URL-policy block; no bypass was attempted.
- Work remains in the isolated campaign branch; main integration has not occurred. Preserve the other session's main-checkout work and recheck its live state before integration. Bounded independent reproduction/integration evidence for this continuation is saved in the sibling `campaign-artifacts/integration/` directory.
- The hourly heartbeat remains scheduled through 2026-09-15 at 07:00 America/Denver. Read the sibling `CAMPAIGN_CONTINUATION.md` for the exact next action. This is a checkpoint in an unfinished campaign.

## 2026-09-12 — Codex — first continuation checkpoint, C05 through 177

### Done

- Advanced C05 from 165/511 to **177/511**, adding c5p56–c5p59: 358 independently reviewed analyst spans plus 12 sentence anchors. Accepted specs, semantic reviews, and landing manifests are committed. Original proposals and reconciliations are preserved in the sibling workspace's `campaign-artifacts/`.
- Commits: `947cf88` (166–168), `8a73874` (169–171), `bb2394f` (172–174), and `0c26f97` (175–177). Earlier `71d03b7` corrected four gates that omitted C05. `929f012` isolates the Catalog routing self-test's proposal store; `072a61f` fixes exact-PID iOS termination and deployment failure propagation.
- Recorded E-183 and E-184 as LOW/PROBABLE source-spelling/editorial candidates. Original Tibetan and English remain unchanged. C05:167's numeral discrepancy remains unpaired; C05:177's phrase retains its explicit ellipsis ambiguity note. No machine evidence was promoted to `hgm_gloss`.
- Updated the alignment spec to distinguish a proven grammatical contribution with no separately alignable English exponent from an unresolved or contradictory mapping. Historical nulls were not silently reclassified.

### Verified

- Each of the four batches passed all **16 targeted fidelity and constitution suites**. Final independent review reproduced all 97 c5p59 analyst spans, its body, both banks, dictionary HTML, and four CSV exports, and proved retention of prior bank links, notes, trees, evidence, and view citations. No unresolved review findings remain.
- Final bank: **42,129 links, 11,829 null-English records, 1,513 note blocks, 11 trees**. Evidence: **6,774 headwords / 14,593 pairs**. C01–C04 coverage remains 496/496, 68/68, 620/620, and 198/198 respectively.
- Master dictionary and corpus hashes still match the starting values: `hgm_dictionary_v27_2.json.gz` SHA-256 `3a99603cd9428f0d04e67c7d2790cc093f272162e7bf6278906a4b635f962253`; `full_parallel_corpus_v32.json.gz` SHA-256 `1a1d8764a960c119d227c3cb7a7b0cb03073f9dc54116a3ec4e585c40323ddf8`.
- Native build succeeded. The initial 116-suite run passed 115 and crashed in `app_selftest` on a read-only library fixture. A private worktree library clone removed that fixture failure; the subsequent Catalog proposal-store failure was fixed in `929f012`. Rebuilt `app_selftest` then passed in 307.52 seconds with the bank through 171. Other Preferences self-test settings remain shared: the entire harness is not hermetic.
- CMake now registers 117 tests. The new iOS regression suite passed its six methods / 21 scenarios, including failure cases; its registered CTest and both test-census checks passed. This is not a claim that all 117 tests ran together against the bank through 177.
- A signed Trainer build containing a 4,000-cloze sampled pack from the bank through 171 installed on Adam's iPhone. Pack generation reported zero unplaceable answers. Bundle-pack SHA-256: `ffd7ec4bafdacf1feae5d86416fa30173bb448c39d4423ade75cae0f3c187510`.

### Unverified and next

- iOS refused launch because the phone was locked. Adam was asked once to unlock; no response has arrived. Runtime/display and the new termination helper on hardware remain unverified. The installed pack predates rows 172–177 and its bounded samples do not prove complete C05 alignment coverage.
- Browser URL policy refused local HTML preview. Rendered-page inspection remains unverified; do not bypass the refusal with another delivery or rendering route. Source and structural checks passed.
- Work remains on isolated branch `codex/aci-alignment-20260915`; no main-checkout integration occurred. Another session is changing the main app. Earlier read-only apply checks passed against main `5240fbc`; recheck its live state before future integration.
- Next page is **c5p60, C05:178–180**. Its source snapshot is ready; independent proposals have not begun. No additional source files are needed. C13 stays blocked for alignment.
- Hourly continuation remains scheduled through Tuesday, 2026-09-15, 07:00 America/Denver. The campaign is unfinished. Read the sibling workspace's `CAMPAIGN_CONTINUATION.md` and `CAMPAIGN_STATUS.md`; final review evidence is in `campaign-artifacts/integration/final-checkpoint-review.md`.

## 2026-09-12 — Codex — campaign resumed; incoming claims checked

### Done

- Adam authorized continuing this campaign through 2026-09-15 at 07:00 America/Denver, superseding the older alignment pause. Work is isolated on `codex/aci-alignment-20260915` from `455c92d` while the main checkout has concurrent edits.
- Preserved 101 surviving C05 scratch files for segments 166–175, including reconciled specs 166–169 and both proposal angles through 175. Originals were left unchanged; every copy was checked by SHA-256. Durable archive and verification reports are in the sibling workspace's `campaign-artifacts/handoff-check/`.
- Fixed four page gates which enumerated only C01–C04: broken English words, split Tibetan syllables, degenerate members, and builder span coverage. They now discover later course directories and refuse an empty scan. Added a regression suite exercising C05, a future course, valid page counts, and absent pages.

### Verified

- Incoming claim 1: **CORRECTED.** Punchlist has 191 rows, not 191 verified currently open tasks. Its headings total 85 unblocked, 91 Adam, 15 outside, differing from the introductory split. Seven of ten sampled statuses/dependencies require correction; details and source citations are in `campaign-artifacts/handoff-check/report.md`.
- Incoming claim 2: **CONFIRMED for the deterministic sample.** Existing `build_drill_pack` produced 500 cloze items, 0 unplaceable, exit 0. This is a sampled check using the existing binary, not a proof over all draws.
- Incoming claim 3: **CORRECTED.** The allegedly lost scratch survives under an older project-related Claude scratch workspace. Preserved files and original paths/hashes are listed in `recovered-c05-inventory.json`.
- Incoming claim 4: **CORRECTED in scope.** The current prose generator rejects the legacy phonetics shape (340 d=7 spans lacking d=5 ancestors across the 14 clone pages). This does not mean the data is irrecoverable or impossible for any generator to reproduce. Existing pages remain untouched.
- Incoming claim 5: **CONFIRMED with terminology.** 2,891 identified recordings / 3,905 identified syllable readings; physical recording count is 3,002, not 2,891. All 3,002 manifest paths exist; no broken by-sound links.
- Baseline alignment checks and constitution: 15 command exits were 0. Four nevertheless omitted C05; the new regression exposed this with 16 failures before the fix and passed after it. The corrected gates inspect 511 pages, including 55 C05 pages. Independent review approved the narrow fix.
- Worktree CMake configuration registers 116 suites (one new regression) and reports all fixtures present. A full build/test run is separately in progress; these configuration counts are not a test-pass claim.

### Claims to verify

- Re-run returned C05 specs independently, then perform semantic reconciliation and adversarial review before banking any new links.
- Confirm the final full native build and tests from their logs; they had not completed at this entry.
- Inspect the final page, bank, desktop data loading, and regenerated iOS pack before claiming propagated results.

### In flight

- Two fresh independent analyses of C05:166–168 are complete; reconciliation is running. No new page was banked at this checkpoint.
- Hourly continuation is scheduled through the deadline; exact recovery state is in the sibling workspace's `CAMPAIGN_CONTINUATION.md`.

### Do not

- Do not repeat the lost-scratch claim or treat the stale punchlist as a current open-task count.
- Do not label new work as Claude-produced. This handover uses Codex agents in the independent roles; preserve actual provenance and provisional labels.
- Do not pair C05:167 `bzhi` with the source English “three”; preserve both source texts and the existing pending discrepancy.
- Do not scan C13, promote hgm_gloss, overwrite concurrent main-checkout work, spend API money, send messages, or publish releases.

## 2026-09-11 — Claude (Opus 5) — first handoff; this folder

### Done

- Wrote `docs/handoff/` in full: `README.md`, `00_ORIENTATION.md`,
  `01_INVIOLABLE_RULES.md`, `02_THE_TASK.md`, `03_HOW_TO_RUN.md`,
  `04_STATE.md`, `05_CROSS_CHECK_PROTOCOL.md`, and this log.
- Added the frequency-ordered-vocabulary item to the top of `TODO.md`, in
  Adam's own words.
- Earlier the same day: the Learn tab's twelve tools completed; Boundary Hunt
  built (it had been falsely marked BUILT); the menu bar reduced 19 → 11;
  `progress.db` schema-versioned with `docs/DATA_MIGRATION.md` as its promise;
  right-click → Pronounce; constitution rule R9; `allcore::placeBlank`;
  `docs/RELEASE_PUNCHLIST.md` generated from a survey of all three backlogs.

### Verified

Facts in this folder were gathered by five independent readers, each instructed
to verify against the repository rather than repeat prose. Specifically checked:

- **115 ctest suites, 100% passing, 312.81 s** — run.
- **Course coverage** — derived by counting page files and querying
  `corpus_segments`, not from the ledgers: C01 496/496, C02 68/68, C03 620/620,
  C04 198/198, C05 165/511.
- **Gloss population** — `sqlite3` on the spine: 105,634 entries, 12,004
  glossed, 93,630 not, 88,369 machine-eligible.
- **Layer counts** — 41,759 links, 11,777 with `"eng": null`, 6,756 evidence
  headwords, 14,528 pair records.
- **`constitution: all rules hold`** — run.
- **The two audit rates and their intervals** — read from the audit documents.
- **Five doc numbers are stale** (the "86 suites" claim in two files, the
  "37 of 73" in `FIXTURES.md`, the `cmake-build` path, the C01 header table).

### Claims to verify

Work these first. Each is something I asserted that another model would
reasonably act on without re-checking.

1. **`docs/RELEASE_PUNCHLIST.md`'s 191 open items and its blocker
   classifications.** Generated by agents reading three backlog files. The
   *counts* are mechanical; the *classifications* (`adam-ruling` vs `none`) are
   judgements and some will be wrong. Spot-check ten.
2. **"Zero drills are actually unplaceable"** — measured once, on a 500-drill
   build. Re-run `build_drill_pack` and confirm the count still reads 0.
3. **"C05's unlanded specs for 166–175 are gone."** I believe session scratch
   did not survive. If you can find them, that is 10 segments of work recovered.
4. **The claim that `c4p1–c4p7` and `c5p1–c5p7` cannot be regenerated.** Taken
   from the campaign ledgers, not tested. Do not test it destructively.
5. **The Kawachen audio bank figures** (2,891 recordings / 3,905 readings) —
   measured by a probe against `data/audio/kawachen`, which is gitignored and
   exists on Adam's machine only.
6. ~~the commit-trailer figure~~ — **resolved before handing over.** Counted
   directly: **184 of 642** commits since 2026-08-25, not 183 of 641. The
   agent's figure was off by one on both terms. The twelve most recent commits
   are clean, so the breach is historical.

### In flight

- **The 85 unblocked punchlist items.** Nine closed today. The next three I had
  queued: the Compare pane gaps (encoding choice, compare-with-backup,
  surfacing invalid regex filters), `package_macos.sh`'s missing
  relaunch-on-exit trap, and making Lookup/Overlay/terminology reach one layer.
- **The alignment campaign is paused, not in flight.** Resuming it is a
  decision, not a continuation — check with Adam before spending a session on
  C05:166 while the release is the stated priority.

### Do not

- **Do not scan C13**, and do not build a sixth column-offset detector. Five
  failed; `docs/COLUMN_OFFSET_UNDETECTABLE.md` exists to stop the sixth.
- **Do not promote anything into `hgm_gloss`.** Not from the layer, not from
  the AI glossary, not from a dictionary. It needs a human ruling.
- **Do not "fill" a no-exponent span.** A Tibetan morpheme with no English
  exponent is a finding, not a gap.
- **Do not restructure `app/main.cpp`.** The single-translation-unit design is
  deliberate and recorded in `docs/ADR.md`.
- **Do not raise an inner wall-clock ceiling** to turn a red run green. Re-run
  on a quiet machine.
- **Do not pipe a build through `| head`** — it SIGPIPEs the compiler and you
  will test a stale binary. That happened today.
- **Do not plan around broad AI glossary coverage.** It is a 1.6% pilot blocked
  on billing.
- **Do not resolve the twelve release blockers that are Adam's.** They are
  licence, privacy and ship/hide decisions.
