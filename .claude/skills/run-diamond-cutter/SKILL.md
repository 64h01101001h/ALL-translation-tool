---
name: run-diamond-cutter
description: Build, run, screenshot, and drive the Diamond Cutter Translation Tool (the ALL/Geshe Michael Roach Tibetan translation desktop app) headlessly. Use when asked to run, start, launch, build, test, verify, screenshot, sweep, or reproduce a bug in this app, or to call allcore engine code directly without the GUI.
---

# Running the Diamond Cutter Translation Tool

A Qt 6 Widgets desktop app (C++20) over a static core library
(`allcore`) and an OCR library (`allocr`). 37,609 lines in
`app/main.cpp`, 22 panes, a SQLite/FTS5 spine. (Counted, not
estimated — `wc -l app/main.cpp`.)

**There is no web surface, so there is no `chromium-cli` to borrow.**
What the binary does have is a set of headless modes built into it, and
a static library you can link a throwaway probe against. Both are
wrapped by:

```
.claude/skills/run-diamond-cutter/driver.py
```

All paths below are relative to the repo root. Run everything from
there. macOS only — verified on darwin 25.5.0, arm64.

## Prerequisites

Qt 6 (Homebrew), CMake, Apple clang, python3. The authoritative record
of what this build actually used is the CMake cache, not a package
manager:

```bash
grep -E "^(CMAKE_CXX_COMPILER|Qt6_DIR):" cmake-build-release/CMakeCache.txt
cmake --version | head -1
clang++ --version | head -1
```

On Adam's machine that prints:

```
CMAKE_CXX_COMPILER:FILEPATH=/usr/bin/c++
Qt6_DIR:PATH=/opt/homebrew/lib/cmake/Qt6
cmake version 4.4.2
Apple clang version 21.0.0 (clang-2100.1.1.101)
```

Do not probe this with `brew list`: there are two Homebrew prefixes on
this machine (`/opt/homebrew` and `/usr/local/Homebrew`), and under a
stripped PATH the Intel one wins and dies with an unrelated Ruby
`MacOSVersionError`. The cache tells you what the compiler actually
found.

## Build

```bash
.claude/skills/run-diamond-cutter/driver.py build
```

First configure + full build is minutes; incremental is seconds.

## Run — the agent path

**Everything here is headless.** `QT_QPA_PLATFORM=offscreen` is set by
the driver; you never need a display.

```bash
.claude/skills/run-diamond-cutter/driver.py paths      # what resolved where
.claude/skills/run-diamond-cutter/driver.py verify     # build + 73 suites + constitution
.claude/skills/run-diamond-cutter/driver.py selftest   # 290 rendered-output pins
.claude/skills/run-diamond-cutter/driver.py shots /tmp/dctt-shots
.claude/skills/run-diamond-cutter/driver.py sweep              # Overlay
.claude/skills/run-diamond-cutter/driver.py sweep Catalog      # a named pane
.claude/skills/run-diamond-cutter/driver.py open "library/sungbum/Sungbum/BLA BRGYUD_LAMA LINEAGE TREES/S00237M META.TXT"
```

`shots` writes 22 PNGs, one per pane, in ~10s:

```
01-overlay.png  02-library.png  03-files.png  …  22-approval.png
```

`open` is the closest thing to a real user flow — it opens a document
in the Overlay and prints what the segmenter made of it:

```
[probe] openFile completed in 13 ms
[probe] hint: Sungbum Collection · text 00237 · L-level + automated typo checks
18 tokens · 3 spans · 3 entries · 15 spelling flag(s) · 0 particle-agreement flag(s)
```

`sweep` clicks every reachable control in a pane and asserts the app is
still coherent:

```
SWEEP COMPLETE: Overlay — 23 control(s) exercised, app coherent
```

### Direct invocation — usually the one you want

Most changes here touch `allcore` (the spine, the ported engines, the
library index) or a render function that reads from it. Booting 22
panes to ask one question is the slow way round. `probe` compiles a
C++ snippet against `liballcore.a` and runs it:

```bash
.claude/skills/run-diamond-cutter/driver.py probe <<'EOF'
#include "allcore/spine.h"
int main() {
    allcore::Spine s("build/hgm_spine_v27_2.db");
    auto es = s.lookup("bsod nams");
    std::printf("entries=%zu\n", es.size());
    if (!es.empty()) {
        std::printf("tier=%s  glosses=%zu  provisional=%d\n",
                    es.front().tier.c_str(), es.front().hgm_gloss.size(),
                    (int)es.front().provisional());
        for (const auto& g : es.front().hgm_gloss)
            std::printf("  gloss: %s\n", g.c_str());
    }
    std::printf("attestations of \"sems can\": %ld\n",
                s.corpusCount("\"sems can\""));
    return 0;
}
EOF
```

