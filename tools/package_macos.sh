#!/bin/bash
# package_macos.sh — build the distributable macOS DMG for the team
# (leadership recommendation #7, macOS half).
#
# Produces dist/Diamond-Cutter-Translation-Tool-<version>.dmg containing:
#   Diamond Cutter Translation Tool.app   (Qt frameworks bundled, ad-hoc signed)
#   Diamond Cutter Tool Data/             (spine db + runtime data manifest)
#   README.txt
#   BUILD_MANIFEST.txt / .json            (what it was built from — BUILD-6)
#
# The app finds "Diamond Cutter Tool Data" beside itself (findDataRoot), so the
# team drags BOTH items anywhere together — Applications, a folder,
# an external drive — and it runs. Ad-hoc signing means the first
# launch needs right-click → Open (no Apple Developer ID yet; a
# notarized build is the follow-up when ALL provides an ID).
#
# Usage: bash tools/package_macos.sh [--skip-build]
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
# Shipwright L1: team vs market distribution. Market mode omits the
# payloads that may not be redistributed (reference.db - the
# unlicensed local-only compilations; BDRC OCR models now ship in
# both modes under Adam's 2026-08-22 free-distribution ruling):
#   tools/package_macos.sh [team|market]      (default: team)
#
# WHAT MAY BE IN THE IMAGE is not decided in this file. It is decided
# in docs/distribution/PAYLOAD_MANIFEST.txt, one row per path, with a
# licence and an OPEN_SOURCE_NOTICES.md anchor. The press reads its
# staging list from that manifest (step 6), prunes to it (6a) and is
# gated on it (6d) - and a market press additionally REFUSES to build
# while any staged row's terms are unresolved, printing the rows that
# block it. Before 2026-08-23 the gate was a keyword match that never
# opened the payload (SQA BUILD-4/BUILD-5).
PRESS_MODE="${1:-team}"
case "$PRESS_MODE" in team|market) ;; *)
  echo "unknown mode '$PRESS_MODE' (team|market)"; exit 2;; esac
echo "press mode: $PRESS_MODE"
BUILD="$ROOT/cmake-build-release"
DIST="$ROOT/dist"
APPVER=$(head -1 "$ROOT/VERSION")
VERSION="$APPVER"
APPNAME="Diamond Cutter Translation Tool"
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
  # BUILD-7: 37 of the 73 suites read data git does not track. An absent
  # fixture is now an honest ctest SKIP (cmake/AllFixtureTests.cmake)
  # rather than a crash or a green over zero rows — but a RELEASE must
  # not be cut from a battery that quietly ran 36 of 73. Count the skips
  # and stop. `tail -2` also used to hide which suites ran at all.
  mkdir -p "$DIST"
  CTEST_LOG="$DIST/ctest_press.log"
  if ! (cd "$BUILD" && ctest --output-on-failure -j 4) > "$CTEST_LOG" 2>&1
  then
    echo "BATTERY FAILED — not packaging. Tail of $CTEST_LOG:"
    tail -20 "$CTEST_LOG"
    exit 1
  fi
  grep -E "tests passed|Total Test time" "$CTEST_LOG" | sed 's/^/   /'
  SKIPPED=$(grep -c '\*\*\*Skipped' "$CTEST_LOG" || true)
  if [[ "$SKIPPED" != "0" ]]; then
    echo "   $SKIPPED suite(s) SKIPPED — the press gates on the FULL battery."
    grep '\*\*\*Skipped' "$CTEST_LOG" | sed 's/^/     /'
    echo "   Provision the fixtures (docs/FIXTURES.md) and press again."
    exit 1
  fi
  echo "   no suite skipped: the battery the press gates on ran in full"
fi

APP="$BUILD/app/DiamondCutterTranslationTool.app"
[[ -d "$APP" ]] || { echo "app bundle missing: $APP"; exit 1; }

echo "== 2a. constitution gate (Fidelity engine, track C) =="
python3 "$ROOT/tools/constitution_check.py" "$ROOT" || {
  echo "CONSTITUTION VIOLATION - press stopped."; exit 8; }

