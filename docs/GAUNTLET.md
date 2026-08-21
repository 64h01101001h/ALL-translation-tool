# THE GAUNTLET — LEDGER
Per docs/GAUNTLET_PROMPT.md. Tracks close by drawn blood (confirmed,
fixed, fossilized) or adjudicated exhaustion. Findings must name why
they were invisible to engines 1–8.

## G1 — THE MONKEY WITH A MEMORY (hunting)
The driver: `--gauntlet <seed> <steps>` — deterministic mt19937 walk
over real actions (group/pane switches, band-tool clicks, checkbox
toggles, typed line-edits with Return, Escape), one action per 15ms
tick, a modal-closer that rejects whatever opens, invariants after
every step (window visible, tab count, menu count, closable modals),
journal replay on failure, `--gauntlet-verbose` telemetry (per-action
lines + 100-tick heartbeats + modal closes).

### Blood
- **G1-01 · P1 · FIXED.** `safeGetOpenFileName`/`SaveFileName`/
  `OpenFileNames`/`ExistingDirectory` guarded only `g_sweepActive` —
  under EVERY OTHER harness mode a real native NSOpenPanel opened
  (seed 1, step 53, "Open scan"; the helper's own comment records the
  original NSSavePanel incident, but the guard was never widened).
  All four now test g_harnessRun. Invisible before: the sweep had its
  private guard, and no other engine ever CLICKED a picker path.
- **G1-02 · the mirror bit us too (driver lesson, fossilized in the
  driver).** The stepper performed clicks ON its own stack; a handler
  that execs a dialog (Compose bibliography entry, seed 1 step 115)
  froze the stepper's heartbeat inside the nested loop — diagnosed by
  `sample` (idle top-level loop, no timer armed) + heartbeat
  telemetry. Actions are now deferred via singleShot(0) so the
  stepper always beats and can close what opens. Seed 1 × 300 now
  survives with four modal opens/closes on record.

### Fleet telemetry + fossil
Seeds 2–11 × 300 steps: **10/10 survived** post-fix. gauntlet_walk
(seed 1 × 300, offscreen, suite 67) is now a permanent battery member —
the walk that drew first blood guards forever.

## G2 — THE WEIRDNESS PERCENTILE (closed by exhaustion-with-telemetry)
tools/weirdness_rank.py scores all 105,634 entries (length extremes,
stack depth, out-of-block codepoints, ASCII-in-Tibetan, control chars,
flag density, empty-field patterns); 8,607 nonzero, top 1,000 banked
(build/weird_top.tsv; champion: a 140-char multi-variant monster with
22 inline markers). Two permanent proofs over the tail:
- **weird_battery (suite 66):** every converter over all 1,000 —
  0 failures, 983 honestly flagged (the ⟨⟩ grammar doing its job on
  editorial-marker rows), all outputs bounded.
- **The card sweep:** the F1 honesty invariants now ALSO run over the
  weird slice inside app_selftest ("weird-slice 1000") — provisional
  marking and honest absence hold at the tail, not just the stride.
No blood: the tail was already survivable — now it's PINNED
survivable. Closed as exhaustion with telemetry: the method ran at
full depth and left two permanent suites behind.
