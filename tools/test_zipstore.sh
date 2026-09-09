#!/bin/zsh
# F6 (2026-09-09): the STORED zip container behind the tracked-changes .docx
# must satisfy a second, independent reader. Writes a sample redline through
# the app's --compare CLI and runs `unzip -t` on it. Honest [SKIP] when unzip
# or the app binary is missing; exit 1 only on a real container fault.
set -uo pipefail
cd "$(dirname "$0")/.."
APP="${APP:-build/app/DiamondCutterTranslationTool.app/Contents/MacOS/DiamondCutterTranslationTool}"
if ! command -v unzip >/dev/null 2>&1; then echo "[SKIP] unzip not installed"; exit 0; fi
if [[ ! -x "$APP" ]]; then echo "[SKIP] app binary not built ($APP)"; exit 0; fi
T="$(mktemp -d)"; trap 'rm -rf "$T"' EXIT
printf '@001A\nBLA MA LA\nSEMS CAN THAMS CAD\n' > "$T/A.act"
printf '@001A\nBLA MA LA\nSEMS CAN KUN\nBDE BA\n' > "$T/B.act"
QT_QPA_PLATFORM=offscreen "$APP" --compare "$T/A.act" "$T/B.act" "$T/out.docx" >/dev/null 2>&1
rc=$?
if [[ $rc -ne 1 ]]; then echo "[FAIL] --compare exited $rc (expected 1: differences exist)"; exit 1; fi
if [[ "$(head -c 2 "$T/out.docx")" != "PK" ]]; then echo "[FAIL] out.docx does not start with PK"; exit 1; fi
if unzip -t "$T/out.docx" 2>&1 | grep -q "No errors detected"; then echo "[PASS] unzip -t: No errors detected ($(stat -f%z "$T/out.docx") bytes, $(unzip -l "$T/out.docx" | tail -1 | awk '{print $2}') parts)"; exit 0; fi
echo "[FAIL] unzip -t reported errors:"; unzip -t "$T/out.docx" 2>&1 | tail -5; exit 1