Real output:

```
entries=1
tier=curated  glosses=3  provisional=0
  gloss: merit
  gloss: good deeds
  gloss: goodness (prayer register)
attestations of "sems can": 1566
```

`<cstdio> <chrono> <string> <vector>` are pre-included. A snippet needs
an `int main`. This is how a search was measured at 3,310 ms / 40.7 MB
that had previously been 346,116 ms / 18.0 GB.

## Run — the human path

`open cmake-build-release/app/DiamondCutterTranslationTool.app` puts a
window on Adam's screen and blocks. **Useless from an agent session**,
and it steals focus on his machine. Use the driver.

To put a real build in front of him — a *release* act, not a run act —
see the `all-press-ritual` skill (`tools/package_macos.sh team`), which
installs to `/Applications` and relaunches.

## Gotchas

- **The build product is a `.app` bundle, not a bare executable.**
  `cmake-build-release/app/DiamondCutterTranslationTool` does not
  exist; the binary is at
  `…/DiamondCutterTranslationTool.app/Contents/MacOS/DiamondCutterTranslationTool`.
  The driver resolves this; hand-rolled commands hit
  "no such file or directory".
- **`ctest` will happily test a stale binary after a failed build.**
  `cmake --build … | grep error ; ctest` reported "72/72 tests passed"
  over a *failed* build twice in one day, because grep's exit status is
  not the build's. Use `driver.py verify` (wraps `tools/verify.sh`),
  which stops at the first failing step.
- **Restoring a file with `cp` can skip the rebuild.** `cp` gives the
  restored file an mtime older than the object built from the previous
  version, so `make` skips it and you keep running the old code.
  `touch` the file after any `cp`-based revert. Learned while
  mutation-testing.
- **`sweep Catalog` reports only 2 controls of ~90.** Not a bug:
  `CatalogPane::gated_` disables its action buttons until sign-in, and
  the sweep only clicks enabled widgets. Same shape wherever a pane
  gates on login.
- **`shots` captures empty state.** No document is loaded, so the
  Overlay screenshot shows an empty editor. There is no mode that both
  opens a document *and* screenshots — `--openprobe` returns before the
  screenshot branch is reached. If you need a picture of a populated
  card, add the capability or read the card's HTML from a selftest pin.
- **`--openprobe` is not in the harness-guard list.** Every other
  headless mode sets `g_harnessRun`, which arms a global dialog reaper.
  `--openprobe` does not, so a flow that raises a modal will hang it.
  Fine for opening a plain text.
- **The app finds its data by walking up from the executable**, so the
  build-tree binary uses the repo's own `data/`, `library/` and
  `build/hgm_spine_v27_2.db`. Run from the repo root or it will not
  find them.
- **Feed `probe` with a quoted heredoc, never `echo`.** `echo '...\n...'`
  under zsh interprets the escapes and hands the compiler mangled
  source ("2 warnings and 4 errors generated"). `<<'EOF'` (quoted
  delimiter) passes the snippet through untouched.
- **A sweep never writes real state** — `g_proposalsDir` is redirected
  to a temp dir first, so it cannot file proposals into the team
  channel.

## Troubleshooting

| Symptom | Fix |
|---|---|
| `no such file or directory: cmake-build-release/app/DiamondCutterTranslationTool` | It is a `.app` bundle — use `driver.py`, or the `Contents/MacOS/` path. |
| `no built binary at: …` from the driver | `driver.py build` |
| `no liballcore.a — run: driver.py build` | Same; `probe` links the static lib. |
| `command not found: timeout` | macOS has no `timeout`. Use `gtimeout` (coreutils) or just let it run — `sweep` and `shots` finish in seconds. |
| ctest green right after a compiler error scrolled past | You tested the previous binary. Re-run `driver.py verify`. |
| `constitution: 1 violation(s) … modal call sites grew N -> N+1` | A new `QMessageBox`/`QInputDialog` was added. Review it for harness reachability, then bump `tools/constitution_baseline.txt` **in the same commit** — that is the rule, not a formality. |

## Test

`driver.py verify` is the gate: build, then 73 ctest suites, then
`tools/constitution_check.py`. Individual suites:

```bash
(cd cmake-build-release && ctest -R libindex --output-on-failure)
(cd cmake-build-release && ctest -N | tail -1)     # Total Tests: 73
```

A test that cannot fail is this project's known weak spot — the
2026-08-22 SQA assessment found five write-failure mutants surviving a
full green suite. **Before trusting a new pin, plant the bug and watch
it go red.**
