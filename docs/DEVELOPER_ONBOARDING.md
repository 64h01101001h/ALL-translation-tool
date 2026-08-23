# Developer onboarding

The short version of how to build, test, and not break the rules.
Deeper companions: `README.md` (pane guide),
`docs/TECHNICAL_OVERVIEW.md` (under the hood),
`DIAMOND_CUTTER_TRANSLATION_TOOL_ROADMAP.md` (master plan), `CLAUDE.md`
(project rules).

## Prerequisites

**An Apple Silicon Mac, macOS 26 or later** (`ALL_MACOS_MIN` in the root
`CMakeLists.txt` is the floor; the press refuses to ship a bundle that
does not declare it).

    brew bundle --file=Brewfile  # the declared build dependencies,
                                 # and it writes Brewfile.lock.json
    brew install pandoc          # optional: .docx doc generation

`Brewfile` is the single list; `brew bundle check --file=Brewfile`
answers "have I got them all?".

Python 3 (system is fine) drives the data harnesses in `tools/`;
nothing Python runs at app runtime.

## Clone, build, run

    cmake -B cmake-build -DCMAKE_PREFIX_PATH="$(brew --prefix qt)"
    cmake --build cmake-build -j

The spine database must exist before the tests or the app will find
nothing: `python3 tools/build_spine.py` (release package + corpus →
`build/hgm_spine_v27_2.db`).

## Prove the build

    ctest --test-dir cmake-build             # 55 suites (2026-08-20)
    ./cmake-build/app/DiamondCutterTranslationTool.app/Contents/MacOS/\
DiamondCutterTranslationTool --selftest                # 225 checks

From a **clean checkout** run `ctest --test-dir cmake-build -LE
fixture` instead: 37 of the suites read data git does not track, and
without it they are registered as ctest SKIPs that name the missing
path. `docs/FIXTURES.md` lists every one and how to produce it. A skip
is never a pass, and the press refuses to cut a release if anything
skipped.
The ctest battery is the trust anchor: engine ports at
full-dictionary parity against their Python oracles, data layers,
stores, mutation paths. The selftest constructs the real panes
against the real spine offscreen and asserts on their actual state;
it prints per-check PASS/FAIL and exits nonzero on any failure.

## The four inviolable rules

1. **hgm_gloss is binding.** Only HGM's English is the dictionary;
   Hopkins/{LC}/Monlam/verbs-DB/etc. are labeled reference layers
   and never feed ranking, parsing, or the dictionary. Machine work
   may MATCH HGM's English from corpus evidence, never COMPOSE it.
2. **Never reconstruct engines from spec.** The Python engines in
   `engines/` (and the data project's `Handoff/tools/`) are the
   canonical oracles. Port from the file; prove the port with the
   source's own validation battery. A port that fails its battery
   does not ship.
3. **Never guess.** Failed conversions surface as `⟨wylie⟩` markers
   and flags, never silent approximations. Below-threshold matches
   are silence, not maybes.
4. **Provenance is sacred.** Generated data carries its source tags
   (`source_wave`, `tibetan_source`, `{LC}`, …); preserve them
   through any schema work.

## The port discipline (rule 2, operationalized)

Every engine port follows the same four steps, in order:

1. **Bank the source** with its license (`third_party/`, or the data
   project's `Handoff/tools`).
2. **Capture oracle outputs from the source itself** — run the
   original over known inputs; save fixtures (TSV/JSON) in-repo.
3. **Port from the file, never the spec.**
4. **Battery = diff against the oracle fixtures**, at every
   intermediate stage where possible, wired into ctest.

Regeneration commands for every reference fixture are in README.md
and each tool's docstring.

## Where things live

    core/           allcore, the C++20 static library: spine access,
                    engine ports, lattice, gofer, all algorithms.
                    One public header per subsystem; batteries in
                    core/tests/*_smoke.cpp
    app/main.cpp    the ENTIRE Qt 6 UI, single file — panes are thin
                    presentation over allcore, no hidden logic
    engines/        canonical Python engines — the oracles; never
                    edit casually
    tools/          Python harnesses: spine/reference builders,
                    extractors, validators, packaging script
    data/           banked reference layers (regenerable; commands
                    documented per tool)
    docs/           design docs, one per subsystem (OVERLAY_PANE,
                    OCR, BDRC_LINKING, TRAINER, PACKAGING, …)
    build/          the compiled spine db (regenerable, large)
    library/        user data — texts, caches, indexes

## The selftest rule

Every UI feature lands with a selftest check. If a pane gains
behavior, `--selftest` gains an assertion that exercises it against
the real spine (see `docs/TEST_PLAN.md` for the tier map: core
battery vs selftest vs manual-by-nature). The automation queue is
kept empty — everything automatable is automated.
