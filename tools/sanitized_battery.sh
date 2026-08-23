#!/usr/bin/env bash
# sanitized_battery.sh — the whole battery under AddressSanitizer +
# UndefinedBehaviorSanitizer.
#
# Why this exists (MEM-2, SQA 2026-08-22). The only sanitized run this
# project has on record was a manual one-off at 59 suites; there are
# more than 70 now, and `grep -rn fsanitize` over every CMake/sh/py
# file in the tree returned zero. So every memory-safety property was
# enforced by somebody remembering, and the one memory-safety
# regression pin — which can only fire under a sanitizer — was dead
# code. This is the machinery.
#
# It is deliberately NOT on the default gate: the sanitized build is a
# separate full build and the suite runs roughly twice as slow. Run it
# before a release, after anything that touches raw memory, indexes a
# container by a computed value, or reworks a lifetime — and quote its
# output, not this comment.
#
#   tools/sanitized_battery.sh                 # build + full ctest
#   tools/sanitized_battery.sh -R spine        # ctest args pass through
#   ALL_ASAN_BUILD_DIR=/tmp/asan tools/sanitized_battery.sh
#
# The build tree it uses is its own (cmake-build-asan by default); it
# never touches cmake-build-release, and it refuses to if asked.
set -Eeuo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD="${ALL_ASAN_BUILD_DIR:-$ROOT/cmake-build-asan}"
JOBS="${ALL_JOBS:-8}"

red() { printf '\033[31m%s\033[0m\n' "$*"; }
grn() { printf '\033[32m%s\033[0m\n' "$*"; }

case "$(basename "$BUILD")" in
  cmake-build-release|cmake-build|build)
      red "refusing to sanitize into $BUILD — that is a shared tree."
      red "Use cmake-build-asan (the default) or set ALL_ASAN_BUILD_DIR."
      exit 1 ;;
esac

# Use the Qt the everyday build actually found, rather than guessing a
# Homebrew prefix (there are two on this machine and the wrong one
# wins under a stripped PATH).
QT_HINT=()
CACHE="$ROOT/cmake-build-release/CMakeCache.txt"
if [[ -f "$CACHE" ]]; then
    qt6dir="$(sed -n 's/^Qt6_DIR:PATH=//p' "$CACHE" | head -1)"
    [[ -n "${qt6dir:-}" ]] && QT_HINT=("-DQt6_DIR=$qt6dir")
fi

printf '\n== configure (ALL_SANITIZE=ON) ==\n'
cmake -S "$ROOT" -B "$BUILD" \
      -DCMAKE_BUILD_TYPE=RelWithDebInfo \
      -DALL_SANITIZE=ON ${QT_HINT[@]+"${QT_HINT[@]}"}   # bash 3.2-safe

printf '\n== build ==\n'
cmake --build "$BUILD" -j"$JOBS"

printf '\n== batteries (ASan + UBSan) ==\n'
# detect_leaks: macOS/arm64 has no usable LSan, so leaks are not the
# subject here — overflows, use-after-free and UB are.
export ASAN_OPTIONS="detect_leaks=0:abort_on_error=1:symbolize=1:print_stats=0"
export UBSAN_OPTIONS="print_stacktrace=1:halt_on_error=1"
export QT_QPA_PLATFORM=offscreen

if ( cd "$BUILD" && ctest -j"$JOBS" --output-on-failure "$@" ); then
    grn "SANITIZED: every suite green under ASan + UBSan"
else
    red "SANITIZED BATTERY FAILED — read the report above."
    red "A sanitizer report is a defect until proven otherwise; do not"
    red "quote a suite count from a run that ended here."
    exit 1
fi
