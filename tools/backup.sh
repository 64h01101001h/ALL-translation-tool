#!/bin/bash
# The backup ritual (2026-08-21, after the "no remote, one laptop"
# discovery). Makes a FILTERED mirror of the full history — every
# commit, minus the regenerable DMG binaries that made the repo 7GiB —
# and lays it down in every off-machine place available today:
# the external drive, Dropbox, and (when authenticated) GitHub.
#
#   tools/backup.sh            # all available targets
#   tools/backup.sh --github   # only push to GitHub (needs gh auth)
#
# Proven by restore drill: a clone from any copy yields the full tree.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
STAMP="$(date +%Y-%m-%d)"
MIRROR="/tmp/all-mirror-$STAMP.git"
NAME="ALL-tool-filtered-history-$STAMP.git"
ONLY_GH="${1:-}"

echo "== 1. mirror + filter (the working repo is never touched) =="
rm -rf "$MIRROR"
git clone --mirror "$ROOT" "$MIRROR" >/dev/null 2>&1
(cd "$MIRROR" && git filter-repo --force --invert-paths \
    --path dist/ --path-glob '*.dmg' \
    --strip-blobs-bigger-than 95M >/dev/null)
SIZE="$(du -sh "$MIRROR" | cut -f1)"
COMMITS="$(cd "$MIRROR" && git log --oneline --all | wc -l | tr -d ' ')"
echo "   filtered: $SIZE, $COMMITS commits"

# refuse to ship a filter that ate the history
[[ "$COMMITS" -gt 700 ]] || {
  echo "REFUSING: only $COMMITS commits survived the filter"; exit 1; }

if [[ "$ONLY_GH" != "--github" ]]; then
  echo "== 2. external drive =="
  for VOL in /Volumes/*; do
    DEST="$VOL/ALL-translation-tool-backups"
    [[ -d "$VOL" && -w "$VOL" && "$VOL" != "/Volumes/Macintosh HD" ]] || continue
    mkdir -p "$DEST" 2>/dev/null || continue
    rm -rf "${DEST:?}/$NAME"
    cp -R "$MIRROR" "$DEST/$NAME" && echo "   -> $DEST/$NAME"
  done

  echo "== 3. Dropbox =="
  DB="$HOME/Dropbox/ALL-translation-tool-backups"
  if [[ -d "$HOME/Dropbox" ]]; then
    mkdir -p "$DB"
    rm -rf "${DB:?}/$NAME"
    cp -R "$MIRROR" "$DB/$NAME" && echo "   -> $DB/$NAME"
  fi
fi

echo "== 4. GitHub =="
if gh auth status >/dev/null 2>&1; then
  REPO="$(gh repo view ALL-translation-tool --json name -q .name \
          2>/dev/null || true)"
  if [[ -z "$REPO" ]]; then
    gh repo create ALL-translation-tool --private \
      --description "The Diamond Cutter Translation Tool — Asian Legacy Library" \
      >/dev/null && echo "   created private repo"
  fi
  USER="$(gh api user -q .login)"
  (cd "$MIRROR" && git push --force --all \
      "https://github.com/$USER/ALL-translation-tool.git" >/dev/null 2>&1 \
   && git push --force --tags \
      "https://github.com/$USER/ALL-translation-tool.git" >/dev/null 2>&1) \
   && echo "   pushed history + tags to github.com/$USER/ALL-translation-tool"
else
  echo "   (skipped — run 'gh auth login' once, then: tools/backup.sh --github)"
fi

echo "BACKUP COMPLETE — $COMMITS commits, $SIZE per copy."
