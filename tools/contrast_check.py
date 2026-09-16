#!/usr/bin/env python3
"""Every ink is measured against the ground it actually lands on.

ux_tokens.h records contrast ratios in COMMENTS -- "(AA 4.66)", "(AA 4.51)" --
and says "every pair here is asserted on its own ground by the battery, so a
wrong value fails the selftest rather than shipping". There was no such
battery. The ratios were documentation, and documentation does not fail.

What that cost, found by the 2026-09-15 audit:
  - sixty-one sites wrote `darkChrome() ? chromeX() : kX` into card HTML, so
    Night mode painted a chrome-tuned ink on a cream page: 2.06 to 2.44
  - sourceBadge chose its ink the same way, so the five badges that say WHOSE
    English the reader is looking at were the least readable marks on the card
  - zoneLabel's #9A7A33 (3.74) and the 84000 licence line's #8A7E6E (3.68)
    are plain literals that simply never cleared AA on any ground

This computes the ratios instead of quoting them.

THE GROUND RULE, from the application stylesheet in main():
    "Reading surfaces are manuscript-cream ALWAYS -- day, night, and
     regardless of the macOS system appearance ... night chrome, paper page."
QTextBrowser / QTextEdit / QPlainTextEdit are #FAF6EE in every appearance.
Everything else follows the palette: #FFFFFF light, #1E1E1E dark (measured).

A literal that sets its OWN background in the same style attribute is judged
against that background, not against cream -- a self-contained plaque is
allowed to be dark on a cream page, and several deliberately are.
"""
import glob
import io
import os
import re
import sys

CREAM = "#FAF6EE"
CHROME_LIGHT = "#FFFFFF"
CHROME_DARK = "#1E1E1E"
AA = 4.5


def lum(h):
    h = h.lstrip("#")
    c = [int(h[i:i + 2], 16) / 255 for i in (0, 2, 4)]
    c = [(x / 12.92 if x <= 0.03928 else ((x + 0.055) / 1.055) ** 2.4)
         for x in c]
    return 0.2126 * c[0] + 0.7152 * c[1] + 0.0722 * c[2]


def ratio(a, b):
    la, lb = lum(a), lum(b)
    return (max(la, lb) + 0.05) / (min(la, lb) + 0.05)