echo "== 2b. visual-regression gate (Phase-2 audit s11) =="
SHOTS_TMP="$(mktemp -d)"
QT_QPA_PLATFORM=offscreen "$APP/Contents/MacOS/DiamondCutterTranslationTool" --screenshots "$SHOTS_TMP" >/dev/null 2>&1 || true
python3 "$ROOT/tools/shot_diff.py" "$ROOT/build/blessed_shots" "$SHOTS_TMP" || {
  echo "VISUAL REGRESSION - press stopped. Re-bless only if the change is intentional."; exit 7; }

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
fixup_macho "$STAGE/$APPNAME.app/Contents/MacOS/DiamondCutterTranslationTool"
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

# The press ritual's own recovery move is "PROVE with --screenshots on
# the INSTALLED binary" — and it could not be executed, because
# macdeployqt bundles only libqcocoa and the harness needs the
# offscreen platform. Verifying the installed artifact was therefore
# impossible without opening windows on Adam's screen. Copy the plugin
# in so the ritual's own instruction works (found 2026-08-22, pressing
# the rename).
echo "== 4c. offscreen platform plugin (so the INSTALLED app is verifiable) =="
QTPLUG="$(dirname "$QTBIN")/share/qt/plugins/platforms/libqoffscreen.dylib"
if [[ -f "$QTPLUG" ]]; then
  cp "$QTPLUG" "$STAGE/$APPNAME.app/Contents/PlugIns/platforms/"
  fixup_macho "$STAGE/$APPNAME.app/Contents/PlugIns/platforms/libqoffscreen.dylib"
  echo "   offscreen plugin bundled"
else
  echo "   WARNING: libqoffscreen.dylib not found at $QTPLUG —"
  echo "   the installed app will NOT be headlessly verifiable"
fi

echo "== 4d. GPL-3.0-only plugin removal =="
# SQA BUILD-2. macdeployqt copies Qt's virtual-keyboard input-context
# plugin by default. Homebrew records qtvirtualkeyboard as
# "GPL-3.0-only AND Apache-2.0 AND BSD-3-Clause" — the one object in
# the bundle carrying strong copyleft with no alternative and no
# linking exception. The app is a desktop tool with a hardware
# keyboard and never selects that input context, so it goes: the
# sharpest licence question in the bundle disappears with it. If a
# touch/on-screen keyboard is ever wanted, this line comes out AND
# the licence consequence gets decided first.
VKPLUG="$STAGE/$APPNAME.app/Contents/PlugIns/platforminputcontexts"
rm -f "$VKPLUG/libqtvirtualkeyboardplugin.dylib"
rmdir "$VKPLUG" 2>/dev/null || true
echo "   qtvirtualkeyboard plugin removed"

echo "== 4e. bundled-component licences =="
# SQA BUILD-2: the notices named 6 software components while ~33
# upstream projects shipped, and the DMG carried no LGPL text at all.
# This maps every Mach-O object in the bundle to the Homebrew formula
# that produced it, writes Contents/Resources/licenses/
# BUNDLED_COMPONENTS.tsv (component, version, SPDX, licence texts,
# object count) and copies the licence texts out of the kegs. It
# FAILS the press on any object it cannot map — an unattributable
# binary in a distribution is the whole finding. A market press also
# demands the texts be present.
if [[ "$PRESS_MODE" == "market" ]]; then
  python3 "$ROOT/tools/bundle_licenses.py" \
          "$STAGE/$APPNAME.app" "$ROOT" --require-texts || {
    echo "BUNDLED-LICENCE GATE FAILED - press stopped."; exit 6; }
else
  python3 "$ROOT/tools/bundle_licenses.py" \
          "$STAGE/$APPNAME.app" "$ROOT" || {
    echo "BUNDLED-LICENCE GATE FAILED - press stopped."; exit 6; }
fi

echo "== 4f. software bill of materials (SBOM) =="
# P1 2026-08-24. bundle_licenses.py (4e) has just mapped every Mach-O
# object to its upstream project; this turns that plus the payload
# manifest into CycloneDX. Data layers are included deliberately: the
# licensing finding that cost this project a day (BUILD-5) was DATA,
# not dylibs, and an SBOM that covered only the binaries would omit the
# half that has already gone wrong.
python3 "$ROOT/tools/sbom.py" "$STAGE/$APPNAME.app" || {
  echo "SBOM GENERATION FAILED - press stopped."; exit 6; }

