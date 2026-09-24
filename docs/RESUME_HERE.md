# Resume here — handed off 2026-09-24, before the alignment restart

Adam (2026-09-24, small hours): pause, clean up, note where we are, and start
the alignment in a FRESH window. This note is that state. Under the quota
ruling a new session reads this, not the old conversation. Detail lives where
it is linked.

## Do not collide

- **The `mutate.py` background task is FINISHED** (session "Make mutate.py
  plant mutants in an isolated copy", last activity 2026-09-24 00:48). Its
  work is on branch `claude/interesting-mccarthy-4dda49` (worktree
  `.claude/worktrees/interesting-mccarthy-4dda49`), **not merged and not
  reviewed**. The edit ban on `tools/mutate.py` / `tools/mutation_sweep.json`
  is lifted, but review and merge that branch before touching them on `main`,
  or the two will conflict.
- **Never `git add -A` or `commit -a` while a workflow runs.** Stage named
  paths (a TODO commit swept a live mutant into HEAD on 2026-09-23).

## Standing rulings (Adam, 2026-09-23) — in force

1. **Model pinned: `claude-opus-5`** at every call site in
   `.claude/workflows/scripts/c05-align.js`. Switching needs the shadow test.
   NOTE: this app session now reports its OWN model as Fable 5.1 (it moved on
   2026-09-24). The harness pins the campaign model explicitly, so the
   session model does not touch the campaign. Do not "fix" the pin.
2. **Quota: upwards of 75% of each week's allowance goes to the campaign.**
   Lean sessions, no research fan-outs unless Adam asks.
3. **Hold:** the lander refuses every row in `data/alignment/held_rows.json`
   (C18:560-760, C15:148-165, C18:1007-1024) and refuses everything if that
   file is missing. None are in Course 5.
4. **Readers:** Geshe Michael or the main Diamond Cutter Classics translators
   — not ACI. Only their judgments count as human-verified.

## Where the work stands

- **Digest #10** regenerated for 23 Sep, in Gmail drafts, verified
  (`98c5bfdf`). Adam: attach `docs/digests/Translation Tool - Daily Digest
  #10 - 20260923.docx` by hand, send, and discard the stale 22 Sep draft.
- **Course 5: 366 of 511.** Bank 50,370 links; 576 reading pages (578 files,
  two of them index pages). **Next page is `c5p123` = C05:367-369**, previous
  `c5p122`.
- `build/align_scratch/` does not exist — a clean start; nothing stale from
  the 22 Sep run survives anywhere.
- `ma byin lan` (E-289) is now on the dictionary work order
  (`docs/SPINE_TIBETAN_DEFECTS_2026-09-16.json`), because digest #10 says so.
- **Nothing pushed since 18 Sep** (`git rev-list --count origin/main..main`);
  Adam has not said to push.

## THE ALIGNMENT RESTART — the exact recipe (read this, not the harness)

**"Make SCRATCH durable" needs NO code change.** The harness falls back to
`build/align_scratch/c05` — repo-local, gitignored, survives a session
restart — whenever `args.scratch` is NOT passed. The 367-369 loss came from a
launch that passed the session scratchpad. So: **never pass `args.scratch`.**
Each analyst writes its proposal to `<scratch>/<seq>-<angle>/` and the
reconciler writes `<scratch>/<seq>-reconciled.json`, so all three artefacts
per segment persist on disk.

1. **Args, with no model between the spine and the JSON:**
   `python3 tools/dump_spine_segments.py --course C05 --seqs 367-369 --precedent`
   (~46 KB: three rows plus three precedent extracts). The harness checksums
   each inlined row in its pre-flight (`--verify-inlined`); the precedent
   carries provenance and every agent is told to re-count it.
2. **Launch:** `Workflow` with `scriptPath:
   .claude/workflows/scripts/c05-align.js` and `args` = the dumper's object
   verbatim (segments + precedent; `seqs` is discovered from `segments`).
   **Watch the result for `failed: You've hit your weekly limit`** — the
   22 Sep run died on it and still left files that look finished. A spec
   whose reconcile did not report success is NOT a deliverable.
3. **Land, dry-run first:**
   `python3 tools/land_alignment_page.py --page c5p123 --seqs 367,368,369
   --specs build/align_scratch/c05 --title "<a few words>" --prev c5p122
   --dry-run`. It refuses held rows, non-contiguous seqs and any generator
   non-zero exit. Span-head licensors the reconciler flags go into
   `data/alignment/span_head_allow.json` before the real landing.
4. **Errata:** for each seq, if `<scratch>/<seq>-reconciled/errata.json`
   exists (the current harness puts it there; the older loop read
   `<scratch>/<seq>-errata.json` — check both), run
   `python3 tools/merge_errata.py <file> --course C05 --apply`. Then
   `python3 tools/check_errata_landed.py <workflow task output file>` so an
   erratum reported but never written is NAMED, not silently skipped. Then
   `python3 tools/build_errata_register.py`.
5. `python3 tools/build_alignment_layer.py` prints COVERAGE and the bank size
   — the live numbers, never a ledger line. Then the gates:
   `cmake -S . -B cmake-build-release && cmake --build cmake-build-release
   -j4 && ctest --test-dir cmake-build-release -j4 --output-on-failure`
   (134/134 on 2026-09-23; reconfigure first — `ctest -N` is a census of
   what was configured, not of what exists).
6. **Commit named paths** (`data/alignment/pages_c05/c5p123.html`, the
   `alignment_*.json` outputs, the errata register, the page map in
   `tools/build_alignment_layer.py`, licensors) and append a batch record to
   `data/alignment/C05_CAMPAIGN.md` in the existing format: coverage, bank,
   errata ids, licensors, models.
7. Then **nine-segment launches** (370-378 → c5p124-c5p126), landing each,
   under the 75% ruling. After C05: C06..C18 in order; C13 stays out.

**Known and deliberately NOT done (do before or between batches, not instead
of them):**
- The brief headers still say "RECONCILE and REFUTE agents run on Claude
  Fable 5.1" (MODEL POLICY 2026-09-01) while the harness pins
  `claude-opus-5` (ruling 2026-09-23) — every judge is told something untrue
  about itself. Fix: edit `docs/alignment_briefs/PROPOSE_BRIEF.md` and
  `RECONCILE_BRIEF.md`, then `tools/sync_workflow_briefs.py --write`; never
  hand-edit the inlined literals. Small; worth doing before the first launch.
- The `parallel()` → `pipeline()` swap in `c05-align.js` (measured 1.59x
  faster per run, no effect on any judgment). Optional. Keep the three
  workflow gates green (`workflow_scripts_parse`, `workflow_briefs_in_sync`,
  `workflow_briefs_delivered`).
- The Opus 5.5 shadow test (ruling 1). Not run.
- The teaching-index honesty defect ("Geshe Michael teaches this idea" never
  checks the speaker). Not fixed; digest #10 says so.

## Waiting on Adam

- Attach, send and tidy digest #10 (above).
- Read the held rows: `docs/research/held_rows_reading_sheet.md` (~15 min).
- Name the Diamond Cutter Classics readers for the first blind test.
- Say whether to push.
- Review or discard the `mutate.py` branch (above).
- The rest under "Waiting on Adam" in `TODO.md` section AC.
