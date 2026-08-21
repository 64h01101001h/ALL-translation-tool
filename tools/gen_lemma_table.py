#!/usr/bin/env python3
"""gen_lemma_table.py — wylie verb-lemma fold table for search.

The lucene-bo TibSyllableLemmatizer capability, driven by OUR data:
from the CC0 verbs bank (data/extracted/verbs_db.csv), every
past/future/imperative form that maps to EXACTLY ONE present stem
across all verbs (ambiguous forms are skipped, never guessed) becomes
a search fold: non-present -> present. Wylie via pyewts.toWylie, but
the C++ battery round-trips EVERY row through our own wylieToUnicode
against the bank's unicode — any mismatch fails the suite, so pyewts
is bootstrap-only, never trusted.

Output: data/extracted/verb_lemmas.tsv (wylie_from, wylie_to,
        unicode_from, unicode_to)
Usage:  python3 tools/gen_lemma_table.py
"""
import collections
import csv
import json
import os
import re

import pyewts

HERE = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SRC = os.path.join(HERE, "data", "extracted", "verbs_db.csv")
OUT = os.path.join(HERE, "data", "extracted", "verb_lemmas.tsv")

conv = pyewts.pyewts()


def forms(cell):
    cell = (cell or "").strip()
    if not cell:
        return []
    m = re.match(r"^(.*?)༼(.+?)༽$", cell)
    return [m.group(1), m.group(1) + m.group(2)] if m else [cell]


def tib(s):
    return bool(s) and all(0x0F00 <= ord(c) <= 0x0FFF for c in s)


def main():
    rows = list(csv.reader(open(SRC, encoding="utf-8")))
    to_present = collections.defaultdict(set)
    presents = set()
    for r in rows[1:]:
        if len(r) < 4:
            continue
        pres = [f for f in forms(r[0]) if tib(f)]
        if not pres:
            continue
        presents.update(pres)
        for k in (1, 2, 3):
            for f in forms(r[k]):
                if tib(f):
                    for p in pres:
                        to_present[f].add(p)
    n_amb = 0
    out = []
    for f, ps in sorted(to_present.items()):
        if f in presents:
            continue
        if len(ps) != 1:
            n_amb += 1
            continue
        p = next(iter(ps))
        if p == f:
            continue
        wf = conv.toWylie(f).strip()
        wp = conv.toWylie(p).strip()
        if not wf or not wp or " " in wf or " " in wp:
            continue
        # only clean wylie survives (malformed bracket-variant cells
        # leak parens/digits through pyewts — 9 such rows dropped;
        # the battery round-trips every kept row through OUR converter)
        ok = all((c.islower() and c not in "vf") or c in "'+.~-"
                 for c in wf + wp)
        if not ok:
            continue
        out.append((wf, wp, f, p))
    with open(OUT, "w", encoding="utf-8") as fo:
        fo.write("# verb lemma fold (CC0 verbs bank; unambiguous only; "
                 "battery round-trips every row)\n")
        for r in out:
            fo.write("\t".join(r) + "\n")
    print(f"{len(out)} lemma folds -> {OUT} ({n_amb} ambiguous skipped)")


if __name__ == "__main__":
    main()
