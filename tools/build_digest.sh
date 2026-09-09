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
# Density: the captures are 2x, and a document converter reads the density
# from the PNG, not from a flag. Stamp it, capping wide images to the text
# column (Adam, 2026-09-09: a small menu was filling a whole page).
python3 tools/digest_image_dpi.py "$IMG"
# Readability (Adam, 2026-09-08): air around every heading, paragraph,
# list and figure — nothing touches. Same rules in Word via
# tools/digest_reference.docx (regenerate with tools/make_digest_reference.py).
CSS="$(mktemp).css"; cat > "$CSS" <<'CSSEOF'
body{max-width:62em;margin:2.5em auto;padding:0 1.5em;font-family:-apple-system,Helvetica,Arial,sans-serif;font-size:1.05em;line-height:1.55;color:#222}
h1{font-size:1.7em;line-height:1.25;margin:0 0 1.2em}
h2{font-size:1.3em;margin:2.4em 0 0.9em;padding-top:0.6em;border-top:1px solid #e3ddd0}
p{margin:0 0 1em}
ul,ol{margin:0.6em 0 1.2em;padding-left:1.6em}
li{margin:0.45em 0}
li>p{margin:0.3em 0}
figure{margin:2em 0 2.2em;text-align:center}
img{max-width:100%;height:auto;border:1px solid #ddd;box-shadow:0 1px 4px rgba(0,0,0,.08)}
figcaption{color:#555;font-size:0.92em;line-height:1.45;margin:0.7em auto 0;max-width:52em;text-align:left}
table{border-collapse:collapse;margin:1.4em 0 2em}
th,td{padding:0.45em 0.9em;border-bottom:1px solid #e3ddd0;text-align:left;vertical-align:top}
th{background:#f4efe4}
strong{color:#111}
CSSEOF
BASE="${MD%.md}"
TITLE="$(head -1 "$MD" | sed 's/^# //')"
# The captures are taken at device pixel ratio 2, so their pixel
# dimensions are twice their intended size on the page; --dpi=192 tells
# the converter that, and a small menu capture stops filling a whole page
# (Adam, 2026-09-09: the spacing complaint was mostly oversized images).
DPI="${DCT_DIGEST_DPI:-192}"
pandoc "$MD" --resource-path=docs/digests -o "$BASE.txt"
REF="tools/digest_reference.docx"; [[ -f "$REF" ]] || python3 tools/make_digest_reference.py >/dev/null
pandoc "$MD" --resource-path=docs/digests --reference-doc="$REF" --dpi="$DPI" -o "$BASE.docx"
pandoc "$MD" --resource-path=docs/digests -s --embed-resources --standalone --css "$CSS" --dpi="$DPI" --metadata title="$TITLE" -o "$BASE.html" 2>/dev/null || pandoc "$MD" --resource-path=docs/digests -s --self-contained --css "$CSS" --dpi="$DPI" --metadata title="$TITLE" -o "$BASE.html"
rm -f "$CSS"
echo "built: $BASE.txt $BASE.docx $BASE.html"
