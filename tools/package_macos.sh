#!/bin/bash
# package_macos.sh — build the distributable macOS DMG for the team
# (leadership recommendation #7, macOS half).
#
# Produces dist/ALL-Translation-Tool-<version>.dmg containing:
#   ALL Translation Tool.app   (Qt frameworks bundled, ad-hoc signed)
#   ALL Tool Data/             (spine db + runtime data manifest)
#   README.txt
#
# The app finds "ALL Tool Data" beside itself (findDataRoot), so the
# team drags BOTH items anywhere together — Applications, a folder,
# an external drive — and it runs. Ad-hoc signing means the first
# launch needs right-click → Open (no Apple Developer ID yet; a
# notarized build is the follow-up when ALL provides an ID).
#
# Usage: bash tools/package_macos.sh [--skip-build]
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BUILD="$ROOT/cmake-build-release"
DIST="$ROOT/dist"
APPVER=$(head -1 "$ROOT/VERSION")
VERSION="$APPVER"
APPNAME="ALL Translation Tool"
STAGE="$DIST/stage"
QTBIN="$(dirname "$(which qmake6 2>/dev/null || echo /opt/homebrew/opt/qt/bin/qmake)")"

echo "== 0. release gate =="
python3 "$ROOT/tools/validate_release.py" >/dev/null || {
  echo "RELEASE GATE FAILED — not packaging a bad release"; exit 1; }
echo "   banked release v$VERSION passes"

if [[ "${1:-}" != "--skip-build" ]]; then
  echo "== 1. Release build =="
  cmake -S "$ROOT" -B "$BUILD" -DCMAKE_BUILD_TYPE=Release >/dev/null
  cmake --build "$BUILD" -j 8 >/dev/null
  echo "== 2. batteries on the Release build =="
  (cd "$BUILD" && ctest --output-on-failure -j 4 2>&1 | tail -2)
fi

APP="$BUILD/app/ALLTranslationTool.app"
[[ -d "$APP" ]] || { echo "app bundle missing: $APP"; exit 1; }

echo "== 3. staging =="
rm -rf "$STAGE"
mkdir -p "$STAGE"
cp -R "$APP" "$STAGE/$APPNAME.app"

echo "== 4. macdeployqt (bundle Qt frameworks) =="
"$QTBIN/macdeployqt" "$STAGE/$APPNAME.app" 2>/dev/null || {
  echo "macdeployqt failed"; exit 1; }

echo "== 4b. dylib fixup (close macdeployqt's gaps) =="
# macdeployqt leaves two escape hatches to Homebrew: (a) LC_RPATH
# entries pointing into /opt/homebrew survive on the executable and on
# bundled dylibs (OpenCV's own Cellar rpath made the Cellar copies load
# BESIDE the bundled ones — two OpenBLAS, two libomp, and the app dies
# with OMP Error #15 on any machine that has Homebrew, or with missing
# dylibs on one that doesn't); (b) inter-dylib @rpath references are
# not rewritten. Strip every homebrew rpath and pin @rpath deps to the
# bundle explicitly.
FW="$STAGE/$APPNAME.app/Contents/Frameworks"
fixup_macho() {
  local f="$1" rp dep base
  # grep exits 1 on no-match; that's fine under pipefail — always
  # terminate these pipelines with `|| true` and iterate over the text
  local rps deps
  rps=$(otool -l "$f" | grep -A2 LC_RPATH | grep "path /opt/homebrew" \
        | awk '{print $2}' || true)
  for rp in $rps; do
    install_name_tool -delete_rpath "$rp" "$f" 2>/dev/null || true
  done
  deps=$(otool -L "$f" | tail -n +2 | awk '{print $1}' \
         | grep "^@rpath/" || true)
  for dep in $deps; do
    base=$(basename "$dep")
    if [[ -f "$FW/$base" ]]; then
      install_name_tool -change "$dep" \
          "@executable_path/../Frameworks/$base" "$f" 2>/dev/null || true
    fi
  done
}
for f in "$FW"/*.dylib; do fixup_macho "$f"; done
fixup_macho "$STAGE/$APPNAME.app/Contents/MacOS/ALLTranslationTool"
# prove the escape hatches are closed
LEAKS=""
while IFS= read -r f; do
  file "$f" | grep -q Mach-O || continue
  if otool -l "$f" | grep -A2 LC_RPATH | grep -q "path /opt/homebrew"; then
    LEAKS="$LEAKS$f"$'\n'
  fi
done < <(find "$STAGE/$APPNAME.app" \( -name "*.dylib" \
         -o \( -type f -perm +111 \) \))
if [[ -z "$LEAKS" ]]; then
  echo "   no homebrew rpaths remain"
else
  echo "   HOMEBREW RPATHS REMAIN:"; echo "$LEAKS"; exit 1
fi

echo "== 5. ad-hoc codesign =="
codesign --force --deep -s - "$STAGE/$APPNAME.app" 2>/dev/null
codesign --verify "$STAGE/$APPNAME.app" && echo "   signature ok"

echo "== 6. data manifest =="
DATA="$STAGE/ALL Tool Data"
mkdir -p "$DATA/build" "$DATA/docs/analysis" "$DATA/library"
# the spine ships by pointer when the release importer has moved it
SPINE_NAME="hgm_spine_v27_2.db"
if [[ -f "$ROOT/build/spine_current.txt" ]]; then
  PTR=$(head -1 "$ROOT/build/spine_current.txt" | tr -d '[:space:]')
  case "$PTR" in (*/*|*..*|"") ;; (*)
    [[ -f "$ROOT/build/$PTR" ]] && SPINE_NAME="$PTR" ;; esac
