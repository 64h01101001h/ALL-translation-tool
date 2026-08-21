# MASTER PROMPT 5 — THE FIDELITY ENGINE
*(successor to Prompt 1: surface honesty · Prompt 2: lived experience ·
Prompt 3: total quality · Prompt 4: stewardship. Each ring audited the
VESSEL. This ring audits the CARGO — whether the scholarship itself is
rendered faithfully, at scale, provably — and it COMPILES THE LESSONS:
every hard-won process lesson from rings 1–4 becomes a mechanical gate,
so the mistakes we already paid for become impossible to repeat. The
open remainders of rings 3–4 [S2 backup, T3 fuzz, S4 security, T7 pins,
S3 release, T4 perf, T8 menus, S5 handbook, S6 loop, T9 docs] continue
here as track R, in that order, under this prompt's gates.)*

## PART I — STANCE

**§1. The cargo outranks the vessel.** A flawless app that shows the
wrong gloss is worse than a crashing one — the crash is honest. This
ring proves, mechanically and at scale, that what renders is what the
data licenses: the right equivalent, in the right tier, wearing the
right badge, with the right warnings, for thousands of entries — not
the handful a human spot-checked.

**§2. Lessons become law.** Four rings produced a lesson-class list
paid for in real bugs. A lesson that lives in a ledger is a hope; a
lesson that lives in a GATE is a guarantee. This ring builds the
constitution checker — a battery suite that greps, counts, and asserts
the process rules — and every future commit passes it or explains
itself.

**§3. Method inherited, unchanged.** HUNT → FIX → PROVE → PRESS;
sabotage; §10 adversarial review for behavior changes; §11 visual gate;
batteries green (run NEW drills twice); announced presses; ledger
docs/FIDELITY.md; memory banked per wave. The honesty rules of
CLAUDE.md outrank this document.

## PART II — TRACK F: FIDELITY OF THE CARGO

**F1. The honesty invariants, swept at scale.** The rendered card makes
promises the code was written to keep — now prove them over the WHOLE
dictionary, not exemplars. An automated sweep (selftest-side) renders
cards for a large stratified sample (every tier, every zone, honorifics,
register-warning terms, no-entry falls) and asserts INVARIANTS on the
HTML, not pixels:
- a provisional-tier gloss NEVER renders without its provisional marking;
- every evidence zone carries its epistemic badge, and reference layers
  never appear above the HGM zone;
- every capped list shows its true "N of M";
- register-warning terms (bsod nams class) always carry the warning;
- machine-derived content never renders unlabeled.
Violations are P0 by rule 1. The sweep becomes a permanent suite.

**F2. Cross-engine coherence, whole-dictionary.** The engines agree
with their canonical Python (batteries) — do they agree with EACH
OTHER? Diff-test at dictionary scale: wylie → unicode → wylie identity
classes; pronunciation of the wylie vs pronunciation recorded in the
release; ACIP → EWTS → ACIP; Sanskrit chain round trips. Divergences
classified: known-benign classes get documented fixtures; unknown
classes are P1 hunts (the Python oracle arbitrates — rule 2).

**F3. The links tell the truth.** Sampled verification of outbound
claims: teaching links (does the timecoded URL's video id exist in the
caption corpus the index was built from?), Das/Jäschke page references
(within the scanned page range?), 84000/BDRC link FORMATS (against the
verified-live formats banked on 2026-08-07/14 — no live network in the
battery). Broken-claim classes get fixed at the index builder, not
patched per row.

**F4. Tier arithmetic.** The counts the tool states about itself —
12,004 HGM-glossed, tier populations, corpus segment totals, teaching
index sizes — are recomputed from the loaded data at selftest time and
pinned. A release import that silently shifts a population fails the
battery instead of shipping a wrong About screen.

## PART III — TRACK C: THE CONSTITUTION CHECKER

**C1. Build tools/constitution_check.py as a ctest suite.** Mechanical
rules, each born from a paid-for bug, each with the incident cited in
its message:
- R1 regex-literal rule: no QRegularExpression built from a plain
  string containing collapsed-escape patterns (the broken 84000
  entity-link); raw strings or doubled escapes only.
- R2 one-flag-list rule: the harness mode list (--selftest/--sweep/…)
  is defined in exactly ONE place (the findDataRoot divergence).
- R3 dialog-guard rule: no QMessageBox/QDialog::exec added in paths
  reachable under harness without a g_harnessRun guard in scope
  (the --survey hang) — enforced as a count-and-review gate.
- R4 forbidden-ink rule: the pre-W5 failing hex literals (#9C948A,
  #8A8A8A as text ink, #999…) may not reappear in new styles.
- R5 truncate-save rule: no std::ios::trunc rewrite of a *.tsv in the
  shared-store directories outside the merge-save path (S1-01).
- R6 fixture-hygiene rule: battery fixtures that create files remove
  them (drills run twice in CI order to prove it).
The checker's own false-positive protocol: a ruled exception is an
inline annotation naming the ruling, never a silent skip.

**C2. The checker guards the checkers.** The press script's gates
(batteries, visual, constitution) are themselves asserted present — a
press that silently lost a gate is a failed press.

## PART IV — TRACK R: THE ABSORBED REMAINDERS
S2 rolling backup + restore drill → T3 engine fuzz (corpus banked) →
S4 security posture → T7 chrome pins for P0 paths → S3 release
checklist + Gatekeeper honesty → T4 perf floors → T8 Word-style menu
reorganization (§63 governance; screenshots to Adam before/after) →
S5 docs/MAINTAINERS.md → S6 findings loop → T9 docs-vs-UI truth pass.

## PART V — ORDER AND EXIT
**§10. Order:** C1 (cheap, catches everything after it) → F1 → F4 →
F2 → R:S2 → R:T3 → F3 → R:S4 → R:T7 → R:S3 → R:T4 → R:T8 → R:S5 →
R:S6 → R:T9 → C2. P0s jump everywhere.
**§11. Exit:** the constitution suite green and wired into press+ctest;
the honesty-invariant sweep a permanent suite with zero open
violations; cross-engine divergences all classified; tier arithmetic
pinned; every R item closed per its own ring's criteria; a closing
fidelity letter in the ledger; memory banked. Then the engine idles at
Adam's word — with one standing rule: ANY future ring starts by reading
the five prompts and their ledgers first.
