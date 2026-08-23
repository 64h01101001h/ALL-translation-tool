# Packaging & distribution

*Leadership recommendation #7. macOS half done; Windows/Linux is the
second half (Qt 6 + CMake keep the code cross-platform-clean; those
builds need machines or CI runners of those platforms).*

## macOS DMG (the team's copy)

```
bash tools/package_macos.sh          # full: gate → build → test → dmg
bash tools/package_macos.sh --skip-build   # repackage an existing build
```

The script refuses to package anything that fails the release gate
(`tools/validate_release.py`) or the ctest battery — a bad build or a
bad data drop cannot become a DMG. Since 2026-08-23 it also refuses a
battery that did not run in full: a suite whose untracked fixture is
absent is registered as a ctest SKIP (`cmake/AllFixtureTests.cmake`,
`docs/FIXTURES.md`), and any skip at press time stops the press.

Product: `dist/Diamond-Cutter-Translation-Tool-<version>.dmg` containing

- **Diamond Cutter Translation Tool.app** — Release build, Qt frameworks bundled
  by `macdeployqt`, ad-hoc signed.
- **Diamond Cutter Tool Data/** — the runtime payload: the spine and reference
  databases plus the data folders the panes read (fonts, honorifics,
  colloquial pronunciations, abbreviations, extracted layers, botok,
  spellcheck, SOAS POS, Whitney, candidate alignments) and an empty
  `library/` for the user's texts.
- **README.txt** — the three-step install for a non-technical user,
  opening with the system requirement: **Apple Silicon Mac, macOS 26 or
  later** (step 5b measures both from the built binary and refuses to
  continue if the bundle's `LSMinimumSystemVersion` is empty).
- **BUILD_MANIFEST.txt / BUILD_MANIFEST.json** — what this copy was
  built from (step 6d, `tools/build_manifest.py`): toolchain versions,
  git commit and tree state, architecture and macOS floor, and every
  bundled Mach-O with its Homebrew formula, version, SPDX licence and
  sha256, plus the checksum of the shipped spine. This is the file that
  answers "which OpenSSL did release X carry?".

## How the app finds its data (findDataRoot)

In order: a folder remembered in QSettings (`app/dataRoot`) → the
development checkout (walk up from the executable looking for
`build/hgm_spine_v27_2.db`) → an **"Diamond Cutter Tool Data" folder beside the
.app** (the DMG layout — drag both anywhere together) → Application
Support → a one-time folder picker whose choice is remembered.
Nothing is guessed: if the user cancels the picker the app reports the
missing spine exactly as before.

## The macdeployqt gaps (why step 4b exists)

The first pressed DMG launched and died with `OMP Error #15` (two
OpenMP runtimes initialized). Traced with `DYLD_PRINT_LIBRARIES` /
`DYLD_PRINT_SEARCHING`: `macdeployqt` copies dylibs and rewrites many
references, but leaves two escape hatches to Homebrew —

1. **LC_RPATH entries survive.** The executable kept
   `/opt/homebrew/lib`, and each bundled OpenCV dylib kept its own
   `/opt/homebrew/Cellar/opencv/...` rpath.
2. **Inter-dylib `@rpath` references are not rewritten.** Bundled
   `libopencv_imgproc` still asked for
   `@rpath/libopencv_geometry.500.dylib`.

Combined effect: the Cellar OpenCV loaded *beside* the bundled copy
(dyld treats different resolved paths as different images), pulled the
Cellar OpenBLAS by absolute path, which pulled the Cellar `libomp` —
while the bundled chain loaded the bundled `libomp`. Two runtimes, one
abort. On a machine *without* Homebrew the same gaps would instead be
missing-dylib failures — the dev machine's Homebrew masked all of it,
which is why the script launch-tests the staged layout (step 6b), not
the dev build.

Step 4b closes both hatches: strip every `/opt/homebrew` rpath from
every Mach-O in the bundle, rewrite `@rpath/lib*` references to
`@executable_path/../Frameworks/…` wherever the library is bundled,
verify none remain (hard fail otherwise), then re-sign.

## Signing status

Ad-hoc signature (`codesign -s -`): the team's first launch is
right-click → Open. When ALL provides an Apple Developer ID, add
`-codesign="Developer ID Application: …"` to the `macdeployqt` step
and a `notarytool submit` step; nothing else changes.

## Windows / Linux (second half, when machines exist)

The codebase is ready: C++20 + Qt 6 + CMake, no mac-only APIs in
`allcore`, SQLite vendored, fonts bundled. The packaging equivalents
are `windeployqt` + an installer (NSIS/WiX) and `linuxdeploy` +
AppImage. The blockers are purely infrastructural: a Windows/Linux
build machine or CI runner, and a round of font-rendering QA on each
platform (HarfBuzz shaping is bundled with Qt, so Tibetan should shape
identically — verify, don't assume).