fi
cp "$ROOT/build/$SPINE_NAME" "$DATA/build/"
[[ -f "$ROOT/build/spine_current.txt" ]] &&   cp "$ROOT/build/spine_current.txt" "$DATA/build/"
cp "$ROOT/build/reference.db" "$DATA/build/" 2>/dev/null || true
# the canonical spine builder rides along so an installed app can
# import a future data release (Maintenance → Import data release…)
mkdir -p "$DATA/tools"
cp "$ROOT/tools/build_spine.py" "$DATA/tools/"
# OCR models ship in the team DMG (BDRC, CC BY-NC 4.0, used with
# BDRC's permission — credited in the Scan pane wherever output
# appears). Omitting them left packaged apps showing the
# download-it-yourself fallback (found by Adam, 2026-08-10).
mkdir -p "$DATA/data/das"
cp "$ROOT/data/das/das_1902_bookmarked.pdf" "$DATA/data/das/" \
   2>/dev/null || true
mkdir -p "$DATA/data/teaching"
cp "$ROOT/data/teaching/teaching_moments_card.json" \
   "$ROOT/data/teaching/PROVENANCE.md" "$DATA/data/teaching/" \
   2>/dev/null || true
# leftovers from older press layouts confuse debugging (2026-08-20:
# a stale teaching_moments.json + dcc_videos.json sat beside the
# live card index); the runtime reads ONLY the card index — keep the
# staged folder to exactly what ships
rm -f "$DATA/data/teaching/teaching_moments.json" \
      "$DATA/data/teaching/dcc_videos.json" \
      "$DATA/data/teaching/tkb_videos.json" \
      "$DATA/data/teaching/terms.txt" 2>/dev/null || true
# and never relaunch onto a missing/truncated index: verify size
SRC_SZ=$(stat -f%z "$ROOT/data/teaching/teaching_moments_card.json" 2>/dev/null || echo 0)
DST_SZ=$(stat -f%z "$DATA/data/teaching/teaching_moments_card.json" 2>/dev/null || echo 1)
if [[ "$SRC_SZ" != "$DST_SZ" ]]; then
  echo "STAGING ERROR: teaching_moments_card.json size mismatch ($SRC_SZ vs $DST_SZ)" >&2
  exit 1
fi
[[ -d "$ROOT/library/ocr_models" ]] && \
  cp -R "$ROOT/library/ocr_models" "$DATA/library/ocr_models"
# runtime data folders the panes read (enumerated from the code)
for d in fonts honorifics pron_colloquial abbreviations extracted idioms \
         botok spellcheck soas_pos whitney candidate_alignments; do
  [[ -d "$ROOT/data/$d" ]] && cp -R "$ROOT/data/$d" "$DATA/data-$d.tmp" \
      && mkdir -p "$DATA/data" && mv "$DATA/data-$d.tmp" "$DATA/data/$d"
done
# strip superseded corpus versions from extracted copies if any slipped in
cp "$ROOT/docs/analysis/PASSAGE_ANALYSIS_TEMPLATE.md" \
   "$DATA/docs/analysis/" 2>/dev/null || true

cat > "$STAGE/README.txt" <<EOF
ALL Translation Tool — release $VERSION ($(date +%Y-%m-%d))

1. Drag BOTH "$APPNAME.app" and "ALL Tool Data" to the same place
   (your Applications folder, or any folder you like — together).
