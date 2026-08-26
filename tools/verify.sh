#!/usr/bin/env bash
# verify.sh — build, then test, and NEVER test a stale binary.
#
# Why this exists (2026-08-22). Twice in one day the loop
#
#     cmake --build … | grep -E " error" ; ctest
#
# reported "72/72 tests passed" over a build that had FAILED. grep found
# the errors and printed them, but its exit status is not the build's,
# and ctest happily ran the binary left over from the previous build. A
# green suite after a broken build has proved nothing at all, and it
# proves it very convincingly — the numbers look exactly like success.
#
# Both times the duplicate-definition errors were real (a "corrected"
# patch edit re-emitted its own anchor line, so the superseded edit
# matched again). Both times the 72/72 arrived a second later and would
# have been quoted as evidence if the compiler output had scrolled past.
#
# So: this script fails loudly at the first failing step, and prints the
# one line that matters at the end. Use it instead of chaining by hand.
#
# A THIRD disguise of the same hazard, found 2026-08-23 while
# mutation-testing: restoring a mutated file with `cp` gives it an
# mtime OLDER than the object built from the mutant, so make skips the
# rebuild and the "restored" run still executes the bug. If you revert
# a file by copying, `touch` it before rebuilding. Corollary: never
# read a build log through `tail -1` — that is exactly where "Building
# CXX object" would have told you the truth.
#
#   tools/verify.sh            # build + ctest + constitution
#   tools/verify.sh --quick    # build + app_selftest only
#   tools/verify.sh --tests-only
set -Eeuo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD="${ALL_BUILD_DIR:-$ROOT/cmake-build-release}"
JOBS="${ALL_JOBS:-8}"
MODE="${1:-full}"

red()  { printf '\033[31m%s\033[0m\n' "$*"; }
grn()  { printf '\033[32m%s\033[0m\n' "$*"; }
step() { printf '\n== %s ==\n' "$*"; }

fail() {
    red "VERIFY FAILED: $1"
    red "Nothing below this line was run. Do NOT quote a test count."
    exit 1
}

[[ -d "$BUILD" ]] || fail "no build dir at $BUILD (configure it first)"

if [[ "$MODE" != "--tests-only" ]]; then
    step "build"
    # The whole point: the build's own exit status decides, not a grep.
    # Output is teed so warnings/errors stay visible either way.
    if ! cmake --build "$BUILD" -j"$JOBS" 2>&1 | tee /tmp/all_verify_build.log; then
        red "--- compiler errors ---"
        grep -E "error:" /tmp/all_verify_build.log | head -20 || true
        fail "the build did not succeed"
    fi
    # STATIC-1: the -Wall wall is only a wall if something fails when
    # it is breached. A warning sat in the build output for days -
    # visible on every run, acted on by nobody - while two documents
    # asserted the count was zero. Warnings from OUR OWN sources are a
    # red gate; third-party and generated code are not ours to silence.
    OWN_WARN=$(grep -E "warning:" /tmp/all_verify_build.log                  | grep -E "$ROOT/(app|core|ocr|tools)/"                  | grep -v "third_party" | sort -u || true)
    if [[ -n "$OWN_WARN" ]]; then
        red "--- compiler warnings in our own code (STATIC-1 wall) ---"
        echo "$OWN_WARN" | head -10
        fail "the build warned; the wall means zero"
    fi
    grn "build ok - zero warnings in our own code"
fi

step "batteries"
# A FOURTH disguise, found 2026-08-23 (SQA BUILD-7): ctest exits 0 when
# a suite is SKIPPED, and 37 of the 73 suites skip when their untracked
# fixture is absent (cmake/AllFixtureTests.cmake). "batteries ok" over a
# run that executed 36 of 73 is the same lie in a new costume, so count
# the skips and put the number in the success line.
SKIP_NOTE=""
CTEST_LOG=/tmp/all_verify_ctest.log
if [[ "$MODE" == "--quick" ]]; then
    ( cd "$BUILD" && ctest --output-on-failure -R app_selftest ) \
        || fail "app_selftest"
    grn "batteries ok (app_selftest only)"
else
    set +e
    ( cd "$BUILD" && ctest -j"$JOBS" --output-on-failure ) 2>&1 \
        | tee "$CTEST_LOG"
    CTEST_RC=${PIPESTATUS[0]}
    set -e
    [[ "$CTEST_RC" == "0" ]] || fail "ctest"
    SKIPPED=$(grep -c '\*\*\*Skipped' "$CTEST_LOG" || true)
    if [[ "$SKIPPED" != "0" ]]; then
        red "$SKIPPED suite(s) did NOT run — their fixtures are absent:"
        grep '\*\*\*Skipped' "$CTEST_LOG" | sed 's/^/  /'
        red "docs/FIXTURES.md says how to produce each one."
        grn "batteries ok on the suites that ran"
        SKIP_NOTE=", $SKIPPED SKIPPED (docs/FIXTURES.md)"
    else
        grn "batteries ok — nothing skipped"
    fi
fi

if [[ "$MODE" != "--quick" ]]; then
    step "constitution"
    python3 "$ROOT/tools/constitution_check.py" "$ROOT" || fail "constitution"
fi

grn "VERIFIED: built from this source, batteries green${SKIP_NOTE}, constitution holds"
