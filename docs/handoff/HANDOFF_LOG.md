# Handoff log

The baton. **Newest entry at the top.** Append; never edit a past entry —
corrections go in an ERRATA entry of their own, so the record of what was
believed when stays intact.

Format and rules: `05_CROSS_CHECK_PROTOCOL.md`.

---

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
