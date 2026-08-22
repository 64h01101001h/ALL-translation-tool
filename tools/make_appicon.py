#!/usr/bin/env python3
"""Generate the macOS app icon from parameters, not by hand.

Why this exists: app/AppIcon.icns was a checked-in binary with no
source, so changing the wordmark from "ALL" to "DCC" (2026-08-22, the
rename to the Diamond Cutter Translation Tool) meant reverse-
engineering it from pixels. It should never cost that twice.

Colours and geometry below were sampled from the original icon so the
new one is the same object with different letters, not a redesign:
field #94473D, gold #A68D4E, ~4.7% inset, ~17.6% corner radius, set in
Palatino — the face the application itself uses app-wide.

Usage:  python3 tools/make_appicon.py [WORD] [--out app/AppIcon.icns]
"""
import os
import shutil
import subprocess
import sys

from PIL import Image, ImageDraw, ImageFont

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
FIELD = (148, 71, 61, 255)      # sampled: the maroon ground
GOLD = (166, 141, 78, 255)      # sampled: border and wordmark
FONT = "/System/Library/Fonts/Palatino.ttc"
INSET = 0.047                   # sampled from the 512px original
RADIUS = 0.176
BORDER = 0.012
CAP = 0.26                      # wordmark cap height as a fraction
SIZES = [16, 32, 64, 128, 256, 512, 1024]


def render(word, px):
    """Draw at 4x and downsample: PIL has no antialiased rounded_rectangle."""
    S = px * 4
    im = Image.new("RGBA", (S, S), (0, 0, 0, 0))
    d = ImageDraw.Draw(im)
    m = S * INSET
    r = S * RADIUS
    bw = max(1, int(S * BORDER))
    d.rounded_rectangle([m, m, S - m, S - m], radius=r,
                        fill=FIELD, outline=GOLD, width=bw)
    # size the wordmark to the sampled cap height, then centre on the
    # INK box rather than the metric box — Palatino's metrics carry
    # ascender space the letters do not use, and centring on them sits
    # the word visibly high.
    target = S * CAP
    size = int(target * 1.4)
    for _ in range(40):
        f = ImageFont.truetype(FONT, size)
        box = d.textbbox((0, 0), word, font=f)
        capH = box[3] - box[1]
        if abs(capH - target) <= S * 0.004:
            break
        size = max(8, int(size * (target / max(1, capH))))
    f = ImageFont.truetype(FONT, size)
    box = d.textbbox((0, 0), word, font=f)
    w, h = box[2] - box[0], box[3] - box[1]
    d.text((S / 2 - w / 2 - box[0], S / 2 - h / 2 - box[1]), word,
           font=f, fill=GOLD)
    return im.resize((px, px), Image.LANCZOS)


def main():
    word = "DCC"
    out = os.path.join(ROOT, "app/AppIcon.icns")
    args = [a for a in sys.argv[1:]]
    if args and not args[0].startswith("--"):
        word = args[0]
    if "--out" in args:
        out = args[args.index("--out") + 1]

    stage = "/tmp/appicon.iconset"
    shutil.rmtree(stage, ignore_errors=True)
    os.makedirs(stage)
    for s in SIZES:
        render(word, s).save("%s/icon_%dx%d.png" % (stage, s, s))
        if s * 2 in SIZES or s <= 512:
            render(word, s * 2).save(
                "%s/icon_%dx%d@2x.png" % (stage, s, s))
    subprocess.run(["iconutil", "-c", "icns", stage, "-o", out],
                   check=True)
    print("wrote %s  (%s, %d bytes)"
          % (os.path.relpath(out, ROOT), word, os.path.getsize(out)))
    render(word, 512).save("/tmp/iconwork/preview.png")
    print("preview: /tmp/iconwork/preview.png")


if __name__ == "__main__":
    main()
