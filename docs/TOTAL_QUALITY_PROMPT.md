# MASTER PROMPT 3 — THE TOTAL QUALITY ENGINE
*(successor to the Phase-1 surface audit and the Phase-2 lived-experience
audit; both remain settled law. This prompt is not another audit — it is a
STANDING ENGINE that unifies design, development, debugging, and testing
into one continuous cycle, run under /loop until every track's backlog is
empty or ruled.)*

## PART I — STANCE

**§1. One discipline, four lenses.** Design, development, debugging, and
testing are not phases; they are four lenses on the same question — *is
this tool worthy of the texts it carries?* Every cycle of the engine turns
all four: a design defect is a bug, an untested path is a debt, a compiler
warning is a smell pointing at a future crash, and a feature that exists
but was never proven is not a feature.

**§2. The inherited law.** Phase-1's design register (tokens, badges,
band grammar, honest counts/elision/empty states) and Phase-2's process
machinery (sabotage testing, adversarial §10 review, §11 visual-regression
gate, W10 usage ledger, warnWriteFail/warnOpenFail voices) are inviolable
infrastructure. The CLAUDE.md honesty rules (hgm_gloss binding; never
reconstruct engines from spec; never guess; provenance sacred) outrank
everything in this prompt.

**§3. The cycle.** Each /loop wave runs HUNT → FIX → PROVE → PRESS:
- **HUNT**: pick the highest-yield lens (below) and find real defects,
  each banked with a repro or a measurement.
- **FIX**: implement, smallest-honest-change first; P0 interrupts
  everything.
- **PROVE**: the fix carries its own proof — a new pin, battery case,
  sanitizer pass, measurement, or screenshot; the full 59-suite gate and
  the §11 visual gate stay green at every commit. A nontrivial fix gets
  the §10 adversarial reviewer.
- **PRESS**: announced batch presses; the installed app is the only
  reality that counts.

**§4. The ledger.** docs/TOTAL_QUALITY.md — every hunt's findings,
severity (P0 data-loss/dishonesty/crash · P1 blocks-a-journey ·
P2 friction/debt · P3 polish), fix, and proof. Memory banked every wave.

## PART II — THE HUNTING GROUNDS

**T1. The compiler already knows.** Drive the warning count to zero or to
explicitly-ruled exceptions. Every warning is triaged, not silenced:
a real bug fixes the bug; a false positive gets a comment and, only
then, a targeted suppression. Then raise the wall: -Wall -Wextra as the
standing build, new warnings fail fast.

**T2. Sanitizers on the batteries.** A dedicated ASan+UBSan build runs
the full ctest battery. Every finding is a real bug by definition —
undefined behavior in an engine port can silently diverge from the
canonical Python. Findings fixed in the mainline, proven by re-running
the sanitized battery clean. Leak findings triaged against Qt parent-
ownership idioms (a parented widget is not a leak).

**T3. Fuzz the engines at their seams.** The engine ports promise
parity with canonical Python. Batteries prove the KNOWN cases; fuzzing
hunts the unknown: random/mutated wylie ↔ unicode round-trips, ACIP ↔
EWTS, Sanskrit chains, tokenizer/segmenter on malformed input. Every
crash or C++-vs-Python divergence is P1: fix in C++ (the Python oracle
is authority — rule 2), bank the case as a permanent battery fixture.

**T4. Performance is a feature with numbers.** Measure: cold start to
usable window, big-document load (the full Diamond Cutter, not the
excerpt), spine query latency under the card's fan-out, library index
build, OCR pipeline. Pin the numbers as regression floors (a perf pin
fails if a future build is >2x slower). Fix only what a user feels;
log the rest.

**T5. The monolith, managed — not detonated.** app/main.cpp is ~33k
lines. RULING built into this prompt: no big-bang refactor — history
proves the block-edit hazard. Instead: (a) map it — a table of every
class/section with line ranges, banked in the ledger so future edits
anchor precisely; (b) strangler-extract ONLY units that are already
self-contained (pure helpers, the ux:: namespace, RibbonBar) into
headers when — and only when — a wave already touches them for another
reason; (c) every extraction proven by identical battery + visual gate.
Code that is stable and proven stays where it is; churn is a defect,
not a virtue.

**T6. Dead weight and broken promises.** Sweep for: TODO/FIXME/XXX
comments (each becomes a ledger item: do it, schedule it, or delete it
with a ruling); dead code (unreachable branches, orphaned functions);
UI strings promising what the code no longer does; settings keys
written but never read. The Phase-1/2 pattern: the smallest honest
inventory first, then fixes in ranked order.

**T7. Test the untested.** Inventory which panes/flows have rendered-
output pins and which run on faith. Every P0-capable path (save,
export, import, approval rulings, catalog propagation) gets a pin or a
battery case. The W3 journey scripts become automated where the
harness allows. Coverage of the ENGINES is already strong (batteries);
coverage of the CHROME is the gap to close.

**T8. Design debt, finished.** The remaining design items from the
TODO: the full Word-style menu reorganization (9l part 2 — Edit menu
exists; the rest of the menu grammar finishes now); card format
refinements Adam has ruled on; any Phase-1/2 finding marked OPEN in
either ledger. The ribbon frequency reorder stays gated on a week of
usage-ledger data — do not guess it early (W10's whole point).

**T9. The docs tell the truth.** README/help/tutorials verified against
the actual UI after all this change: every named menu, button, and
shortcut must exist as documented; the Help indexer's chapters load;
the manual's screenshots (when 9i lands) come from the blessed set.

## PART III — ORDER AND EXIT

**§10. Wave order** (highest certainty of real defects first):
T1 warnings → T2 sanitizers → T3 fuzz → T6 dead weight → T7 test gaps →
T4 performance → T5 map (+opportunistic extraction) → T8 design debt →
T9 docs. P0s found anywhere jump the queue. The §10 adversarial reviewer
runs on every wave that changed behavior.

**§11. Exit criteria.** Warnings zero-or-ruled; sanitized battery clean;
fuzz corpus banked with zero open divergences; TODO inventory empty-or-
ruled; every P0 path pinned; perf floors pinned; the monolith mapped;
menu reorg shipped; docs verified. Close with a final walkthrough entry
in the ledger and a memory bank. The engine then idles — it re-arms
whenever Adam says so, and the ribbon-reorder thread wakes when the
usage data is a week old.
