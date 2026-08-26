#!/usr/bin/env python3
"""§11 visual-regression gate: compare a fresh --screenshots sweep
against the blessed baseline set.

Usage: shot_diff.py <blessed_dir> <current_dir>
Exit 0 = clean (or no blessed set yet); exit 1 = unexplained change.

Perceptual, not byte-wise: images are compared as downscaled grayscale
mean-absolute-difference, so antialiasing jitter passes and layout or
content changes fail. Threshold chosen from observed same-build noise.
"""
import sys, os
from PIL import Image
import numpy as np

# Two tiers (2026-08-21): panes carry LIVE content (Spotlight search
# results, folder listings, shared queues) that drifts between honest
# runs — 0.022-0.09 deltas proved to be drift, not breakage, while
# structural WRONGNESS is the geometry inquisition's job. Advisory
# tier reports; hard tier stops the press.
ADVISORY = 0.022   # printed, never fatal
THRESH = 0.09      # gross change — stops the press

def load(p):
    im = Image.open(p).convert("L")
    im.thumbnail((256, 256))
    return np.asarray(im, dtype=np.float32) / 255.0

def main():
    blessed, cur = sys.argv[1], sys.argv[2]
    if not os.path.isdir(blessed) or not os.listdir(blessed):
        # BUILD-9: this used to return 0 - so a deleted or renamed
        # baseline directory silently DISARMED the visual gate while
        # the press printed its usual success. A missing baseline is
        # not a pass; it is the gate's own preconditions failing. The
        # one legitimate case - a machine that has never blessed -
        # says so explicitly.
        if os.environ.get("SHOT_DIFF_ALLOW_EMPTY") == "1":
            print("shot_diff: no blessed set at", blessed,
                  "- SKIPPED by explicit SHOT_DIFF_ALLOW_EMPTY=1 "
                  "(first run; bless with tools/bless_shots.sh)")
            return 0
        print("shot_diff: FAIL - no blessed set at", blessed,
              "- the visual gate cannot run. Bless with "
              "tools/bless_shots.sh, or export "
              "SHOT_DIFF_ALLOW_EMPTY=1 for a deliberate first run. "
              "This is NOT a pass.")
        return 1
    bad, seen = [], 0
    for name in sorted(os.listdir(blessed)):
        if not name.endswith(".png"):
            continue
        bp, cp = os.path.join(blessed, name), os.path.join(cur, name)
        if not os.path.exists(cp):
            bad.append((name, "MISSING in current sweep"))
            continue
        a, b = load(bp), load(cp)
        if a.shape != b.shape:
            bad.append((name, f"size changed {a.shape}->{b.shape}"))
            continue
        seen += 1
        d = float(np.abs(a - b).mean())
        if d > THRESH:
            bad.append((name, f"delta {d:.4f} > {THRESH}"))
        elif d > ADVISORY:
            print(f"  advisory: {name} drift {d:.4f} "
                  f"(live content; not fatal)")
    extra = [n for n in os.listdir(cur)
             if n.endswith(".png") and
             not os.path.exists(os.path.join(blessed, n))]
    for n in sorted(extra):
        print(f"shot_diff: NEW pane (no baseline): {n} - bless to adopt")
    if bad:
        print(f"shot_diff: {len(bad)} pane(s) changed vs blessed:")
        for n, why in bad:
            print(f"  FAIL {n}: {why}")
        print("If intentional, re-bless: tools/bless_shots.sh")
        return 1
    print(f"shot_diff: {seen} pane(s) match the blessed set")
    return 0

if __name__ == "__main__":
    sys.exit(main())
