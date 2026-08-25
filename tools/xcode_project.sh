#!/bin/bash
# xcode_project.sh — (re)generate the Xcode project for browsing and
# building this codebase in Xcode.
#
#   bash tools/xcode_project.sh [--open]
#
# WHY THIS SCRIPT EXISTS AT ALL: `xcode-select -p` on this machine
# points at /Library/Developer/CommandLineTools, which has no Xcode
# generator, so a plain `cmake -G Xcode` fails with "No CMAKE_CXX_
# COMPILER could be found". Rather than switch the global developer
# directory — which needs an admin password and would change which
# clang every OTHER build in this repo uses — this scopes DEVELOPER_DIR
# to the one command that needs it.
#
# The generated xcode/ directory is a BUILD ARTEFACT and is gitignored.
# It is safe to delete; run this again to get it back.
#
# Xcode is a second way to build, not the authority. The gate remains
# tools/verify.sh against the cmake-build tree.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"

XCDEV="/Applications/Xcode.app/Contents/Developer"
[[ -d "$XCDEV" ]] || {
  echo "xcode_project: Xcode.app is not installed at /Applications/Xcode.app"
  echo "  (the CommandLineTools alone cannot generate an Xcode project)"
  exit 1; }

echo "== regenerating xcode/ =="
rm -rf "$ROOT/xcode"
DEVELOPER_DIR="$XCDEV" cmake -G Xcode -S "$ROOT" -B "$ROOT/xcode" >/dev/null
PROJ="$(ls -d "$ROOT"/xcode/*.xcodeproj | head -1)"
echo "   $PROJ"

# Headers are not target sources, so CMake does not list them in the
# navigator. Say so rather than let the omission look like a mistake.
REPO_H=$(find app core ocr -name '*.h' 2>/dev/null | wc -l | tr -d ' ')
PROJ_H=$(grep -oE '[A-Za-z0-9_/.-]+\.h' "$PROJ/project.pbxproj" | sort -u | wc -l | tr -d ' ')
echo "   navigator lists $PROJ_H of $REPO_H headers — CMake only lists"
echo "   files that are target sources; the rest are still indexed and"
echo "   reachable by jump-to-definition and Open Quickly."

[[ "${1:-}" == "--open" ]] && open "$PROJ"
exit 0
