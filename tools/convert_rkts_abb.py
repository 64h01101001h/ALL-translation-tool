#!/usr/bin/env python3
"""rKTs abbreviations XML -> the app's abbreviation-table CSV.

Source: data/rkts/abb.xml — Dr. Bruno Lainé's export of the rKTs
Tibetan Abbreviations list (CC BY 4.0; permission + his exact words
in docs/licenses/RKTS_ABBREVIATIONS.md).

Output: data/abbreviations/rkts_abb.csv in the same 4-column shape
AbbrTable::load() already reads (abbr wylie, abbr unicode, expansion
wylie, expansion unicode) plus a 5th column naming the attesting
source, so the card can attribute each expansion to the table that
carries it. Rows missing any of the four core fields are SKIPPED AND
COUNTED, never approximated (rule 3).
"""

import sys
import xml.etree.ElementTree as ET

SRC = "data/rkts/abb.xml"
OUT = "data/abbreviations/rkts_abb.csv"

root = ET.parse(SRC).getroot()

# the list's own bibliography: source number -> short label
srcnames = {}
for k in root:
    if k.tag != "source":
        continue
    d = {x.tag: (x.text or "").strip() for x in k}
    # source 5 has no <name>; its <link> carries the label (Esukhia)
    label = d.get("name") or d.get("link") or "unnamed source"
    srcnames[d.get("no", "")] = label

rows, skipped, comma_fixed = [], 0, 0
for k in root:
    if k.tag != "abb" and k.tag == "source":
        continue
    if k.tag == "source":
        continue
    d = {x.tag: (x.text or "").strip() for x in k}
    abbw, abb = d.get("abbw", ""), d.get("abb", "")
    tibw, tib = d.get("tibw", ""), d.get("tib", "")
    if not (abbw and abb and tibw and tib):
        skipped += 1
        continue
    src = "rKTs (CC BY 4.0)"
    name = srcnames.get(d.get("source", ""))
    if name:
        src += ": " + name
    cells = [abbw, abb, tibw, tib, src]
    clean = []
    for c in cells:
        c = c.replace("\n", " ").replace("\r", " ")
        if "," in c:  # the naive CSV split can't carry commas
            comma_fixed += 1
            c = c.replace(",", " ")
        clean.append(c)
    rows.append(",".join(clean))

with open(OUT, "w", encoding="utf-8") as f:
    f.write("Abb. Wylie,Abb. Unicode,Expan. Wylie,Expan. Unicode,"
            "Source\n")
    f.write("\n".join(rows) + "\n")

print(f"{len(rows)} rows -> {OUT}")
print(f"skipped (incomplete): {skipped}; comma-cleaned cells: "
      f"{comma_fixed}")
print(f"sources: {len(srcnames)}")
if skipped:
    sys.exit(0)
