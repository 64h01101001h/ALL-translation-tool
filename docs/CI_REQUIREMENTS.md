# What continuous integration would take

**Status: not stood up.** This page records the cost honestly instead of
leaving "no CI" as an unexplained absence (SQA 2026-08-22, BUILD-6). It
is a plan, not a claim — nothing here runs today.

## What already exists

Two of the three prerequisites landed on 2026-08-23:

* **A dependency declaration.** `Brewfile` lists the direct build
  dependencies; `brew bundle --file=Brewfile` installs them and writes
  `Brewfile.lock.json` with the exact resolved versions. Commit that
  lock file with the release commit.
* **A fixture-independent battery.** `ctest -LE fixture` selects the 36
  of 73 suites that need nothing outside git. That is a runnable gate on
  a clean machine today — the number is derived at configure time, not
  asserted (see `docs/FIXTURES.md`).
* **A record of what was built.** `tools/build_manifest.py` writes
  `BUILD_MANIFEST.{json,txt}` into the DMG: toolchain versions, git
  commit and tree state, architecture and macOS floor, and every bundled
  Mach-O with its Homebrew formula, version, SPDX licence and sha256.
  On a CI runner that same file is the build record.

## What is still in the way

1. **The press is a deployment, not a build.** `tools/package_macos.sh`
   quits the running app, `rsync`s over `/Applications`, and relaunches
   (SQA BUILD-13). A CI runner cannot do any of that, and two presses
   cannot run concurrently. Splitting `package_macos.sh` into
   `package` (produce + verify the DMG, stop) and `install_local.sh`
   (quit / copy / relaunch) is the prerequisite, and is filed
   separately.
2. **The suite is not hermetic.** One `--selftest` run rewrites the
   developer's live `com.ALL.TranslationTool` preferences (SQA TEST-8),
   so runs cannot be parallelised safely on a shared runner.
3. **macOS runners.** The build is Apple Silicon and macOS 26 (see
   `ALL_MACOS_MIN` in the root `CMakeLists.txt`). GitHub-hosted
   `macos-latest` has historically lagged the newest macOS by months;
   until it carries 26, CI means a self-hosted Apple Silicon runner —
   which is a machine somebody has to own, keep patched, and keep
   Homebrew current on.
4. **Nothing is public.** The repository has no remote CI configuration
   because it has had no CI host. Whichever host is chosen, the runner
   must have access to the external HGM data project if it is ever to
   run more than `ctest -LE fixture`.

## The smallest useful CI, in order

1. **Configure + build + `ctest -LE fixture`** on a clean checkout, on
   every push. This is the gate that would have caught BUILD-7 in the
   first place: it fails the moment a suite stops building or a
   fixture-free test regresses.
2. **`brew bundle check --file=Brewfile`** before the build, so a
   missing or drifted dependency is a named failure rather than a
   confusing configure error.
3. **Publish `BUILD_MANIFEST.json`** as a build artifact. The moment two
   machines press, the diff between two manifests is the answer to "why
   is this build different?".
4. Only then: the fixture-dependent 37, on a runner that has the data,
   and the press itself — after the split in (1) above.

## Effort, honestly

Steps 1–3 are roughly a day's work *given a runner* — a workflow file,
a Brewfile install step, and an artifact upload. Step 4 is gated on the
press split and on data access, and is the larger half. Acquiring and
maintaining the runner is the real cost, and it is a decision for Adam,
not a technical detail.
