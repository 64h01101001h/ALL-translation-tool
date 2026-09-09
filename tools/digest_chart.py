#!/usr/bin/env python3
"""Render a simple, honest bar chart PNG for the daily digest.

Usage: digest_chart.py out.png "Title" "label=before:after" ["label=before:after" ...]
Bars are drawn in pairs (before, after). Values print on the bars so no
reader has to estimate. Pure Pillow — no plotting library required.
"""
import sys
from PIL import Image, ImageDraw, ImageFont

def font(size, bold=False):
    for name in (["/System/Library/Fonts/Supplemental/Arial Bold.ttf"] if bold else ["/System/Library/Fonts/Supplemental/Arial.ttf"]) + ["/System/Library/Fonts/Helvetica.ttc"]:
        try:
            return ImageFont.truetype(name, size)
        except OSError:
            continue
    return ImageFont.load_default()

def main():
    if len(sys.argv) < 4:
        sys.exit(__doc__)
    out, title, specs = sys.argv[1], sys.argv[2], sys.argv[3:]
    rows = []
    for s in specs:
        label, vals = s.split("=", 1)
        before, after = vals.split(":", 1)
        rows.append((label, float(before) if before not in ("", "-") else None, float(after)))
    W, rowH, top, left, right = 1400, 74, 110, 420, 60
    H = top + rowH * len(rows) + 50
    im = Image.new("RGB", (W, H), (250, 246, 238))
    d = ImageDraw.Draw(im)
    d.text((left, 30), title, fill=(43, 33, 24), font=font(34, True))
    d.rectangle((left, 78, left + 18, 94), fill=(201, 185, 146))
    d.text((left + 26, 74), "before", fill=(107, 94, 78), font=font(20))
    d.rectangle((left + 110, 78, left + 128, 94), fill=(138, 43, 30))
    d.text((left + 136, 74), "after", fill=(107, 94, 78), font=font(20))
    maxv = max([r[2] for r in rows] + [r[1] or 0 for r in rows]) or 1
    scale = (W - left - right) / maxv
    y = top
    for label, before, after in rows:
        d.text((40, y + 14), label, fill=(43, 33, 24), font=font(24))
        if before is not None:
            bw = max(4, int(before * scale))
            d.rectangle((left, y + 6, left + bw, y + 30), fill=(201, 185, 146))
            d.text((left + bw + 8, y + 6), f"{before:g}", fill=(107, 94, 78), font=font(20))
        aw = max(4, int(after * scale))
        d.rectangle((left, y + 36, left + aw, y + 62), fill=(138, 43, 30))
        d.text((left + aw + 8, y + 38), f"{after:g}", fill=(43, 33, 24), font=font(20, True))
        y += rowH
    im.save(out)
    print(out)

if __name__ == "__main__":
    main()
