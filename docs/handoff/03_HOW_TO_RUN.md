# How to run it

Every command here was verified on 2026-09-11. Where a number is quoted, it was
measured, not remembered.

---

## The thing that stops a fresh clone dead

**The heavy data is not in the repository.** `.gitignore` excludes
`build/`, `data/*.gz`, `data/*.json`, `data/*.tsv`, `data/extracted/`,
`progress.db` and `library/`. On a fresh clone these are genuinely absent.

The most important one:

```
build/hgm_spine_v27_2.db     ~249 MB — the SQLite+FTS5 spine
```

Every spine-dependent test takes this path **hardcoded** from
`core/CMakeLists.txt`. It is built by:

```bash
python3 tools/build_spine.py
```

…which needs three files that are themselves **not in the repo** and come from
the external HGM data project:

```
data/hgm_dictionary_v27_2.json.gz
data/full_parallel_corpus_v32.json.gz
data/hgm_reverse_index_v27_2.json
```

**If you do not have those, you cannot run most of the suite.** Say so plainly
rather than reporting a green run of the 64 suites that do not need them.

### One caveat that reads backwards

"Gitignored" does **not** mean "absent". **1,775 tracked files match an ignore
rule** because they were added before the rule existed — including some
reference fixtures under `build/`. Check `git ls-files` before concluding a
file is missing.

---

## The canonical ritual

```bash
tools/verify.sh
```

Build (zero own-code warnings) + full ctest + constitution check. Variants:

```bash
tools/verify.sh --quick        # build + app_selftest only
tools/verify.sh --tests-only   # skip the build (only when the tree is fresh)
ALL_BUILD_DIR=build tools/verify.sh    # against the everyday build/ tree
```

### The build-directory trap

There are two build trees and the docs disagree with the scripts:

| tree | who uses it |
|---|---|
| `cmake-build-release/` | `tools/verify.sh`, `tools/package_macos.sh` — **the real ones** |
| `build/` | the everyday tree most commands in this folder use |
| `cmake-build/` | **stale, do not trust it** — `docs/DEVELOPER_ONBOARDING.md` still points here |

Be explicit about which tree you ran.

---

## Tests

> **Build before you test, or use `tools/verify.sh`, which does.** `ctest`
> runs the suite list **and the binaries** recorded the last time the tree was
> configured. On 2026-09-17 `cmake-build-release` had been configured two days
> earlier: three suites written since were invisible to `ctest -N` — not
> failing, not skipped, absent — and the batteries that did run were examining
> two-day-old objects, so `engines_battery` and `forward_battery` reported
> divergences that vanished the moment the tree was rebuilt. A green `ctest`
> is a claim about what was compiled, not about what is in the repository.

```bash
cmake -S . -B build && cmake --build build -j8   # FIRST — see the note above
ctest --test-dir build -N                 # list all suites
ctest --test-dir build -j8 --output-on-failure
ctest --test-dir build -N -LE fixture     # the clean-checkout gate
ctest --test-dir build -R layer_matches_spine     # one gate by name
```

**How many suites are there?** Ask, do not read:

```bash
python3 tools/count_checks.py             # declared, registered, and the gap
python3 tools/count_checks.py --log <ctest output>   # and judge a run
```

This paragraph used to carry the number, and the number was wrong three times
running — `README.md` and `docs/DEVELOPER_ONBOARDING.md` said 86, `docs/FIXTURES.md`
said "37 of 73", this file said 115, and a digest told leadership 124 while
three of those had never executed anywhere. There is no count written here
now, on purpose. The suites guarded by `if(Python3_FOUND)` and by fixture
availability mean the honest answer depends on how *your* tree is configured,
which is exactly what `count_checks.py` reports. A missing fixture registers an
honest **SKIP naming the file**, never a failure.

### Two gates that are not ctest suites

```bash
python3 tools/constitution_check.py .      # → "constitution: all rules hold"
python3 tools/repo_license_check.py        # every tracked file mapped
```

### app_selftest

The application's own pane-level suite — the densest signal in the project.

```bash
QT_QPA_PLATFORM=offscreen \
  ./build/app/DiamondCutterTranslationTool.app/Contents/MacOS/DiamondCutterTranslationTool \
  --selftest
```

**Two hard-won warnings:**

1. **Never pipe a build through `| head`.** It SIGPIPEs the compiler: a compile
   error goes unseen and you then run a **stale binary** that reports ALL PASS.
   This happened on 2026-09-11. Redirect to a file and grep the file.
2. **The wall-clock gates inside it fail under machine load.** T4b measured
   7556 ms and 6084 ms against its 6000 ms ceiling at load 180 and 91, and
   4882 ms at load 9. **Do not raise an inner ceiling to make a red run green** —
   re-run on a quiet machine. `tools/package_macos.sh` refuses to press above
   1-minute load 12.50 for exactly this reason.

---

## The alignment layer

```bash
python3 tools/build_alignment_layer.py     # rebuilds both artefacts; refuses on any battery failure
```

Counting what is actually there, rather than trusting a ledger:

```bash
# links, and how many have no English exponent
python3 -c "import json;d=json.load(open('data/alignment/alignment_full_v1.json'));print(len(d['links']), sum(1 for l in d['links'] if l['eng'] is None))"

# evidence headwords / pair records
python3 -c "import json;p=json.load(open('data/alignment/alignment_evidence_v1.json'))['pairs'];print(len(p), sum(len(v) for v in p.values()))"

# pages per course
ls data/alignment/pages_c05/*.html | wc -l

# spine segments per course
sqlite3 build/hgm_spine_v27_2.db "SELECT course,COUNT(*) FROM corpus_segments GROUP BY course ORDER BY 1;"
```

Gloss population:

```bash
sqlite3 build/hgm_spine_v27_2.db "SELECT COUNT(*) FROM entries;"
sqlite3 build/hgm_spine_v27_2.db "SELECT ifnull(tier,'(none)'), COUNT(*) FROM entries GROUP BY tier ORDER BY 2 DESC;"
```

---

## The application and the phone

```bash
cmake --build build --target DiamondCutterTranslationTool -j4 > /tmp/b.log 2>&1; grep -E "error:" /tmp/b.log
bash tools/package_macos.sh            # the press: batteries, bundle, DMG, relaunch
./build/core/build_drill_pack build/hgm_spine_v27_2.db ios/DiamondDrills/drills.json 4000 "$(git rev-parse --short HEAD)"
bash tools/ios_deploy.sh               # pack → bundle → phone
```

The press **refuses above 1-minute load 12.50** and unwinds a commit on a
failed press. Wait for background work to finish rather than overriding.

---

## The AI glossary lane

```bash
python3 tools/build_ai_glossary.py --limit 1800   # resume-safe; needs ANTHROPIC_API_KEY
python3 tools/mt_wall_screen.py --glossary --n 6  # "the wall" — screen against his corpus
```

**Currently blocked on exhausted API credits** (since 2026-08-12). That is
Adam's to resolve. Never use or store an API key he pastes.

---

## Committing

Author of record is `vajra108`. **No `Co-Authored-By` trailer**, and no mention
of the tooling in any commit, document or email. Write the message as a record
of what changed and *why*, including what went wrong — the git log here is a
genuine engineering narrative and is worth matching.