echo "== 5. ad-hoc codesign =="
# D2 (shipwright): real signing when the identity exists, honest ad
# hoc when it doesn't. Set ALL_DEV_IDENTITY ("Developer ID
# Application: ...") once Adam's Apple Developer ID is active.
if [[ -n "${ALL_DEV_IDENTITY:-}" ]]; then
  codesign --force --deep --options runtime \
    -s "$ALL_DEV_IDENTITY" "$STAGE/$APPNAME.app"
  echo "   signed with $ALL_DEV_IDENTITY"
else
  codesign --force --deep -s - "$STAGE/$APPNAME.app" 2>/dev/null
  echo "   (ad-hoc signed — set ALL_DEV_IDENTITY to sign for real;"
  echo "    right-click-open instruction applies until then)"
fi
codesign --verify "$STAGE/$APPNAME.app" && echo "   signature ok"

echo "== 5b. architecture and macOS floor (BUILD-1) =="
# The audit found an arm64-only binary whose LSMinimumSystemVersion was
# the empty string, with the requirement stated in no document. Measure
# it here, print it so a change is visible, refuse to ship a bundle that
# cannot tell Finder what it needs — and put the answer in README.txt.
EXE="$STAGE/$APPNAME.app/Contents/MacOS/DiamondCutterTranslationTool"
ARCHS=$(lipo -archs "$EXE")
MINOS=$(otool -l "$EXE" | awk '/LC_BUILD_VERSION/{f=1} f&&/minos/{print $2; exit}')
LSMIN=$(plutil -extract LSMinimumSystemVersion raw \
        "$STAGE/$APPNAME.app/Contents/Info.plist" 2>/dev/null || echo "")
echo "   architecture:           $ARCHS"
echo "   LC_BUILD_VERSION minos: $MINOS"
echo "   LSMinimumSystemVersion: ${LSMIN:-(EMPTY)}"
if [[ -z "$LSMIN" ]]; then
  echo "   Info.plist carries no LSMinimumSystemVersion, so macOS cannot"
  echo "   warn a user on an unsupported system — the app just fails to"
  echo "   launch. Set ALL_MACOS_MIN (root CMakeLists.txt) and rebuild."
  exit 1
fi
case "$ARCHS" in *arm64*) ;; *)
  echo "   the executable has no arm64 slice ($ARCHS) — that is not the"
  echo "   artifact this project ships; check CMAKE_OSX_ARCHITECTURES."
  exit 1;; esac

echo "== 6. data manifest =="
DATA="$STAGE/Diamond Cutter Tool Data"
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
if [[ "$PRESS_MODE" == "team" ]]; then
  # local-only reference compilations: TEAM builds only, never market
  cp "$ROOT/build/reference.db" "$DATA/build/" 2>/dev/null || true
fi
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
# BDRC OCR models: CC BY-NC 4.0 + BDRC's direct okay. Adam ruled
# 2026-08-22 that the tool is distributed FREE OF CHARGE and never
# sold — which puts bundling inside the NC license (attributed in
# the Scan pane and OPEN_SOURCE_NOTICES). They therefore ship in
# BOTH modes. If the free-distribution stance ever changes, this
# block returns to team-only and market users take the in-app
# download-from-BDRC path, which still exists.
[[ -d "$ROOT/library/ocr_models" ]] && \
  cp -R "$ROOT/library/ocr_models" "$DATA/library/ocr_models"
# runtime data folders the panes read. The list is NOT written here
# any more: it comes from docs/distribution/PAYLOAD_MANIFEST.txt, so a
# directory cannot be staged without a row naming its licence and the
# OPEN_SOURCE_NOTICES.md anchor that backs it (SQA BUILD-4).
DATA_DIRS=$(python3 "$ROOT/tools/manifest_check.py" \
              --list-data-dirs "$ROOT" "$PRESS_MODE") || {
  echo "PAYLOAD MANIFEST UNREADABLE - press stopped."; exit 6; }
