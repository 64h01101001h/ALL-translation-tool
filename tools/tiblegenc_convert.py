#!/usr/bin/env python3
"""tiblegenc_convert.py — thin CLI over py-tiblegenc (Apache-2.0)
for the app's legacy rescue lane. Runs INSIDE build/tiblegenc_venv
(the app never runs Python itself; it QProcess-calls this).

  tiblegenc_convert.py --list-fonts
  tiblegenc_convert.py <font_name> <infile>   # unicode → stdout,
                                              # stats → stderr
"""
import sys

from pytiblegenc import char_converter as cc


def main():
    if len(sys.argv) >= 2 and sys.argv[1] == "--list-fonts":
        base = cc.get_base()
        names = sorted(set(list(base.keys()) +
                           list(cc.FONT_ALIASES.keys())))
        for n in names:
            print(n)
        return 0
    if len(sys.argv) < 3:
        print(__doc__, file=sys.stderr)
        return 2
    font, path = sys.argv[1], sys.argv[2]
    with open(path, "rb") as f:
        raw = f.read()
    text = raw.decode("cp1252", errors="replace")
    # the exact stats shape pdfminer_text_converter uses
    stats = {
        "unhandled_fonts": {},
        "handled_fonts": {},
        "unknown_characters": {},
        "error_characters": 0,
        "diffs_with_utfc": {},
        "nb_non_horizontal_removed": 0,
    }
    out = cc.convert_string(text, font, stats)
    sys.stdout.write(out)
    errs = {k: v for k, v in stats.items()} if stats else {}
    print(f"[tiblegenc] font={font} chars={len(text)} "
          f"stats={errs}", file=sys.stderr)
    return 0


if __name__ == "__main__":
    sys.exit(main())
