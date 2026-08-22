---
name: all-press-ritual
description: Use when packaging, installing, or releasing the Diamond Cutter Translation Tool — the announced press and release rituals, their gates, and the recovery moves when a gate fires
---

# The ALL Press Ritual (house skill #1)

## The law
Presses are ANNOUNCED to Adam, never ambient. The installed app is the
only reality that counts — a change that never pressed never happened.

## The press
    tools/package_macos.sh [team|market]     # default: team
Gates, in order — a press that skips one is a failed press (the
constitution's C2 rule asserts their presence):
1. **Constitution** (`tools/constitution_check.py`) — exit 8 stops it.
2. Release build (zero warnings under -Wall).
3. **Full battery** (ctest — all suites).
4. **Visual gate** (`tools/shot_diff.py` vs `build/blessed_shots`;
   the sweep also runs the geometry inquisition — exit 4 on
   clip/overlap/escape violations).
5. Stage → sign (ad hoc, or ALL_DEV_IDENTITY) → DMG → notarize
   (when the ALL_NOTARY keychain profile exists; skip-with-notice
   otherwise).
6. Install → **relaunch + verify** (never end a press with the app
   down).
**market mode omits** reference.db and the BDRC OCR models
(licensing — docs/SHIP.md is the manifest of record).

## The release
    tools/release.sh <version> [team|market]
Refuses a dirty tree (tracked dirt = code; data corpora are
gitignored). Moves VERSION + CHANGELOG + press + tag + blessed-
baseline snapshot as ONE ritual — and **presses BEFORE tagging**,
auto-unwinding the version commit on a failed press. A tag means
"this exact commit passed every gate and produced a DMG."

## When a gate fires
- Constitution violation → read the rule's message; it cites the
  incident that paid for it. Fix the cause, never annotate around it
  without a ruling.
- Visual gate → if the change is INTENTIONAL, re-bless
  (`tools/bless_shots.sh`) as an explicit act; otherwise you broke a
  pane.
- Battery → fix before pressing; no exceptions.
- Geometry gate (exit 4) → a pane clips/overlaps/escapes at some
  size; the minimum window is the proven-clean floor (820x560) —
  never lower it without re-bisecting.

## Recovery moves (paid-for)
- A press that seems stale on Adam's machine: force-quit + open the
  installed app, then PROVE with `--screenshots` on the INSTALLED
  binary and Read the PNG.
- Half-release (tag without DMG): `git tag -d v<V>` +
  `git reset --hard HEAD~1`, fix, re-run — but the press-before-tag
  ordering makes this historical.