for d in $DATA_DIRS; do
  [[ -d "$ROOT/data/$d" ]] && cp -R "$ROOT/data/$d" "$DATA/data-$d.tmp" \
      && mkdir -p "$DATA/data" && mv "$DATA/data-$d.tmp" "$DATA/data/$d"
done
# L1 (shipwright): ingest intermediates with unresolved licenses NEVER
# ship (the app reads build/reference.db, not these); the TM cache is
# rebuilt from the CC BY tsv.gz on first use — 279MB of dead weight.
# WHAT goes is now the manifest's `drop` rows, and every removal is
# printed with its size. 2026-08-23 (SQA BUILD-5): this took the raw
# THL catalogue record trees out of the DMG — the notices said THL
# payload was "NOT redistributed" while 1,926 of its records shipped —
# together with ~62 MB of ingest by-product that app/ and core/ never
# open (bdrc_toh_labels, wvpp_rows, sanskrit_cluster_census, the
# engine oracle reports).
echo "== 6a. prune unshipped ingest material (payload manifest) =="
python3 "$ROOT/tools/manifest_check.py" --prune-extracted \
        "$ROOT" "$STAGE" "$PRESS_MODE" || {
  echo "PRUNE FAILED - press stopped."; exit 6; }

# strip superseded corpus versions from extracted copies if any slipped in
cp "$ROOT/docs/analysis/PASSAGE_ANALYSIS_TEMPLATE.md" \
   "$DATA/docs/analysis/" 2>/dev/null || true

cat > "$STAGE/README.txt" <<EOF
Diamond Cutter Translation Tool — release $VERSION ($(date +%Y-%m-%d))

SYSTEM REQUIREMENTS
  * An Apple Silicon Mac (M1 or later). This build is $ARCHS — it will
    NOT run on an Intel Mac.
  * macOS $LSMIN or later.
  If your Mac is older than either line, the app will not open at all;
  tell your coordinator rather than retrying.

1. Drag BOTH "$APPNAME.app" and "Diamond Cutter Tool Data" to the same place
   (your Applications folder, or any folder you like — together).
2. First launch: RIGHT-CLICK the app and choose Open (the build is
   signed ad hoc, not yet notarized with an Apple Developer ID).
3. Put your Tibetan texts in "Diamond Cutter Tool Data/library" — the Library
   pane reads it.
