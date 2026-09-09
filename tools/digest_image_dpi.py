#!/usr/bin/env python3
"""Stamp each digest image with the density that makes it the right size.

The captures are taken at device pixel ratio 2, so a 544-pixel menu is
meant to appear 2.83 inches wide, not 5.67. Document converters read the
density from the PNG itself (a pHYs chunk), not from a command-line flag,
so the density is written here: 192 dpi for everything, except an image
whose 192-dpi width would overflow the text column, which gets exactly
the density that makes it fill the column. Run over docs/digests/img/<date>.
"""
import sys, os
from PIL import Image

COLUMN_INCHES = 6.3   # letter page, one-inch margins, a little slack

def main(folder):
    for name in sorted(os.listdir(folder)):
        if not name.lower().endswith(".png"):
            continue
        path = os.path.join(folder, name)
        im = Image.open(path)
        w, _ = im.size
        dpi = 192.0
        if w / dpi > COLUMN_INCHES:
            dpi = w / COLUMN_INCHES
        im.save(path, dpi=(dpi, dpi))
        print(f"{name}: {w}px -> {w/dpi:.2f}in at {dpi:.0f} dpi")

if __name__ == "__main__":
    main(sys.argv[1] if len(sys.argv) > 1 else ".")
