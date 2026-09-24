# Resume here — handed off 2026-09-23

Adam: *"let's turn this over to a new session."* This note is the state, kept
short on purpose: under Adam's quota ruling, a new session should read this,
not the old conversation. Detail lives where it is linked.

## Do not collide

- **A background task is running in a separate local session:** make
  `tools/mutate.py` plant mutants in an isolated copy (task_54dc079f). Until it
  reports, **do not edit `tools/mutate.py` or `tools/mutation_sweep.json`.**
- **Never `git add -A` or `commit -a` while any workflow or other session may be
  writing to the tree.** Stage named paths. On 2026-09-23 a TODO commit swept a
  live generator mutant into HEAD that way (fixed in d352a01f).

## Standing rulings (Adam, 2026-09-23) — in force

1. **Model: `claude-opus-5`, pinned** at all six call sites in
   `.claude/workflows/scripts/c05-align.js` (a probe proved a full id is
   honoured). The `opus` alias moved to Opus 5.5 that day. Switching needs the
   shadow test below first.
2. **Quota: upwards of 75% of each week's allowance goes to the campaign.**
   Keep orchestrating sessions and turns lean; no large research fan-outs
   unless Adam asks. Cost is quota, not cash (flat Max 20x plan).
3. **Hold: flagged mispaired rows stay out** until Adam reads them —
   C18:560-760, C15:148-165, C18:1007-1024. Enforced: the lander refuses any
   row in `data/alignment/held_rows.json`, and refuses everything if that file
   is missing.
4. **Readers: Geshe Michael, or the main Diamond Cutter Classics translators —
   not ACI.** Only their judgments count as human-verified.

## Where the work stands

- **Course 5: 366 of 511.** Bank 50,370 links, 578 pages. **C05:367-369 must be
  re-run from scratch** — their specs were lost when the session scratchpad
  was wiped.
- **Span-ID compiler landed** (`tools/span_id_compiler.py`, b2e2b584): proved
  to lose nothing (953 blocks round-trip byte-identical), NOT proved to save
  anything, **not wired into the campaign.** Its remaining defects are in
  `TODO.md` and must close first.
- **Research banked:** `docs/research/2026-09-23-ai-avenues/README.md` — every
  AI avenue, what to avoid and why, the projection, funding.
- **Every actionable item is in `TODO.md` section AC**, grouped by owner, with
  the rulings above ticked.
- **Nothing pushed** since the 2026-09-22 pause; Adam has not said to push.

## Suggested order for the new session

Free fixes first, because each makes every later batch cheaper or safer:

1. **Make SCRATCH durable and keep both proposals per segment** — or the next
   batch's specs can vanish the way 367-369's did.
2. **Swap the `parallel()` barrier for `pipeline()`** in c05-align.js —
   measured 1.59x faster per run, no effect on any judgement.
3. **Re-run C05:367-369** on the pinned model, in a fresh lean session.
4. **Run the Opus 5.5 shadow test** Adam ruled for (~36 landed segments; Adam
   reads a random 50 disagreements).
5. In parallel, **fix the teaching-index honesty defect** — the app card says
   "Geshe Michael teaches this idea" but its builder never checks the speaker.

## Waiting on Adam

- Read the held rows: `docs/research/held_rows_reading_sheet.md` (~15 min).
- Name the specific Diamond Cutter Classics readers for the first blind test.
- Say whether to push.
- Digest #10 was regenerated for 23 Sep: attach `Translation Tool - Daily Digest #10 - 20260923.docx` to the new draft by hand, send it, and discard the stale 22 Sep #10 draft.
- The other rulings listed under "Waiting on Adam" in `TODO.md` section AC.