4. Propose tab: set your name and the shared proposals folder
   (the team's Dropbox) once. Authorities also tick the admin box
   to receive the Approval tab.

Everything runs offline except the Analysis pane's AI report and the
woodblock viewer's BDRC page images.

BUILD_MANIFEST.txt beside this file records exactly what this copy was
built from — toolchain, every bundled library with its version and
licence, and the checksum of the dictionary database.
EOF

# SQA BUILD-5. docs/SHIP.md said market mode "stamps the README
# accordingly"; nothing did. This heredoc had no mode conditional, so
# a 424 MB team DMG carrying build/reference.db — the unlicensed
# reference-dictionary compilations the interface labels "(local
# only)" — went out with eight lines of install instructions and not
# one word about redistribution, in a file anyone can forward. The
# stamp is real now, and it names the payload it is about.
if [[ "$PRESS_MODE" == "team" ]]; then
  cat >> "$STAGE/README.txt" <<'READMEEOF'

------------------------------------------------------------------
TEAM COPY - NOT FOR REDISTRIBUTION
------------------------------------------------------------------
This disk image is an internal ALL/ACIP build. Please do not forward
it, post it, or pass it on outside the ALL/ACIP team.

It contains, at "Diamond Cutter Tool Data/build/reference.db", the
reference-dictionary compilations (the THL layers and the
ACIPHypercontext dictionary extraction) whose per-dictionary
licensing review is still open. The tool labels every definition
drawn from them "(local only)" for that same reason. It also carries
the Asian Legacy Library's own dictionary and aligned corpus, which
are distributed within the team only.

The public build omits reference.db. If someone outside the team
needs a copy, ask for a market-mode DMG rather than forwarding this
one.

Full terms for every component are in OPEN_SOURCE_NOTICES.md beside
this file; every binary in the app is itemised with its version and
licence in "Diamond Cutter Translation Tool.app/Contents/Resources/
licenses/BUNDLED_COMPONENTS.tsv".
READMEEOF
else
  cat >> "$STAGE/README.txt" <<'READMEEOF'

------------------------------------------------------------------
PUBLIC BUILD
------------------------------------------------------------------
This build omits the local-only reference-dictionary compilations
(build/reference.db) whose licensing review is still open; the Lookup
pane says so where those layers would otherwise appear.

Full terms for every component are in OPEN_SOURCE_NOTICES.md beside
this file; every binary in the app is itemised with its version and
licence in "Diamond Cutter Translation Tool.app/Contents/Resources/
licenses/BUNDLED_COMPONENTS.tsv".
READMEEOF
fi
# and prove the stamp is really in the file that ships, not merely in
# the script that wrote it
python3 "$ROOT/tools/manifest_check.py" --readme "$ROOT" \
        "$STAGE/README.txt" "$PRESS_MODE" || {
  echo "README STAMP MISSING - press stopped."; exit 6; }

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
  "$STAGE/$APPNAME.app/Contents/MacOS/DiamondCutterTranslationTool" \
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
# and the project's own licence file, which until 2026-08-23 did not
# exist at all (SQA BUILD-2)
cp "$ROOT/LICENSE" "$STAGE/LICENSE"

echo "== 6d. payload gate: open the DMG and check every path =="
# SQA BUILD-4. The constitution's L2 rule was supposed to stop
# unmanifested payload. It matched a keyword: data/extracted passed
# forever because the unrelated string "84000" occurs in the notices,
# and it never saw anything staged by an explicit `cp` at all — not
# the 80 MB reference.db, not the OCR models. This gate walks the
# FINISHED stage and fails on any file or directory
# docs/distribution/PAYLOAD_MANIFEST.txt does not name, on team-only
# payload found in a market press, and on any `drop` row that
# survived step 6a. It is the last thing to touch the stage before
# the DMG is cut.
python3 "$ROOT/tools/manifest_check.py" --press \
        "$ROOT" "$STAGE" "$PRESS_MODE" || {
  echo "PAYLOAD GATE FAILED - press stopped."
  echo "Each FAIL above names a path and why it may not be in a"
  echo "$PRESS_MODE image. UNMANIFESTED/UNPRUNED: give it a row in"
  echo "docs/distribution/PAYLOAD_MANIFEST.txt, or stop staging it."
  if [[ "$PRESS_MODE" == "market" ]]; then
    echo "UNRESOLVED TERMS / WRONG MODE: these are the licensing"
    echo "questions docs/SHIP.md lists as open. A market DMG waits on"
    echo "the humans named there; a team DMG (the default) does not."
  fi
  exit 6; }

echo "== 6d. build manifest (BUILD-6) =="
# There was no lockfile and no record in the DMG of which Homebrew
# bottles macdeployqt copied in, so "which OpenSSL did release X carry?"
# had no answer. This writes BUILD_MANIFEST.{json,txt} beside the app:
# toolchain versions, git commit and tree state, the architecture and
# macOS floor, and every bundled Mach-O with its formula, version, SPDX
# licence and sha256 — plus the checksum of the shipped spine.
python3 "$ROOT/tools/build_manifest.py" \
    --bundle "$STAGE/$APPNAME.app" \
    --out "$STAGE" \
    --source "$ROOT" \
    --payload "$DATA" \
    --mode "$PRESS_MODE" || {
  echo "BUILD MANIFEST FAILED — a release that cannot say what it was"
  echo "built from does not ship."; exit 1; }

# 2026-08-21 field crash: the install rsync replaced the spine db
# UNDER a live instance Adam had reopened mid-press — sqlite threw,
# nothing caught it (now hardened), and the app died. The installer
# now insists on a quiet target: quit any running instance politely,
# wait, and only then touch /Applications.
if pgrep -x DiamondCutterTranslationTool >/dev/null; then
  echo "   (a running instance holds the install target - asking it"
  echo "    to quit before touching its files)"
  osascript -e 'quit app "DiamondCutterTranslationTool"' 2>/dev/null || true
  for i in $(seq 1 20); do
    pgrep -x DiamondCutterTranslationTool >/dev/null || break
    sleep 1
  done
  if pgrep -x DiamondCutterTranslationTool >/dev/null; then
    echo "   still running after 20s - stopping it (the relaunch at"
    echo "    step 8 brings the NEW build back)"
    pkill -x DiamondCutterTranslationTool || true
    sleep 2
  fi
fi

echo "== 6c. install to /Applications (the desktop copy) =="
# THE step that keeps Adam's running app current — it was lost in
# the 2026-08-13 rewrite, and every press until 2026-08-14 built a
# fresh DMG while the desktop kept running the old install.
INSTALL="/Applications/Diamond Cutter Translation Tool"
mkdir -p "$INSTALL"
rsync -a --delete "$STAGE/$APPNAME.app/" "$INSTALL/$APPNAME.app/"
# data: add/update shipped files, NEVER delete user materials
# ditto, not rsync: rsync dies with "Illegal byte sequence" on the
# corpus's decomposed-Unicode filenames (e.g. "Ārya Mahāyāna Prasāda
# …Sūtra.txt") and stops MID-COPY — found 2026-08-22 migrating Adam's
# 4,648-file library, where rsync moved 76 files and quit. ditto is
# macOS-native and handles the normalisation. Still never --delete:
# user materials live in this tree.
ditto "$STAGE/Diamond Cutter Tool Data" "$INSTALL/Diamond Cutter Tool Data"
cp "$STAGE/README.txt" "$STAGE/OPEN_SOURCE_NOTICES.md" \
   "$STAGE/LICENSE" "$STAGE/BUILD_MANIFEST.txt" \
   "$STAGE/BUILD_MANIFEST.json" "$INSTALL/" 2>/dev/null || true
# verify: the installed binary must BE the staged binary
if cmp -s "$STAGE/$APPNAME.app/Contents/MacOS/DiamondCutterTranslationTool" \
          "$INSTALL/$APPNAME.app/Contents/MacOS/DiamondCutterTranslationTool"; then
  echo "   installed binary == staged binary (byte-identical)"
else
  echo "   ERROR: installed binary differs from the staged build!"
  exit 1
fi
echo "   installed: $INSTALL/$APPNAME.app ($(date))"

echo "== 7. DMG =="
mkdir -p "$DIST"
DMG="$DIST/Diamond-Cutter-Translation-Tool-$VERSION.dmg"
rm -f "$DMG"
hdiutil create -volname "$APPNAME $VERSION" -srcfolder "$STAGE" \
    -ov -format UDZO "$DMG" | tail -1

# D2: notarization — armed, dormant until the keychain profile exists.
# One-time setup once the Developer ID is active:
#   xcrun notarytool store-credentials ALL_NOTARY --apple-id ... \
#     --team-id ... --password <app-specific>
if [[ -n "${ALL_DEV_IDENTITY:-}" ]] && \
   xcrun notarytool history --keychain-profile ALL_NOTARY \
     >/dev/null 2>&1; then
  echo "== 7b. notarize + staple =="
  xcrun notarytool submit "$DMG" --keychain-profile ALL_NOTARY --wait
  xcrun stapler staple "$DMG"
else
  echo "   (notarization skipped — no ALL_NOTARY profile; Gatekeeper"
  echo "    will require right-click-open on downloaded copies)"
fi
du -sh "$DMG"
echo "PACKAGE COMPLETE: $DMG"

echo "== 8. relaunch the installed app =="
# the quit at step 0 leaves the translator without the app; bring the
# NEW build up and verify it stays up (2026-08-20: two silent
# relaunch failures read as "the app keeps crashing" from the
# user's seat — never end a press with the app down)
open "$INSTALL/$APPNAME.app"
sleep 4
if pgrep -x DiamondCutterTranslationTool >/dev/null; then
  echo "   relaunched and running"
else
  echo "   first relaunch did not stick — retrying"
  open "$INSTALL/$APPNAME.app"
  sleep 5
  pgrep -x DiamondCutterTranslationTool >/dev/null \
    && echo "   relaunched on retry" \
    || { echo "RELAUNCH FAILED — launch the app manually"; exit 9; }
fi
