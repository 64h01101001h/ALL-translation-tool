#!/bin/bash
# §11: bless the current screenshot sweep as the visual-regression
# baseline. An explicit, logged act - the press gate compares against
# this set and fails loudly on unexplained change.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
APP="$ROOT/cmake-build-release/app/ALLTranslationTool.app/Contents/MacOS/ALLTranslationTool"
BLESSED="$ROOT/build/blessed_shots"
TMP="$(mktemp -d)"
"$APP" --screenshots "$TMP" >/dev/null 2>&1
mkdir -p "$BLESSED"
rm -f "$BLESSED"/*.png
cp "$TMP"/*.png "$BLESSED/"
echo "blessed $(ls "$BLESSED" | wc -l | tr -d ' ') pane shots -> $BLESSED"
date "+blessed on %Y-%m-%d %H:%M" > "$BLESSED/BLESSED.txt"
