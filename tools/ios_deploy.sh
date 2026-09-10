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
xcrun devicectl device install app --device "$DEVICE" "$APP" >/dev/null
[ "$LAUNCH" = "1" ] && xcrun devicectl device process launch \
    --device "$DEVICE" "$BUNDLE" >/dev/null && echo "   launched"
echo
echo "ON THE PHONE: $STAMP"
