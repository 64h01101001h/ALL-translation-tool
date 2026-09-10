# Diamond Cutter Drills — the iPhone app

Path A, built 2026-09-09. **The phone generates nothing.** Every drill in
`drills.json` was produced on the Mac by the same `allcore` the desktop uses,
so the app inherits the desktop's fixes and, more importantly, its *refusals*.

## Build it

```bash
export DEVELOPER_DIR=/Applications/Xcode.app/Contents/Developer

# 1. the pack — 4,000 drills, ~4 MB
cmake --build build -j8 --target build_drill_pack
./build/core/build_drill_pack build/hgm_spine_v27_2.db ios/DiamondDrills/drills.json 4000

# 2. the app
SDK=$(xcrun --sdk iphonesimulator --show-sdk-path)
xcrun --sdk iphonesimulator swiftc -parse-as-library \
  -target arm64-apple-ios17.0-simulator -sdk "$SDK" -O -whole-module-optimization \
  ios/DiamondDrills/DiamondDrills.swift -o DiamondDrills.app/DiamondDrills
```

For a real device swap `iphonesimulator` for `iphoneos`, the target for
`arm64-apple-ios17.0`, and sign the bundle with Adam's team.

## What the pack inherits

Every guard in `DrillFactory::isDrillable`, measured on the real corpus:

| Refused | Why | Share of the corpus |
|---|---|---|
| The title catalogue (TITL*, AUTH, SUBJ) | a catalogue of work titles, not teaching material — and the adaptive draw preferred it because it is the shortest text there is | 9.5% |
| English sitting in the Tibetan field | renders as convincing nonsense; fabrication, not a display fault | 0.60% |
| English wildly out of proportion | 300 words of hint for 3 syllables; median is 1.43 : 1, p95 3.11 : 1, ceiling 6 : 1 | 1.8% |

Tibetan is converted on the Mac through `allcore::acipDisplayPieces`, so the
phone renders only Unicode it was handed. Anything the engine could not read
arrives already flagged in ⟨ ⟩ and can never be mistaken for script — about
26% of drills carry one such flag, and the reason is written up in
`docs/FINDING_ACIP_EWTS_GAPS.md`.

## What it does not have yet

The Trainer, the vocabulary deck, the SRS schedule, the other four drill modes,
and any sync with the desktop. Path A is the cloze drill and nothing else.
