# Release readiness — v0.27.2 app, verified 2026-08-09

*Target: release next week (Adam, 2026-08-09). This is the one
document to work from. Everything automated has been run fresh today
against commit `8a2da1a`; results below. What remains is the human
half — the checklist at the bottom is the release gate you hold.*

## Verification results (all run 2026-08-09, all green)

| Layer | What it proves | Result |
|---|---|---|
| C++ battery, 38 suites (`ctest`) | every engine port, data layer, store, and mutation path | **38/38 PASS** (64 s) |
| App selftest, 54 checks (suite 38) | the real panes against the real spine: features, hostile-input survival, engine speed | **54/54 PASS** |
| Python canonical batteries (`tools/validate_engines.py`) | the C++ ports' oracles still agree with the canonical engines | **PASS** — ewts→unicode 99.03% variant-aware on all 26,318 ground-truth pairs (reference 98.88%); Sanskrit standard-doc examples 7/7 |
| Pronunciation battery | engine vs 1,308 card-attested pronunciations | 68.4% exact — **expected, not drift**: card-governed segmentation overrides are banked as evidence with the engine deliberately untouched (documented in the harness output) |
| Release-package gate (`tools/validate_release.py`) | the data release: structure, provenance, no-shrink, engine spot-battery, corpus integrity | **PASS** |
| Packaging pipeline (`tools/package_macos.sh`) | gate → Release build → battery → bundle fixup → sign → staged launch test → DMG | **PASS**; DMG re-verified today: mounts, contents right, signature valid |
| Performance | full canonical volume (344,940 tokens) through the engine | **424 ms** (15 s regression ceiling enforced) |

**The artifact: `dist/ALL-Translation-Tool-0.27.2.dmg` (183 MB), byte-current with commit `8a2da1a`.** App + "ALL Tool Data" (424 MB) + README. Ad-hoc signed.

## The human release gate — the five-step checklist (docs/TEST_PLAN.md)

These need eyes; nothing here is automatable. Est. 45–60 minutes total.

1. **Reading pass** — open a Kangyur volume in the Overlay, script
   mode: shading calm, phrases banded, no gaps; click three words —
   cards complete (tier, concordance, references, badges).
2. **Woodblock** — BDRC follow-along on a Kangyur text: pages turn
   with the cursor; word-locate box lands on the word (task #31 —
   the oldest outstanding check).
3. **Input pane** — open a real scan: follow tracks typing; OCR
   pre-fill banner present; compare flags a planted difference.
4. **Mixed Nuts export** on a short text — format matches the
   demonstrated workflow.
5. **Clean-Mac install** — the DMG on a machine that never had the
   dev tools (ideally no Homebrew): drag both, right-click → Open,
   data found, five random panes exercised. *This is the single most
   important step: it is the only test of the experience your team
   will actually have.*

## Release-week runbook

1. Run the checklist above (any failures → back to me, they gate).
2. If any code changes result: `bash tools/package_macos.sh`
   re-presses the DMG through the full gated pipeline (~10 min).
3. Distribute the DMG (Dropbox is fine — it is self-contained).
4. Team setup (each person, two minutes): install per the README →
   Propose tab → name + shared proposals folder (the team Dropbox
   folder; copy `data/proposals/proposals.tsv` there to carry the
   205-item prenasal queue). Authorities also tick the admin box.
5. Send the overview: the artifact link or
   `docs/APP_FUNCTIONS_AND_DESIGN.docx`, and the pane gallery
   (`docs/PANE_GALLERY.docx`) for the visual tour.

## Known limitations to state honestly at release

- **Ad-hoc signature**: first launch is right-click → Open. Fix
  when ALL provides an Apple Developer ID (one flag in the script,
  plus notarization).
- **Analysis pane AI report and BDRC page images need a connection**
  (the only two online features); everything else is offline.
- **Scan pane** requires the OCR model download (instructions
  in-pane); OCR output is review material, never trusted text.
- **Pronunciation engine** renders GMR-convention phonetics; the 205
  machine-derived colloquial forms are PENDING the authority's
  ruling (the seeded Approval queue) and display as derived until
  approved.
- **Provisional-tier glosses** are visibly provisional everywhere —
  that is honesty, not a defect; say so in the announcement.
- **macOS only** this release; Windows/Linux are packaging work
  awaiting machines (docs/PACKAGING.md).

## If something breaks in the field

Every installation can self-diagnose: run the app binary with
`--selftest` from Terminal — 54 checks against the installed data,
per-check PASS/FAIL. Ask the user to send the output; it localizes
the failure layer immediately.


## Full-library open sweep (2026-08-11)

Every text in the shipped library — all 2,716 files of ACIP
Release 6 — opened through the real Overlay pipeline (tokenize,
match, convert, spellcheck, display build) via `--openprobe`,
90-second timeout per file. **Result: 2,716/2,716 opened, zero
failures, zero timeouts; slowest file 1.78 s** (R0007A1_E.TXT).
The release ships with the statement: every text in the library
opens.