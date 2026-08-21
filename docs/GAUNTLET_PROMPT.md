# ENGINE 1.0.0 — THE GAUNTLET
*(The ninth master prompt, superseding the Burndown Directive's closing
line at Adam's word. Eight engines examined the app AT REST — static
screens, code, headless batteries, single-shot tests — and drove that
world to zero. The Gauntlet attacks the one world they could not see:
the app IN MOTION and UNDER TIME. Its prey is exclusively the bug
classes that are STRUCTURALLY INVISIBLE to everything that came before.
It runs against the living RC, under /loop, until it has drawn blood
or proven exhaustion on every track.)*

## PART I — THE DOCTRINE OF DRAWN BLOOD

**§1. A quota, not a checklist.** Every prior engine could close a
track by verifying it. The Gauntlet cannot: its tracks close only by
(a) producing a CONFIRMED finding — a real bug or a real UX wrong,
reproduced, fixed, and pinned — or (b) demonstrating EXHAUSTION: the
track's method ran at full depth, its telemetry is in the ledger, and
an adversarial reviewer agrees nothing was left on the table. "It
looks fine" closes nothing. The engine assumes bugs exist and treats
a quiet track as a method failure until proven otherwise.

**§2. Only the invisible prey.** Any finding a prior engine's method
COULD have caught is logged and fixed but does not count toward a
track's blood — the Gauntlet hunts exclusively where the old lights
don't reach. Every confirmed finding must name the reason it was
structurally invisible before.

**§3. Every finding becomes a fossil.** A confirmed bug's reproduction
becomes a permanent battery case; a confirmed UX wrong becomes a
constitution rule or a pinned invariant. The Gauntlet leaves the
suite bigger than it found it, or it wasn't here.

**§4. The law stack is unchanged** — CLAUDE.md honesty rules, the
constitution, the settled rulings of engines 1–8, batteries green at
every commit, the three-gate press, §10 adversarial review, announced
presses, drills run twice, ledger docs/GAUNTLET.md, memory banked.

## PART II — THE SIX TRACKS

**G1 · The monkey with a memory (stateful sequence fuzzing).**
Every existing test is single-shot; real bugs live in SEQUENCES.
Build `--gauntlet <seed> <steps>`: an in-app driver that performs a
deterministic random walk of REAL actions — raise panes, click band
tools, load and switch documents, click words, toggle layers
mid-render, type into fields, open and dismiss dialogs — and after
EVERY step asserts the global invariants: process alive, event loop
responsive, no modal under harness, the card's honesty markers intact,
no widget tree corruption. A failing seed replays exactly; its minimal
prefix becomes a battery case. Run: many seeds × hundreds of steps,
then longest-surviving seeds extended. Invisible before because: no
prior method ever performed two actions in a row.

**G2 · The weirdness percentile (adversarial data sweep).**
F1 sampled every 23rd entry — systematic sampling structurally misses
the tail. Rank ALL 105,634 entries by weirdness (length extremes,
stack depth, variant count, mixed scripts, rare codepoints, empty-field
combinations, flag density) and drive the TOP 1,000 weirdest through
the full card render and every engine, invariants asserted. The same
for the corpus's weirdest segments and the catalog's weirdest
filenames. Invisible before because: stride sampling is blind to the
tail by construction.

**G3 · The pixel inquisition (rendered-artifact detection).**
The visual gate detects CHANGE, not WRONGNESS. Build automated
screenshot analysis (PIL) that hunts: tofu boxes (missing glyphs),
text clipped mid-glyph or elided in load-bearing labels, widget
overlap, horizontal scrollbars where none belong — swept at THREE
window sizes (small laptop, default, wide) across every pane, plus
the card at zoom extremes. Invisible before because: the gate compares
pixels to yesterday's pixels, never to RIGHTNESS.

**G4 · The soak (time as an adversary).**
Nothing ever ran longer than a battery. Drive the app for hours under
G1's monkey while sampling RSS: memory growth curves, unbounded caches
(the folio-scan cache, session files, logs, the usage ledger),
backup pruning proven at scale, FTS handles, QObject leaks under
repeated pane churn. A flat curve after warmup closes the track; a
slope is blood. Invisible before because: every prior process lived
for seconds.

**G5 · The stranger's reflexes (convention guessability).**
Every audit asked "does what exists work?" — never "does what a Mac
user TRIES first work?" Audit the reflex vocabulary: ⌘O, ⌘S, ⌘W,
⌘, (preferences), ⌘N, drag-a-file-onto-the-window (and the Dock icon),
Open Recent, right-click context menus on every major surface, the
window-title proxy icon, pinch/scroll zoom, Escape backing out of
transient states. Each reflex either works, gets built, or gets a
ruled exception with a reason a stranger would accept. Invisible
before because: no prior engine ever asked what a hand does before
the eyes read.

**G6 · The seam ledger (cross-feature promise chains).**
Every pane was proven ALONE. Enumerate the promises that SPAN panes —
a ruling in Approval reaches the open card WITHOUT relaunch; a
glossary entry shows in Draft's suggestions; a team comment appears
on the second machine after sync; an imported release refreshes every
layer including lazy ones; identity set in Propose gates Catalog
correctly; Files' rename updates the Overlay's open document — and
prove each chain LIVE, end to end. Every broken chain is blood; every
proven chain becomes a pinned integration case. Invisible before
because: every prior test held one pane still and asked it questions.

## PART III — ORDER, THROUGHPUT, EXIT

**§10. Order:** G1 first (it multiplies every other track's reach),
then G2 ∥ G3 (independent), G6, G5, G4 last (it rides everything
built before it). P0s jump. Fix batches ride announced presses;
behavior changes get the adversarial reviewer.

**§11. Throughput:** waves close multiple tracks' increments;
independent analysis fans out to background agents; app/main.cpp
edits stay serialized. The burndown report each wave: blood drawn ·
fossils added · tracks closed/exhausted · the waiting list verbatim.

**§12. Exit.** All six tracks closed by blood-and-fix or by
adjudicated exhaustion; every fossil in the battery; the ledger's
closing entry states, in one honest paragraph, what the Gauntlet
proved about the RC that nothing before it could have — and the RC
that survives it is the build that deserves the name 1.0.0.
