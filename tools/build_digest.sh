#!/bin/zsh
# Build a daily digest's deliverables: screenshots, charts, md → txt/docx/html.
# Usage: tools/build_digest.sh YYYY-MM-DD [--draft] [--no-shots]
#   --draft     build docs/digests/D.draft.md instead of D.md
#   --no-shots  skip the headless screenshot pass (reuse existing images)
# Images live in docs/digests/img/D/ and are referenced from the .md with
# relative paths; pandoc embeds them in the .docx and the HTML render.
set -euo pipefail
cd "$(dirname "$0")/.."
D="${1:?date YYYY-MM-DD}"; shift || true
DRAFT=0; SHOTS=1
for a in "$@"; do case "$a" in --draft) DRAFT=1;; --no-shots) SHOTS=0;; esac; done
MD="docs/digests/$D.md"; [[ $DRAFT == 1 ]] && MD="docs/digests/$D.draft.md"
[[ -f "$MD" ]] || { echo "no $MD" >&2; exit 2; }
IMG="docs/digests/img/$D"; mkdir -p "$IMG"
APP="build/app/DiamondCutterTranslationTool.app/Contents/MacOS/DiamondCutterTranslationTool"
if [[ $SHOTS == 1 ]]; then
  [[ -x "$APP" ]] || { echo "build the app first ($APP)" >&2; exit 2; }
  TMP="$(mktemp -d)"
  # DCT_SHOT_EXTRA lists the digest captures beyond the blessed baseline:
  #   compare  → compare-text/compare-folders/compare-threeway.png
  #   prefs    → prefs-grid.png, prefs-house-style.png
  #   menus    → menu-<name>.png for every top-level menu
  # Digest captures (Adam, 2026-09-08): a 1440x900 logical frame — a
  # laptop screen, so panes are not crowded — at device pixel ratio 2, so
  # text is crisp: 2880x1800 PNGs. pandoc fits them to the page; the HTML
  # gets max-width. Mechanism: the offscreen platform's default screen is
  # 800x600 at 96 dpi; a config file replaces it, and with a configured
  # screen the window FILLS it (measured), so the screen is the frame:
  # logical size × 2 at 192 dpi. DCT_SHOT_SIZE overrides the logical size.
  SIZE="${DCT_SHOT_SIZE:-1440x900}"; LW="${SIZE%x*}"; LH="${SIZE#*x}"
  CFG="$(mktemp).json"
  printf '{"screens":[{"name":"digest","x":0,"y":0,"width":%s,"height":%s,"logicalDpi":192,"logicalBaseDpi":96,"dpi":192}]}' "$((LW * 2))" "$((LH * 2))" > "$CFG"
  QT_QPA_PLATFORM="offscreen:configfile=$CFG" DCT_SHOT_EXTRA="${DCT_SHOT_EXTRA:-compare,prefs,menus}" "$APP" --screenshots "$TMP" --shotsize "$SIZE" >/dev/null 2>&1 || true
  rm -f "$CFG"
  # keep only the images the .md references (the digest decides what is
  # pertinent); everything else stays out of the repository
  mkdir -p "$IMG"
  for f in "$TMP"/*.png; do
    b="$(basename "$f")"
    if grep -q "img/$D/$b" "$MD"; then cp "$f" "$IMG/$b"; fi
  done
  rm -rf "$TMP"
  echo "captures kept in $IMG:"; ls "$IMG"
fi
CSS="$(mktemp).css"; printf 'img{max-width:100%%;height:auto;border:1px solid #ddd}figcaption{color:#555;font-size:0.9em}body{max-width:60em;margin:2em auto;font-family:-apple-system,Helvetica,sans-serif;line-height:1.45}' > "$CSS"
BASE="${MD%.md}"
TITLE="$(head -1 "$MD" | sed 's/^# //')"
pandoc "$MD" --resource-path=docs/digests -o "$BASE.txt"
pandoc "$MD" --resource-path=docs/digests -o "$BASE.docx"
pandoc "$MD" --resource-path=docs/digests -s --embed-resources --standalone --css "$CSS" --metadata title="$TITLE" -o "$BASE.html" 2>/dev/null || pandoc "$MD" --resource-path=docs/digests -s --self-contained --css "$CSS" --metadata title="$TITLE" -o "$BASE.html"
rm -f "$CSS"
echo "built: $BASE.txt $BASE.docx $BASE.html"