def main():
    root = sys.argv[1] if len(sys.argv) > 1 else "."
    fails, notes = [], []

    tok = io.open(os.path.join(root, "app/ux_tokens.h"),
                  encoding="utf-8").read()

    # 1. every paper token must clear AA on cream.
    for m in re.finditer(r'constexpr const char\* (k\w+) = "(#[0-9A-Fa-f]{6})"',
                         tok):
        name, val = m.group(1), m.group(2)
        if name in ("kPaper", "kPlaqueBg"):
            continue                      # grounds, not inks
        r = ratio(val, CREAM)
        if r < AA:
            fails.append("%s %s on cream = %.2f (AA %.1f) - a paper ink that "
                         "cannot be read on paper" % (name, val, r, AA))

    # 2. every chrome token must clear AA on the chrome it names.
    for m in re.finditer(
            r'inline const char\* (chrome\w+)\(\)\s*\{\s*return darkChrome\(\)'
            r'\s*\?\s*"(#[0-9A-Fa-f]{6})"\s*:\s*"(#[0-9A-Fa-f]{6})"', tok):
        name, dark, light = m.group(1), m.group(2), m.group(3)
        if name in ("chromePlaque", "chromeRule"):
            continue                      # grounds and hairlines
        for ground, val, which in ((CHROME_DARK, dark, "dark"),
                                   (CHROME_LIGHT, light, "light")):
            r = ratio(val, ground)
            if r < AA:
                fails.append("%s %s on %s chrome %s = %.2f (AA %.1f)"
                             % (name, val, which, ground, r, AA))

    # 3. literal inks written into card HTML, judged on cream. A style that
    #    sets its own background is judged against that instead.
    # ONLY html inline styles. A Qt stylesheet -- setStyleSheet(...), or a
    # "QToolButton{...}" fragment of one -- styles a WIDGET, whose ground
    # follows the palette and is usually not cream at all; the scan viewer's
    # toolbar is deliberately dark. Judging those against cream produced four
    # false alarms on the first run, and a gate that cries wolf gets switched
    # off. So: the ink must sit inside an html style= attribute.
    style = re.compile(r"color:\s*(#[0-9A-Fa-f]{6})")
    bg = re.compile(r"background:\s*(#[0-9A-Fa-f]{6})")
    SHEET = re.compile(r"setStyleSheet|\"Q[A-Za-z]+\s*[{:]|^\s*\"\s*[a-z-]+:\s")
    seen = {}
    files = [os.path.join(root, "app/main.cpp")]
    files += sorted(glob.glob(os.path.join(root, "app/*.inc")))
    for p in files:
        rel = os.path.relpath(p, root)
        win = []
        for i, line in enumerate(io.open(p, encoding="utf-8",
                                         errors="replace"), start=1):
            # A Qt stylesheet often spans several string literals, so the
            # marker that we are inside one can be a few lines back. Requiring
            # style= on the SAME line instead lost three real card inks whose
            # attribute was split across lines, which is the wrong trade: this
            # gate exists to find unreadable text, and a miss is silent.
            win.append(line)
            if len(win) > 5:
                win.pop(0)
            if line.lstrip().startswith("//"):
                continue
            if any(SHEET.search(w) for w in win):
                continue
            for m in style.finditer(line):
                ink = m.group(1)
                b = bg.search(line)
                ground = b.group(1) if b else CREAM
                r = ratio(ink, ground)
                if r < AA:
                    key = (ink, ground)
                    seen.setdefault(key, []).append("%s:%d" % (rel, i))
    for (ink, ground), where in sorted(seen.items()):
        fails.append("%s on %s = %.2f (AA %.1f) at %d site(s): %s"
                     % (ink, ground, ratio(ink, ground), AA, len(where),
                        ", ".join(where[:4]) +
                        (" ..." if len(where) > 4 else "")))

    # 4. literal inks set on WIDGET rows -- setForeground on a tree or list.
    #    Those follow the palette, so a fixed ink has to clear AA on BOTH
    #    grounds. This section exists because sections 1-3 read only #RRGGBB
    #    and the offenders were written QColor(0x9A, 0x2B, 0x1E) -- the same
    #    colour in a spelling no gate here could see. Rejected patch rows sat
    #    at 2.34 on dark, and they are exactly the rows a person must read
    #    before deciding. A theme-aware QColor(ux::chromeX()) is the fix and
    #    is skipped.
    fg = re.compile(
        r"setForeground\s*\([^;]*?QColor\s*\(\s*"
        r"(?:0x([0-9A-Fa-f]{2})\s*,\s*0x([0-9A-Fa-f]{2})\s*,\s*0x([0-9A-Fa-f]{2})"
        r'|"(#[0-9A-Fa-f]{6})")')
    for p2 in files:
        rel = os.path.relpath(p2, root)
        for i, line in enumerate(io.open(p2, encoding="utf-8",
                                         errors="replace"), start=1):
            if line.lstrip().startswith("//"):
                continue
            for m in fg.finditer(line):
                ink = (m.group(4) if m.group(4)
                       else "#%s%s%s" % (m.group(1), m.group(2), m.group(3)))
                for ground, which in ((CHROME_DARK, "dark"),
                                      (CHROME_LIGHT, "light")):
                    r = ratio(ink, ground)
                    if r < AA:
                        fails.append(
                            "%s on %s row ground %s = %.2f (AA %.1f) at %s:%d "
                            "- a fixed ink on a widget that follows the "
                            "palette; use a ux::chrome sibling"
                            % (ink, which, ground, r, AA, rel, i))

    base_path = os.path.join(root, "tools/contrast_baseline.txt")
    accepted = set()
    if os.path.exists(base_path):
        for ln in io.open(base_path, encoding="utf-8"):
            ln = ln.strip()
            if ln and not ln.startswith("#"):
                accepted.add(ln.split("  ")[0])

    fresh = [f for f in fails if f.split(" at ")[0] not in accepted]

    if "--write-baseline" in sys.argv:
        with io.open(base_path, "w", encoding="utf-8") as f:
            f.write("# Ink/ground pairs a human has ruled on. Keyed by the\n"
                    "# measurement, so changing a value asks again.\n"
                    "# Written by tools/contrast_check.py --write-baseline.\n")
            for x in fails:
                f.write(x.split(" at ")[0] + "\n")
        print("contrast: baseline written with %d ruled pair(s)" % len(fails))
        return 0

    for n in notes:
        print("  note:", n)
    if fresh:
        print("contrast: %d ink(s) below AA on the ground they land on"
              % len(fresh))
        for x in fresh:
            print("  FAIL", x)
        return 1
    print("contrast: all inks clear AA on their own ground "
          "(%d pair(s) ruled in the baseline)" % len(accepted))
    return 0


if __name__ == "__main__":
    sys.exit(main())
