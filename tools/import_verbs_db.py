#!/usr/bin/env python3
"""import_verbs_db.py — bank the CC0 Tibetan verbs database for the app.

Source: github.com/tibetan-nlp/tibetan-verbs-database (db.csv, CC0 1.0 —
verified in the repo's LICENSE). 2,491 rows of verb paradigms in Tibetan
Unicode: present, past, future, imperative, with attestation flags from
four sources (TDC, PH, GT, KN).

Emits data/extracted/verb_stems.tsv:
  stem<TAB>roles(comma)<TAB>row_id<TAB>sources(comma)<TAB>paradigm
where paradigm = "pres|past|fut|imp" for display. Stems are UNICODE —
the app compares by converting its own tokens through the proven
wylieToUnicode engine, so no new converter is introduced.

Bracketed variants like ཀེར༼ད༽ (optional suffix) expand to both forms.
A cached copy of db.csv lives beside the output; refresh it from the
repo when desired.

Usage: python3 tools/import_verbs_db.py
"""
import csv
import os
import re

HERE = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SRC = os.path.join(HERE, "data", "extracted", "verbs_db.csv")
OUT = os.path.join(HERE, "data", "extracted", "verb_stems.tsv")

ROLES = ["present", "past", "future", "imperative"]


def tibetan(s):
    return bool(s) and all(0x0F00 <= ord(c) <= 0x0FFF for c in s)


def forms(cell, skips, row_id):
    cell = (cell or "").strip()
    if not cell:
        return []
    m = re.match(r"^(.*?)༼(.+?)༽$", cell)
    out = [m.group(1), m.group(1) + m.group(2)] if m else [cell]
    good = [f for f in out if tibetan(f)]
    for f in out:
        if not tibetan(f):
            # misaligned source flags etc. — flagged, never banked
            skips.append([row_id, f])
    return good


def main():
    rows = list(csv.reader(open(SRC, encoding="utf-8")))
    src_names = rows[0][4:8]
    index = {}
    skips = []
    n_rows = 0
    for i, row in enumerate(rows[1:], 1):
        if len(row) < 4 or not any(c.strip() for c in row[:4]):
            continue
        n_rows += 1
        sources = [n for n, c in zip(src_names, row[4:8]) if c.strip()]
        paradigm = "|".join((row[k] or "").strip() for k in range(4))
        for k, role in enumerate(ROLES):
            for f in forms(row[k], skips, i):
                key = (f, i)
                index.setdefault(key, {"roles": [], "sources": sources,
                                       "paradigm": paradigm})
                if role not in index[key]["roles"]:
                    index[key]["roles"].append(role)
    with open(OUT, "w", encoding="utf-8") as out:
        out.write("# tibetan-verbs-database (tibetan-nlp) · CC0 1.0 · "
                  "reference layer — display only, never parse-binding\n")
        for (stem, row_id), v in sorted(index.items()):
            out.write("\t".join([stem, ",".join(v["roles"]), str(row_id),
                                 ",".join(v["sources"]),
                                 v["paradigm"]]) + "\n")
    print(f"{n_rows} paradigm rows -> {len(index)} stem entries -> {OUT}")
    if skips:
        print(f"skipped {len(skips)} non-Tibetan cells:", skips[:6])


if __name__ == "__main__":
    main()
