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

THRESH = 0.022   # mean |delta| on 0..1 grayscale, 256px thumbnails

def load(p):
    im = Image.open(p).convert("L")
    im.thumbnail((256, 256))
    return np.asarray(im, dtype=np.float32) / 255.0

def main():
    blessed, cur = sys.argv[1], sys.argv[2]
    if not os.path.isdir(blessed) or not os.listdir(blessed):
        print("shot_diff: no blessed set at", blessed,
              "- skipping (bless with tools/bless_shots.sh)")
        return 0
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
