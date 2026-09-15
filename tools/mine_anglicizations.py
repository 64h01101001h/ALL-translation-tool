#!/usr/bin/env python3
"""mine_anglicizations.py — HGM-ATTESTED community spellings, mined
from the corpus itself.

The corpus English carries Geshe Michael's own published
anglicizations (Lobsang, Ganden, Tengyur, Kriya…), which sometimes
differ from his pronunciation convention (losang, genden, tenngyur,
triya). Those published spellings are attestation of the highest
tier the colloquial register can hold: HIS OWN usage.

Method (deterministic):
 - capitalized token runs in each English segment, cross-checked
   against dictionary entries whose wylie occurs in the SAME segment's
   Tibetan side (alignment = attestation);
 - a variant is kept only when its letters-only fold differs from the
   GMR pron fold by a bounded edit distance (1 for short, 2 for long)
   — fold-identical surfaces are just the pron, not variants;
 - filters: English possessive/plural endings whose stripped form
   equals the pron fold; surfaces containing English function words;
   a tiny blocklist of false cognates (china); frequency >= 2.

Rows are written into data/pron_colloquial/colloquial_pron.tsv with
class `hgm-attested` (replacing any previous hgm-attested rows), the
colloquial column folded to the searchable lowercase form.

Usage: python3 tools/mine_anglicizations.py
"""
import collections
import os
import re
import sqlite3

HERE = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
TSV = os.path.join(HERE, "data", "pron_colloquial", "colloquial_pron.tsv")

STOPWORDS = {"of", "the", "in", "and", "but", "for", "with", "a", "an",
             "to", "on", "at", "by", "from", "or", "as", "is", "was"}
BLOCKLIST = {"china", "lamas"}


def fold(s):
    return "".join(c for c in s.lower() if c.isalpha())


def ed(a, b, maxd):
    if abs(len(a) - len(b)) > maxd:
        return maxd + 1
    prev = list(range(len(b) + 1))
    for i, ca in enumerate(a, 1):
        cur = [i]
        best = i
        for j, cb in enumerate(b, 1):
            v = min(prev[j] + 1, cur[j - 1] + 1, prev[j - 1] + (ca != cb))
            cur.append(v)
            best = min(best, v)
        if best > maxd:
            return maxd + 1
        prev = cur
    return prev[-1]


def main() -> int:
    db = sqlite3.connect(os.path.join(HERE, "build",
                                      "hgm_spine_v27_2.db"))
    segs = db.execute(
        "SELECT wylie, english FROM corpus_segments "
        "WHERE english IS NOT NULL AND english != ''").fetchall()
    entries = {}
    for w, p in db.execute(
            "SELECT wylie, pronunciation FROM entries "
            "WHERE pronunciation IS NOT NULL AND pronunciation != ''"):
        entries.setdefault(w, p)

    rx = re.compile(r"\b([A-Z][a-z']+(?:[- ][A-Z][a-z']+){0,2})")
    hits = collections.Counter()
    for wy, en in segs:
        surfs = set(m.group(1) for m in rx.finditer(en))
        if not surfs:
            continue
        toks = wy.split()
        cands = set()
        for n in (2, 3, 4):
            for i in range(len(toks) - n + 1):
                cw = " ".join(toks[i:i + n])
                if cw in entries:
                    cands.add(cw)
        if not cands:
            continue
        for surf in surfs:
            words = re.split(r"[- ]", surf)
            if any(t.lower() in STOPWORDS for t in words):
                continue
            sf = fold(surf)
            if len(sf) < 5 or sf in BLOCKLIST:
                continue
            for cw in cands:
                pf = fold(entries[cw])
                if pf == sf:
                    continue
                # possessive/plural surface of the pron itself
                if sf.rstrip("s") == pf or sf == pf + "s":
                    continue
                maxd = 1 if len(sf) <= 7 else 2
                if ed(sf, pf, maxd) <= maxd:
                    hits[(sf, cw, entries[cw])] += 1

    kept = {}
    for (sf, cw, p), n in hits.items():
        if n < 2:
            continue
        key = (sf, cw)
        if key not in kept or hits[(sf, cw, p)] > kept[key][1]:
            kept[key] = (p, n)

    # rewrite: drop previous hgm-attested rows, keep everything else
    lines = []
    existing = set()
    for line in open(TSV, encoding="utf-8"):
        line = line.rstrip("\n")
        if not line or line.endswith("hgm-attested"):
            continue
        lines.append(line)
        c = line.split("\t")
        if len(c) >= 2 and not line.startswith("#"):
            existing.add((c[0], c[1]))
    added = 0
    with open(TSV, "w", encoding="utf-8") as f:
        for line in lines:
            f.write(line + "\n")
        for (sf, cw), (p, n) in sorted(kept.items(),
                                       key=lambda kv: -kv[1][1]):
            if (sf, cw) in existing:
                continue
            f.write(f"{sf}\t{cw}\t{p}\thgm-attested\n")
            added += 1
    print(f"hgm-attested anglicizations: {added} added "
          f"(frequency >= 2, filtered)")
    return 0


if __name__ == "__main__":
    main()
