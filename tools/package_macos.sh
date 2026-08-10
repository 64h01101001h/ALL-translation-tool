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
VERSION=$(python3 -c "
import gzip, json
m = json.load(gzip.open('$ROOT/data/hgm_dictionary_v27_2.json.gz','rt'))
print(m['meta']['version'])")
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
cp "$ROOT/build/hgm_spine_v27_2.db" "$DATA/build/"
cp "$ROOT/build/reference.db" "$DATA/build/" 2>/dev/null || true
# OCR models ship in the team DMG (BDRC, CC BY-NC 4.0, used with
# BDRC's permission — credited in the Scan pane wherever output
# appears). Omitting them left packaged apps showing the
# download-it-yourself fallback (found by Adam, 2026-08-10).
[[ -d "$ROOT/library/ocr_models" ]] && \
  cp -R "$ROOT/library/ocr_models" "$DATA/library/ocr_models"
# runtime data folders the panes read (enumerated from the code)
for d in fonts honorifics pron_colloquial abbreviations extracted \
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

echo "== 7. DMG =="
mkdir -p "$DIST"
DMG="$DIST/ALL-Translation-Tool-$VERSION.dmg"
rm -f "$DMG"
hdiutil create -volname "$APPNAME $VERSION" -srcfolder "$STAGE" \
    -ov -format UDZO "$DMG" | tail -1
du -sh "$DMG"
echo "PACKAGE COMPLETE: $DMG"
