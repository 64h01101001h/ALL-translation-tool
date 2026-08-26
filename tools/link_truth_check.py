#!/usr/bin/env python3
"""F3 (fidelity): outbound claims verified at the index level, no
network. Teaching links must trace to a caption source; Das page
references must sit inside the scanned PDF and in reading order.
Skips gracefully (exit 0 with a notice) when the data isn't present
(a build box without the caption corpus)."""
import json
import os
import re
import sys

root = sys.argv[1] if len(sys.argv) > 1 else "."
fails = []

card = os.path.join(root, "data/teaching/teaching_moments_card.json")
if os.path.exists(card):
    idx = json.load(open(card))
    ids = set()

    def walk(o):
        if isinstance(o, dict):
            for k, v in o.items():
                if isinstance(v, str):
                    m = re.search(r"(?:v=|youtu\.be/)([A-Za-z0-9_-]{11})", v)
                    if m:
                        ids.add(m.group(1))
                walk(v)
        elif isinstance(o, list):
            for x in o:
                walk(x)

    walk(idx)
    caps = set()
    capdirs = [d for d in os.listdir(os.path.join(root, "data/teaching"))
               if d.endswith("captions")] if os.path.isdir(
                   os.path.join(root, "data/teaching")) else []
    for d in capdirs:
        for f in os.listdir(os.path.join(root, "data/teaching", d)):
            m = re.match(r"(.{11})\.", f)
            if m:
                caps.add(m.group(1))
    if caps:
        missing = ids - caps
        if missing:
            fails.append(f"teaching: {len(missing)} video ids with no "
                         f"caption source (e.g. {sorted(missing)[:3]})")
        else:
            print(f"  teaching: {len(ids)} ids, all trace to captions")
    else:
        print("  teaching: caption corpus absent - skipped")
else:
    print("  teaching index absent - skipped")

das_p = os.path.join(root, "data/extracted/das_pages.json")
if os.path.exists(das_p):
    das = json.load(open(das_p))
    mx = das["meta"]["pages"]
    bad = [s for s in das["sections"] if not (1 <= s["pdf_page"] <= mx)]
    mono = sum(1 for a, b in zip(das["sections"], das["sections"][1:])
               if b["pdf_page"] < a["pdf_page"])
    if bad or mono:
        fails.append(f"das: {len(bad)} out-of-range, {mono} order "
                     f"violations")
    else:
        print(f"  das: {len(das['sections'])} sections in range and "
              f"in order")

if fails:
    print("link_truth: FAIL")
    for x in fails:
        print("  ", x)
    sys.exit(1)
print("link_truth: all claims hold")
