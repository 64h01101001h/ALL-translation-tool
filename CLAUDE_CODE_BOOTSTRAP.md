# Handing off to Claude Code — bootstrap guide

## 1. Recommended folder setup
Create one dev directory for the app and start Claude Code there:

    ~/ALL-translation-tool/
      CLAUDE.md                        ← (provided; Claude Code reads this automatically)
      ALL_TRANSLATION_TOOL_ROADMAP.md  ← copy from HGM folder root
      data/                            ← copy of Handoff/release/ (v0.27.2) + corpus .gz
      engines/                         ← copy of Handoff/tools/ (the canonical Python engines)
      legacy/                          ← optional: symlinks or copies of Gofer, Hypercontext, spellchecker, THL folders

Keeping the app repo separate from `HGM DICTIONARY - TRANSLATION APP` preserves the rule that the app *imports releases* rather than owning the data. Claude Code can also read the original folders in place — grant access when it asks, or launch with additional directories:

    claude --add-dir "$HOME/Desktop/HGM DICTIONARY - TRANSLATION APP" \
           --add-dir "$HOME/ACIP Software Developmeent" \
           --add-dir "$HOME/Tibetan Translation Tool"

(Adding "$HOME/ACIP Software Developmeent" covers Gofer, Hypercontext, all three LokeshChandra projects, and tibetan-spellchecker in one grant. The full itemized list lives in CLAUDE.md → "REQUIRED ACCESS" — Claude Code is instructed to verify every path at session start and prompt you for any that are missing, so you don't need to remember them.)

## 2. First prompt to give Claude Code
> Read CLAUDE.md and ALL_TRANSLATION_TOOL_ROADMAP.md. Start with roadmap item 1: build the SQLite (FTS5) spine — a build script that compiles data/hgm_dictionary_v27_2.json.gz + the parallel corpus into one app database (entries, variants, reverse index, corpus segments, FTS5 indexes on wylie/tibetan/english), plus a small query CLI to smoke-test lookups and NEAR searches. Then run the engine validation batteries described in CLAUDE.md to confirm the environment reproduces them before any porting begins.

## 3. What transfers and what doesn't
- ✅ Everything on disk: HANDOFF.md, README changelogs (v0.10 → v0.27.1), roadmap, engines, release packages, extracts, evidence files.
- ❌ This Cowork conversation and Cowork's project memory — that's what CLAUDE.md replaces. Keep CLAUDE.md updated as decisions are made; it is Claude Code's persistent memory.
- 🔁 Division of labor that works well: Claude Code builds/tests the Xcode app locally (it can compile, run tests, iterate); Cowork sessions continue the data work (ingests, mining, tier review, releases). The release package is the interface between the two.

## 4. Things Claude Code should NOT do (also in CLAUDE.md)
- Reimplement engines from memory instead of porting + battery-validating.
- Promote reference glosses ({LC}/{PH}/{GD}/Hopkins) into HGM equivalents.
- Ship TibetanDictionary.dic or THL dictionary content in a release before the licensing review.
