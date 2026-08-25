# Architecture decision record

*Diamond Cutter Translation Tool · opened 2026-08-25 for the P2 gap
"No architecture decision log". One entry per decision that would
otherwise be re-litigated by whoever arrives next. Newest last, because
the order is the argument.*

**What belongs here.** A decision, the situation that forced it, and
the consequence — including the bad half. An ADR that records only
upsides is a brochure. Several entries below are back-dated from
`CLAUDE.md`, the roadmap and the commit record; where a date is
inferred rather than documented it says so.

---

## ADR-001 · C++20 + Qt 6, not Swift
**2026-08-06 · Adam · superseded the roadmap's "Swift ports" wording**

Qt 6 was chosen for proper Tibetan complex-script shaping through
bundled HarfBuzz, and because it gives Windows and Linux for the input
centres without a rewrite. `allcore` is a C++20 static library with no
UI dependency; the Qt layer only calls it.

*Consequence, good:* everything testable lives in `allcore`, which is
why the battery can be thorough without driving a GUI.
*Consequence, bad:* Qt is LGPL-3.0 and pulls ~34 upstream projects into
the bundle, which became BUILD-2 and half of this week's licensing
work. Qt also brings a QML/Quick surface the app never uses but must
still sign and attribute.

## ADR-002 · Engines are PORTED from the Python originals, never reconstructed from spec
**2026-08-06 · Adam · inviolable rule 2**

The Python engines in `Handoff/tools/` remain the canonical oracles. A
C++ port must be proved against its original's validation battery.

*Why:* a port can be proven against its source; a reimplementation can
only be proven against somebody's memory.
*Consequence:* full-dictionary parity numbers exist (100.000% on 35,211
headwords for unicode→wylie) that no reimplementation could have
claimed. The cost is that the Python must be kept.

## ADR-003 · `hgm_gloss` is binding; everything else is labelled reference
**2026-08-06 · Adam · inviolable rule 1**

Geshe Michael Roach's English is the dictionary. Hopkins, 84000, Lokesh
Chandra, Monlam and machine output are comparanda, and must *look* it.
Machine work may MATCH his English from evidence; it may never COMPOSE
it.

*Consequence:* this is the rule the whole product is shaped around, and
almost every serious defect found this week was a violation of it in
some costume — an untiered gloss under "HGM has:", a display cap
printed as a corpus total, garbage from a heap overread surfaced as
recognised Tibetan.

## ADR-004 · The deterministic core stays offline
**2026-08-06 · Adam**

Dictionary lookup, conversion, segmentation, phonetics, search and OCR
run locally. Network use is a user-initiated convenience layer, clearly
labelled.

*Consequence:* the app works at an input centre with no connection,
and the privacy statement can be short and true. The AI back-check is
the single exception and is inert without a key.

## ADR-005 · One `app/main.cpp` — and, from 2026-08-25, no longer
**~2026-08-06 (inferred), reversed 2026-08-25**

The UI was deliberately one file. That was defensible early: one place
to look, no header churn while the shape was still moving.

*Consequence, bad, and now measured:* 39,450 lines, over half the
codebase — and that number went stale three times in one day while
documents about it were being written, which is the consequence
restating itself — and `app/` contained **no header at all** — so there was no
pattern for putting anything anywhere else. Patches miss anchors; two
remediation tracks had to be warned off colliding in it.
*Reversal:* staged extraction, one unit per press, each proven by the
existing battery. `ux_tokens.h` and `textreport.h` are the first two.
Not a rewrite. See RISK_REGISTER R7.

## ADR-006 · The press stages FROM a payload manifest, and is gated on it
**2026-08-24 · after SQA BUILD-4/BUILD-5**

`docs/distribution/PAYLOAD_MANIFEST.txt` carries one row per shipped
path with a licence and a notices anchor. The press stages from it,
prunes to it, and fails on anything unmanifested.

*Why:* the previous gate was a keyword match that never opened the
payload, and 1,926 THL catalogue records shipped in every DMG while the
notices said that payload was not redistributed.
*Consequence:* 406 MB leaves each DMG; a market press refuses to build
while any row's terms are unresolved. Adding data to the product now
costs a manifest row, deliberately.

## ADR-007 · A pin does not count until a mutation proves it can fail
**2026-08-24 · after SQA TEST-1**

`tools/mutate.py` plants a named bug and REQUIRES the battery to go
red; green is the failure condition. `tools/mutation_sweep.json` holds
the committed set with expected verdicts and reports drift.

*Why:* 13 of 21 re-injected defects survived the full battery, and the
2026-08-22 baseline mis-measured three stores by probing above the
stream buffer — the measurement manufactured a pass.
*Consequence:* it caught four of my own tests that could not fail,
within two days of being written. It also costs a rebuild per mutation,
so the sweep is periodic rather than part of the gate.

## ADR-008 · Diagnostics are saved, never sent
**2026-08-24**

Help → Save Diagnostic Report writes a file the user hands over
deliberately. There is no crash reporter and no telemetry.

*Why:* `docs/PRIVACY.md` states this app has no telemetry, and that was
verified before it was written. A reporter would have quietly made a
published statement false.
*Consequence:* file paths are reduced to extension and count, because a
text's title can be the sensitive part here. The cost is real: nothing
captures the moment of a crash, and there is still no crash handler at
all.

## ADR-009 · The SBOM covers data layers, not just binaries
**2026-08-24**

`tools/sbom.py` emits CycloneDX at press time including every row of
the payload manifest with its licence and status.

*Why:* the licensing finding that cost this project a day was DATA, not
dylibs. A binaries-only SBOM would omit the half that has already gone
wrong.
*Consequence:* it reports 24 of 115 components with OPEN terms — not
new, but counted every press instead of discovered in an audit.

## ADR-010 · Autonomous repair works on a branch and never merges itself
**2026-08-24 · Adam approved automation; the shape was mine**

The nightly repair task fixes one finding, on `auto/<date>-<slug>`,
gated on a **mutation-verified** pin. It may not touch `main`, press,
push, or rewrite history.

*Why:* a green gate had already proven insufficient — three of my own
pins passed while detecting nothing. The acceptance test is therefore
"the harness says MUTANT KILLED", not "the suite is green".
*Consequence, and it cut both ways:* on its first run it solved a
problem I had given up on (TERMINOLOGY-NESTING, by pinning the
invariant that makes the mutant equivalent). I then wrongly blamed it
for a flakiness it did not cause, using an uncontrolled comparison. The
leash worked; the reviewer was the one who was wrong.

## ADR-011 · Claims in documents are verified against the code before they ship
**2026-08-25 · implicit for weeks, made explicit after it failed twice in one day**

Every number in the Joel briefing, the risk register and the
accessibility audit was recomputed from the repository before commit.

*Why:* the 22 August technical brief contained three claims that were
false when written, and on 2026-08-25 I published an accessibility
audit whose headline claim — that tier badges have no non-visual
equivalent — was wrong, because I inferred it from a symbol count
instead of checking how badges are drawn.
*Consequence:* documents carry their corrections inline rather than
being silently edited, since a reader may hold the earlier version.
Two documents committed this week already do.

---

*If a decision here is wrong, argue with the entry rather than working
around it. An ADR nobody contradicts is one nobody read.*
