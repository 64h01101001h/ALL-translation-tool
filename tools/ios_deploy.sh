#!/bin/bash
# ios_deploy.sh — get the current state of the project onto the iPhone.
#
# A change to the Trainer reaches the phone only after FOUR things happen, and
# forgetting any one of them leaves a stale pack that looks perfectly fine:
#
#   1. allcore and the pack builder are rebuilt
#   2. drills.json is regenerated from the spine
#   3. the iOS app is rebuilt around the new pack
#   4. it is installed on the device
#
# So this does all four, in order, and stamps the pack with the commit it came
# from so a stale one can always be spotted.
#
# Usage: bash tools/ios_deploy.sh [--no-launch]
set -euo pipefail
cd "$(dirname "$0")/.."
ROOT="$(pwd)"
export DEVELOPER_DIR=/Applications/Xcode.app/Contents/Developer

DEVICE="${ALL_IOS_DEVICE:-00008140-000465561490801C}"   # བདེ་མཆོག་
DD="${ALL_IOS_DERIVED:-$ROOT/build/ios-derived}"
BUNDLE=org.asianlegacylibrary.TibetanTrainer
LAUNCH=1
for a in "$@"; do [ "$a" = "--no-launch" ] && LAUNCH=0; done

STAMP="$(git rev-parse --short HEAD 2>/dev/null || echo nogit)"
git diff --quiet 2>/dev/null || STAMP="$STAMP-dirty"
STAMP="$STAMP $(date -u +%Y-%m-%dT%H:%M:%SZ)"

echo "== 1/4  allcore + the pack builder =="
cmake --build build -j8 --target build_drill_pack >/dev/null
echo "   built"

echo "== 2/4  the drill pack, from the spine =="
./build/core/build_drill_pack build/hgm_spine_v27_2.db \
    ios/DiamondDrills/drills.json 4000 "$STAMP"

# Is the phone actually there? Without this the build fails with "unable to
# find a destination matching the provided destination specifier", which reads
# like a broken project rather than an unplugged cable.
if ! xcrun devicectl list devices 2>/dev/null | grep -q "$DEVICE\|connected"; then
  :
fi
STATE="$(xcrun devicectl list devices 2>/dev/null | grep -c 'connected' || true)"
if [ "${STATE:-0}" = "0" ]; then
  echo
  echo "THE PHONE IS NOT REACHABLE."
  echo "  The pack above is rebuilt and current; only the install is blocked."
  echo "  Plug it in over USB-C, or bring it onto this network with the screen"
  echo "  unlocked, then run this script again."
  echo
  echo "  (Wireless needs the device awake and on the same network. As of"
  echo "   2026-09-10 it has only ever been reached over the cable.)"
  exit 3
fi

echo "== 3/4  the iOS app =="
xcodebuild -project ios/TibetanTrainer.xcodeproj -scheme TibetanTrainer \
  -destination "id=$DEVICE" -derivedDataPath "$DD" \
  -allowProvisioningUpdates -allowProvisioningDeviceRegistration \
  build >/dev/null 2>&1 || {
    echo "   BUILD FAILED — rerun without the redirect to see why:"
    echo "   xcodebuild -project ios/TibetanTrainer.xcodeproj -scheme TibetanTrainer \\"
    echo "     -destination id=$DEVICE -derivedDataPath $DD \\"
    echo "     -allowProvisioningUpdates -allowProvisioningDeviceRegistration build"
    exit 1; }
APP="$DD/Build/Products/Debug-iphoneos/TibetanTrainer.app"
echo "   $(du -h "$APP/drills.json" | cut -f1) pack inside the bundle"

echo "== 4/4  onto the phone =="
# Terminate FIRST. A running app keeps its old bundle: installing over it
# succeeds, the launch re-attaches to the live process, and the phone goes on
# serving the previous pack while every step here reports success. Adam caught
# this on 2026-09-11 — his phone showed a passage that had been refused from
# the pack hours earlier.
python3 "$ROOT/tools/ios_terminate_existing.py" --device "$DEVICE" \
    --bundle-id "$BUNDLE" --executable TibetanTrainer
xcrun devicectl device install app --device "$DEVICE" "$APP" >/dev/null
if [ "$LAUNCH" = "1" ]; then
  xcrun devicectl device process launch \
      --device "$DEVICE" --terminate-existing "$BUNDLE" >/dev/null
  echo "   launched"
fi
echo
echo "ON THE PHONE: $STAMP"
