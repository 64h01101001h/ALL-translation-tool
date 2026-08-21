#!/bin/sh
# Build Henning's original KCK (MIT) on macOS/clang for oracle runs.
# The sources are untouched except T1.C's 'void main' -> 'int main'
# (applied here via sed on a copy); DOS headers are shimmed.
set -e
HERE="$(cd "$(dirname "$0")" && pwd)"
BUILD="${1:-/tmp/kck_oracle_build}"
mkdir -p "$BUILD"
cp "$HERE"/*.C "$HERE"/*.H "$BUILD"/
cp "$HERE"/TC.DEF "$BUILD"/tc.def
cp "$HERE"/TC.EXT "$BUILD"/tc.ext
cp "$HERE"/BCD.EXT "$BUILD"/bcd.ext
cp "$HERE"/shims/* "$BUILD"/
cd "$BUILD"
sed -i '' 's/^void main/int main/' T1.C
cc -w -I. -std=gnu89 -x c -o kck T1.C T2.C T3.C T4.C UTILS.C BCD.C
echo "built: $BUILD/kck"