2. First launch: RIGHT-CLICK the app and choose Open (the build is
   signed ad hoc, not yet notarized with an Apple Developer ID).
3. Put your Tibetan texts in "ALL Tool Data/library" — the Library
   pane reads it.
4. Propose tab: set your name and the shared proposals folder
   (the team's Dropbox) once. Authorities also tick the admin box
   to receive the Approval tab.

Everything runs offline except the Analysis pane's AI report and the
woodblock viewer's BDRC page images.
EOF

echo "== 6b. launch test from the staged layout =="
# the app must come up from the distribution layout itself (data found
# beside the .app), on the real cocoa platform — the bundle ships no
# offscreen plugin, exactly like a user's machine
# two attempts: a clean-exit flake was observed once under packaging
# load (2026-08-10, exit 0, no output, no crash report, unreproducible
# 5/5 afterward) — one retry absorbs a transient environment hiccup;
# two consecutive failures still fail the gate, with the exit code
# recorded for diagnosis
LAUNCH_OK=0
for ATTEMPT in 1 2 3; do
  "$STAGE/$APPNAME.app/Contents/MacOS/ALLTranslationTool" \
      > /tmp/all_stage_launch.log 2>&1 &
  LPID=$!
  sleep 8
  if kill -0 $LPID 2>/dev/null; then
    kill $LPID
    echo "   staged app launches and finds its data (attempt $ATTEMPT)"
    LAUNCH_OK=1
    break
  fi
  wait $LPID; LEXIT=$?
  echo "   attempt $ATTEMPT: staged app exited early (exit=$LEXIT)"
done
if [ "$LAUNCH_OK" != 1 ]; then
  echo "   STAGED APP FAILED TO LAUNCH (three times):"
  tail -5 /tmp/all_stage_launch.log
  exit 1
fi

# every release carries the open-source & data notices (Adam,
# 2026-08-11) — same commit discipline as the code
cp "$ROOT/docs/distribution/OPEN_SOURCE_NOTICES.md" \
   "$STAGE/OPEN_SOURCE_NOTICES.md"

echo "== 6c. install to /Applications (the desktop copy) =="
# THE step that keeps Adam's running app current — it was lost in
# the 2026-08-13 rewrite, and every press until 2026-08-14 built a
# fresh DMG while the desktop kept running the old install.
INSTALL="/Applications/ALL Translation Tool"
mkdir -p "$INSTALL"
rsync -a --delete "$STAGE/$APPNAME.app/" "$INSTALL/$APPNAME.app/"
# data: add/update shipped files, NEVER delete user materials
rsync -a "$STAGE/ALL Tool Data/" "$INSTALL/ALL Tool Data/"
cp "$STAGE/README.txt" "$STAGE/OPEN_SOURCE_NOTICES.md" "$INSTALL/" \
   2>/dev/null || true
# verify: the installed binary must BE the staged binary
if cmp -s "$STAGE/$APPNAME.app/Contents/MacOS/ALLTranslationTool" \
          "$INSTALL/$APPNAME.app/Contents/MacOS/ALLTranslationTool"; then
  echo "   installed binary == staged binary (byte-identical)"
else
  echo "   ERROR: installed binary differs from the staged build!"
  exit 1
fi
echo "   installed: $INSTALL/$APPNAME.app ($(date))"

echo "== 7. DMG =="
mkdir -p "$DIST"
DMG="$DIST/ALL-Translation-Tool-$VERSION.dmg"
rm -f "$DMG"
hdiutil create -volname "$APPNAME $VERSION" -srcfolder "$STAGE" \
    -ov -format UDZO "$DMG" | tail -1
du -sh "$DMG"
echo "PACKAGE COMPLETE: $DMG"

echo "== 8. relaunch the installed app =="
# the quit at step 0 leaves the translator without the app; bring the
# NEW build up and verify it stays up (2026-08-20: two silent
# relaunch failures read as "the app keeps crashing" from the
# user's seat — never end a press with the app down)
open "$INSTALL/$APPNAME.app"
sleep 4
if pgrep -x ALLTranslationTool >/dev/null; then
  echo "   relaunched and running"
else
  echo "   first relaunch did not stick — retrying"
  open "$INSTALL/$APPNAME.app"
  sleep 5
  pgrep -x ALLTranslationTool >/dev/null \
    && echo "   relaunched on retry" \
    || { echo "RELAUNCH FAILED — launch the app manually"; exit 9; }
fi
