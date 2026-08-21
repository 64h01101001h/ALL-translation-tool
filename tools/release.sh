#!/bin/bash
# D3 (shipwright): the release ritual — one command moves VERSION, the
# git tag, the CHANGELOG, the press, and the blessed visual baseline
# TOGETHER, so no release artifact can drift from its siblings.
#
#   tools/release.sh 1.0.0-rc.1 [team|market]
#
# Refuses a dirty tree. Writes VERSION, prepends a CHANGELOG scaffold
# from the commits since the last tag, commits, tags v<version>, runs
# the three-gate press in the given mode, and snapshots the blessed
# screenshot baseline as build/blessed_shots_<version>.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"

VER="${1:?usage: release.sh <version> [team|market]}"
MODE="${2:-team}"

[[ -z "$(git status --porcelain)" ]] || {
  echo "release: working tree is dirty — commit or stash first"; exit 1; }

LAST_TAG="$(git describe --tags --abbrev=0 2>/dev/null || echo "")"
echo "== release $VER ($MODE) — since ${LAST_TAG:-the beginning} =="

echo "$VER" > VERSION

{ echo "# $VER — $(date +%Y-%m-%d)"
  echo
  if [[ -n "$LAST_TAG" ]]; then
    git log --pretty='- %s' "$LAST_TAG"..HEAD | grep -v "^- Co-Auth" \
      | head -40
  else
    echo "- initial tracked release"
  fi
  echo
  [[ -f CHANGELOG.md ]] && cat CHANGELOG.md
} > CHANGELOG.md.new
mv CHANGELOG.md.new CHANGELOG.md

git add VERSION CHANGELOG.md
git commit -m "Release $VER

Co-Authored-By: Claude Fable 5 <noreply@anthropic.com>"

# the press PROVES the release before anything is tagged - a failed
# press leaves no tag behind (learned the hard way: two half-releases
# had to be unwound on 2026-08-21)
if ! bash tools/package_macos.sh "$MODE"; then
  echo "release: the press FAILED - unwinding the version commit"
  git reset --hard HEAD~1
  exit 1
fi
git tag "v$VER"

if [[ -d build/blessed_shots ]]; then
  rm -rf "build/blessed_shots_$VER"
  cp -R build/blessed_shots "build/blessed_shots_$VER"
  echo "blessed baseline snapshotted: build/blessed_shots_$VER"
fi

echo "RELEASE $VER COMPLETE — tag v$VER, DMG in dist/, baseline snapshotted."
echo "Push when ready: git push && git push --tags"
