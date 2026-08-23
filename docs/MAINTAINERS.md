# MAINTAINERS — the operational handbook
*(S5: everything a competent stranger needs to build, test, press, and
release this tool without breaking its promises. The deeper history
lives in the ledgers; this is the working core.)*

## What this is
A macOS Qt 6 desktop suite for Tibetan digital texts: dictionary/corpus
reading (the HGM data project's releases), translation drafting,
cataloging, OCR, teaching links. One app, two parts:
- `core/` — **allcore**, a C++20 static library: SQLite/FTS5 spine
  access, engine ports, stores. No Qt. Everything testable lives here.
- `app/main.cpp` — the whole Qt UI (~33k lines, deliberately one file;
  the class map with line anchors is in docs/TOTAL_QUALITY.md §T5 —
  read it before any large edit).

## The five inviolable rules (CLAUDE.md; they outrank you)
1. hgm_gloss is binding; everything else is reference and must LOOK it.
2. Never reconstruct canonical engines from spec — port from the
   Python file in Handoff/tools and prove with its battery.
3. Never guess: failed conversions are flagged (⟨wylie⟩), not smoothed.
4. Provenance fields are sacred; preserve them through any change.
5. Honesty wording in the UI is load-bearing; never soften it.

## Build · test · run
    cmake -S . -B cmake-build-release -DCMAKE_BUILD_TYPE=Release
    cmake --build cmake-build-release -j8        # zero warnings, -Wall
    (cd cmake-build-release && ctest -j8)        # ALL suites must pass
    #  ... and none may be SKIPPED. 37 of the suites read data git does
    #  not track; without it they register as ctest SKIPs naming the
    #  missing path (docs/FIXTURES.md). A skip is not a pass, tools/
    #  verify.sh puts the count in its success line, and the press stops
    #  if anything skipped. From a clean checkout use `ctest -LE fixture`.
The battery includes: engine batteries (fixtures from canonical
Python), the constitution (process rules as greps — read
tools/constitution_check.py, every rule cites the bug that paid for
it), the honesty sweep (4,592 cards' invariants), perf floors, fuzz,
backup restore drill, and the app selftest with rendered-output pins.

## The press (packaging)
    tools/package_macos.sh [team|market]
Gates, in order: constitution → build → battery → visual regression
(tools/shot_diff.py vs build/blessed_shots — re-bless with
tools/bless_shots.sh ONLY for intentional change) → stage → sign
(ad hoc, or ALL_DEV_IDENTITY when set) → DMG → notarize (when the
ALL_NOTARY keychain profile exists) → install → relaunch + verify.
**market mode omits** reference.db and the OCR models (licensing —
docs/SHIP.md). Presses are announced to Adam, never ambient.

## Backups (the work must exist in more than one place)
    tools/backup.sh              # external drive + Dropbox + GitHub
    tools/backup.sh --github     # cloud only (after `gh auth login`)
Makes a FILTERED mirror — every commit, minus the regenerable DMG
binaries that made the repo 7GiB (220MB filtered) — and refuses to
ship if the filter ate the history. Proven by restore drill: cloning
any copy yields the full working tree. The working repo is never
touched; only the backup copies are rewritten.

## Releases
    tools/release.sh <version> [team|market]
Moves VERSION + CHANGELOG + git tag + press + blessed-baseline
snapshot as one ritual. Refuses a dirty tree.

## Editing app/main.cpp without disasters (paid-for lessons)
- Anchor block edits on EXACT unique strings; assert count==1; a
  failed assert mid-script loses every earlier edit in that script.
- Never use a far-away end anchor (half the file once vanished).
- New regex literals: raw strings only (constitution R1).
- New dialogs: guard with g_harnessRun or they hang the headless
  modes (R3 census will stop you).
- New battery drills: clean up at every entry point; run twice (R6).
- To re-render the Overlay card use refreshCard(), never onClick().
- The harness flag list exists ONCE; test g_harnessRun, never re-parse
  argv (R2).

## Data
- The spine db + runtime layers ship in "Diamond Cutter Tool Data" beside the
  app; the staging list in package_macos.sh IS the distribution
  manifest, and every staged dir must be credited in
  docs/distribution/OPEN_SOURCE_NOTICES.md (constitution L2).
- Human judgments (proposals.tsv, CATALOG_TEAM.tsv, glossaries,
  alignment links) outrank computed data: they get startup backups
  (Application Support/backups, keep 14) and conflict-safe saves
  (merge-save; Dropbox conflicted copies absorbed + bannered).
- New data releases: Maintenance → Import data release (atomic:
  stage-then-swap).

## Where things are decided
- docs/*_PROMPT.md — the eight master prompts (surface honesty, lived
  experience, quality, stewardship, fidelity, shipwright, closer,
  burndown). Their ledgers (UX_AUDIT.md, UX_AUDIT_2.md,
  TOTAL_QUALITY.md, STEWARDSHIP.md, FIDELITY.md, SHIP.md, CLOSER.md)
  record every finding, ruling, and proof.
- docs/CLOSER.md is the single open-work backlog. An item not on it
  does not exist.
- Externally gated: see CLOSER.md's WAITING rows (licensing lines,
  Developer ID, GMR sign-offs, pilot).

## The daily habit
Hunt → fix → PROVE (a pin, a battery case, a measurement) → commit
with the battery green → press in announced batches. If you learned a
lesson, make it a constitution rule. If you're about to soften an
honesty label to make a screen prettier — stop, and reread rule 1.
