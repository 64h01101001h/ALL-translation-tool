#!/usr/bin/env python3
"""The shipped drill pack must be able to prove what it says about itself.

ios/DiamondDrills/drills.json carries a meta.tier sentence describing the
provenance of everything inside it. Until 2026-09-15 that sentence read:

    "Every English line is Geshe Michael Roach's own text from his courses.
     Engine guidance is labeled guidance."

Measured against the pack it describes: 6,003 entries are his glossary or
curated English, 709 are AUTO-ALIGNED -- machine-matched to his English,
never composed -- and 372 carry no tier at all. 400 entries carry an
aligned_eng, which is a machine-cut SPAN of one of his sentences; some of
those spans are not words he ever wrote ('da' bar -> "gressing", E-183).

So the claim was false for about a tenth of the pack, it was false in the
pack's own provenance record, and it shipped to the phone. Rule 10: never
bank a falsehood. A provenance claim that nothing checks is a claim waiting
to go stale -- this checks it.

Run after building the pack; the press and ios_deploy both rebuild it.
"""
import io
import json
import os
import sys

ABSOLUTES = ("every english line is", "all english is", "everything here is")


def walk(o, fn):
    if isinstance(o, dict):
        fn(o)
        for v in o.values():
            walk(v, fn)
    elif isinstance(o, list):
        for x in o:
            walk(x, fn)


def main():
    root = sys.argv[1] if len(sys.argv) > 1 else "."
    path = os.path.join(root, "ios/DiamondDrills/drills.json")
    if not os.path.exists(path):
        print("pack: no drills.json — nothing to check")
        return 0
    d = json.load(io.open(path, encoding="utf-8"))
    meta = d.get("meta") or {}
    claim = meta.get("tier") or ""
    fails = []

    counts = {"auto-aligned": 0, "glossary": 0, "curated": 0}
    bad_auto, bad_gmr, aligned = [], [], 0

    def visit(o):
        nonlocal aligned
        t, p = o.get("tier"), o.get("provisional")
        if isinstance(t, str) and t in counts:
            counts[t] += 1
            if t == "auto-aligned" and p is not True:
                bad_auto.append(o.get("wylie") or o.get("tib") or "?")
            if t in ("glossary", "curated") and p is True:
                bad_gmr.append(o.get("wylie") or o.get("tib") or "?")
        if isinstance(o.get("aligned_eng"), str):
            aligned += 1

    walk(d, visit)

    # 1. the machine's matches must say so, every one of them
    if bad_auto:
        fails.append("%d auto-aligned entr(ies) are not marked provisional, "
                     "so the phone can show a machine match as his English: %s"
                     % (len(bad_auto), ", ".join(bad_auto[:4])))
    # 2. and his own English must never be labelled the machine's
    if bad_gmr:
        fails.append("%d glossary/curated entr(ies) are marked provisional, "
                     "which understates his own English: %s"
                     % (len(bad_gmr), ", ".join(bad_gmr[:4])))

    # 3. the sentence must not claim more than the pack contains
    low = claim.lower()
    machine = counts["auto-aligned"]
    for a in ABSOLUTES:
        if a in low and machine:
            fails.append(
                "meta.tier claims %r while %d entr(ies) are auto-aligned "
                "machine matches -- a falsehood in the pack's own provenance "
                "record (Rule 10)" % (a, machine))
    if machine and "auto-aligned" not in low:
        fails.append("meta.tier does not mention auto-aligned, but %d "
                     "entr(ies) are: the reader is not told the pack holds "
                     "machine matches at all" % machine)
    if aligned and "aligned_eng" not in low:
        fails.append("meta.tier does not mention aligned_eng, but %d "
                     "entr(ies) carry one: those are machine-cut spans and "
                     "may not be whole words" % aligned)
    # 4. and the project names him
    if " his " in (" " + claim + " ") or claim.strip().startswith("His "):
        fails.append("meta.tier says \"his\" where the project names Geshe "
                     "Michael (Adam's ruling, 2026-09-15)")

    if fails:
        print("pack: %d claim(s) the pack cannot support" % len(fails))
        for f in fails:
            print("  FAIL", f)
        return 1
    print("pack: meta.tier holds — %d glossary, %d curated, %d auto-aligned "
          "(all marked provisional), %d aligned_eng span(s), and the sentence "
          "says so" % (counts["glossary"], counts["curated"], machine, aligned))
    return 0


if __name__ == "__main__":
    sys.exit(main())
