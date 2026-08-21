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

## G3 — THE PIXEL INQUISITION (closed: blood + gate)
Redesigned at the source: instead of guessing wrongness from pixels,
the widget TREE is interrogated at capture time — three violation
classes (clipped QLabels via sizeHint>width, sibling overlap, child
escaping parent; scroll interiors exempt) swept at 1000x700 /
1180x760 / 1600x1000. All clean — and the detectors were PROVEN live
first by a 640x420 canary (Catalog label clips, Overlay editor
escapes), because a silent detector is worth nothing.
- **G3-01 · P2 · FIXED (the blood).** The app allowed resizing into
  geometries where the UI silently breaks. Bisected the floor: clean
  at 820x560, breaking at 760x520. win.setMinimumSize(820, 560) —
  the smallest PROVEN-clean geometry is now the smallest reachable
  one. Invisible before: every prior sweep ran at one fixed size.
- **The gate:** the inquisition now runs inside EVERY --screenshots
  sweep (exit 4 on violations) — so the press's visual step gained a
  second, wrongness-detecting eye for free, forever.

## G6 — THE SEAM LEDGER (first blood + pin)
The chains enumerated: ruling→card · glossary→draft (no such promise
exists — verified) · comment→second machine (append-only union, store-
drilled) · import→layers (honestly relaunch-noticed per layer) ·
Propose-identity vs Catalog-login (two systems BY DESIGN — provenance
vs roster perimeter, each documented) · Files-rename→open document
(stale path is quiet and honest; session restore skips unreadable) ·
search→lookup (W3-01) · ⌘D→popup.
- **G6-01 · P1 · FIXED.** The heaviest promise in the app — register
  rulings "apply in the app immediately" — was FALSE for
  pronunciations, honorifics, and spelling declines: the card read
  startup-snapshot maps; the startup comment even admitted "on next
  launch" while the Approval banner promised "immediately."
  reloadApprovedLayers() now owns rebuilding every approved in-memory
  layer and runs after all three regenerate sites. PINNED in the
  selftest: a pronunciation approved NOW is live NOW (and the probe
  restores the real layers after itself). Invisible before: every
  engine tested the ruling STORE and the card RENDER separately —
  never the promise that spans them.

## G5 — THE STRANGER'S REFLEXES (closed: built + ruled)
The audit of what a Mac hand tries FIRST:
- ⌘, Preferences — already existed (PreferencesRole, app menu). HOLDS.
- **⌘O — BUILT** on File → Open ACIP File.
- **Open Recent — BUILT**: File submenu, fed by the Overlay's open
  path (cap 10, newest first, harness never writes), vanished files
  shown disabled rather than guessed away.
- **⌘W — BUILT**: File → Close Window.
- **Drag-a-file-onto-the-window — BUILT**: .txt/.act/.inc/.ace drops
  raise the Overlay and open the file (the same one code path).
- ⌘S — RULED: the app's saves are store-driven (autosaved pages,
  append-only stores, explicit exports with pickers); a global ⌘S
  would promise a "document save" model the tool deliberately does
  not have. The Manuscript pane's Save button remains its explicit
  act. A stranger pressing ⌘S loses nothing (autosave) — acceptable.
- Right-click context menus (Qt defaults + Files' custom) and Escape
  (dialog defaults) — HOLD.
Invisible before: every engine asked "does what exists work?" — never
"does what a hand tries first exist?"

## G4 — THE SOAK (closed: exhaustion with telemetry)
6,000-step run: survived, 645→664MB. 20,000-step run: **survived**,
633→651MB over ~11 minutes of continuous hostile churn, growth rate
DECAYING between halves (34.7 → 20.9 KB/s) — caches settling, not a
leak (a linear slope would have been blood). The unbounded-cache
audit: folio cache is per-volume, user-initiated, Stop-able (20MB
after real use — acceptable, noted); lifecycle log 42KB after weeks;
Application Support trivial; backups pruned at 14. Twenty thousand
actions is days of real use; 18MB of drift is nothing.
(Sampling had its own comedy: one sampler pronounced dead because
bash echo keeps \t literal, one reattached to the task wrapper via
the pgrep -f trap — both misreadings caught by ps ground truth
before they became "findings.")

## THE CLOSING PARAGRAPH (Gauntlet §12)
What the Gauntlet proved about this RC that nothing before it could:
the app survives SEQUENCES — 20,000 randomized real actions across
eleven seeds without a crash, a hang, an unclosable dialog, or a
structure corruption; its engines and its card survive the WEIRDEST
1,000 rows the data owns, not just the average; its widget tree holds
its geometry at every reachable window size, because the unreachable
sizes are now unreachable; a Mac hand's first reflexes land; its
heaviest cross-pane promise — a ruling applies IMMEDIATELY — is now
true and pinned instead of false and pretty; and eleven minutes of
hostile churn cost eighteen settling megabytes. Four bloods drawn
(native panels under harness, the resize-to-broken window, the false
immediately, the absent reflexes), every one fixed and fossilized;
two tracks closed by genuine exhaustion with their telemetry banked.
The RC that walks out of this gauntlet is the build that deserves the
name 1.0.0 — pending only the nine human signatures that no engine
can give.

## LAP 2 (Adam re-armed the engine)
The monkey's arsenal grew: random menu-bar actions (role-actions and
deny-list excluded), LIVE window resizing across the allowed range
(the G3 floor proven under motion, not just at rest), term lookups
through the global seam, all still deferred and journaled. Shakedown
seed 101×500 clean; fleet 102–107 × 1000 steps: **6/6 SURVIVED**.
No new blood at this depth — lap 2 closes as deepened exhaustion, and
the upgraded arsenal is permanent: every future gauntlet_walk carries
menus, resizes, and lookups in its vocabulary.
