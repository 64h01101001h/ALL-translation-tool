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
    # tee makes $? the tee's; check the build's via PIPESTATUS
    if [[ "${PIPESTATUS[0]:-0}" -ne 0 ]]; then
        grep -E "error:" /tmp/all_verify_build.log | head -20 || true
        fail "the build did not succeed (PIPESTATUS)"
    fi
    grn "build ok"
fi

step "batteries"
if [[ "$MODE" == "--quick" ]]; then
    ( cd "$BUILD" && ctest --output-on-failure -R app_selftest ) \
        || fail "app_selftest"
else
    ( cd "$BUILD" && ctest -j"$JOBS" --output-on-failure ) \
        || fail "ctest"
fi
grn "batteries ok"

if [[ "$MODE" != "--quick" ]]; then
    step "constitution"
    python3 "$ROOT/tools/constitution_check.py" "$ROOT" || fail "constitution"
fi

grn "VERIFIED: built from this source, batteries green, constitution holds"
