#!/usr/bin/env python3
"""Gate 5 candidate: SEMANTIC SINGLETON AGAINST A WELL-ATTESTED KEY.

Named by the first honest audit (2026-08-28) as the only signal that would
have caught the one confirmed WRONG it found -- `'dod` keyed to "make",
sitting among eleven renderings all meaning *want*. It was never built.
This builds it.

    For a key with several renderings, flag any rendering whose content
    words are DISJOINT from every other rendering of that key.

Unlike gates 1-4 this is not a surface feature of one pair; it is a claim
about a pair's relationship to its siblings, which is the shape both
confirmed WRONGs actually had (`'dod`->"make" among want-words;
`gnas`->"really" among place-words).

It will fire on legitimate polysemy, and that is the whole question --
Geshe Michael renders one term many ways by context, and the project
preserves every one by explicit ruling. So the test is whether the
disjoint ones are WRONG or merely DISTANT.

FALSIFICATION CRITERIA, FIXED BEFORE THE FIRST RUN:
  - falsified above 150 fires (1.8% of the 8,308 triples; beyond that it
    is describing ordinary polysemy, not defects)
  - falsified below 50% precision on a hand-checked random draw of fires,
    the same threshold gates 2, 3b and 4 were held to
"""
import json, io, os, re, sys, collections

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
LAYER = os.path.join(ROOT, "data", "alignment", "alignment_evidence_v1.json")

STOP = set("""a an the of to in on at by for with from and or but is are was
were be been being this that these those it its his her their our your my as
not no than then so if all any each some who whom which what when where how
why you we they he she i me him them us there here into onto upon over under
out up down off very just only even also more most such own same do does did
done has have had having will would can could may might must shall should
about after before between during without within per one""".split())


def content(s):
    return {w for w in re.split(r"[^A-Za-z'’-]+", s.lower())
            if w and w not in STOP and len(w) > 2}


def main():
    layer = json.load(io.open(LAYER, encoding="utf-8"))["pairs"]
    MIN_RENDERINGS = int(os.environ.get("MIN_RENDERINGS", "5"))
    fires = []
    for tib, rends in layer.items():
        if len(rends) < MIN_RENDERINGS:
            continue
        sets = [(r, content(r["eng"])) for r in rends]
        for r, own in sets:
            if not own:
                continue          # rendering is all function words
            others = set()
            for r2, c2 in sets:
                if r2 is not r:
                    others |= c2
            if own & others:
                continue          # shares a content word with a sibling
            fires.append({
                "tib": tib, "eng": r["eng"], "refs": r["refs"][:3],
                "n_renderings": len(rends),
                "siblings": [x["eng"] for x in rends if x is not r][:10],
            })
    out = sys.argv[1] if len(sys.argv) > 1 else None
    if out:
        json.dump(fires, io.open(out, "w", encoding="utf-8"),
                  ensure_ascii=False, indent=1)
    pop = sum(len(r["refs"]) for rends in layer.values() for r in rends)
    sys.stderr.write("population        : %d triples\n" % pop)
    sys.stderr.write("keys with >=%d renderings: %d\n"
                     % (MIN_RENDERINGS,
                        sum(1 for v in layer.values() if len(v) >= MIN_RENDERINGS)))
    sys.stderr.write("semantic-singleton fires: %d\n" % len(fires))
    sys.stderr.write("volume criterion  : %s (falsified above 150)\n"
                     % ("FALSIFIED" if len(fires) > 150 else "survives"))


if __name__ == "__main__":
    main()
