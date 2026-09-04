#!/usr/bin/env python3
"""Gate 4 candidate: ORDINAL DISPLACEMENT IN PARALLEL LISTS.

The rule, stated before it was run:

    A span whose Tibetan key contains an ordinal morpheme (dang po,
    gnyis pa, gsum pa, ...) should hold that ordinal's English word
    ("first", "second", "third", ...). If the English ordinal is present
    in the segment but held by a DIFFERENT span, the clause has been
    displaced.

Unlike gates 1-3 this targets a mechanism actually observed rather than
a surface feature: it is the exact signature of the C03:158 defect, where
`bsam pa'i stobs kyis` swallowed "The second is reached" while the real
owner `gnyis pa 'grub` was handed "these instructions".

FALSIFICATION CRITERIA, FIXED BEFORE THE FIRST RUN:
  - falsified if it fires on more than 60 triples. A structural check on
    a closed morpheme set should be rare; firing on more than ~5% of the
    C03 population means it is matching a common legitimate pattern.
  - falsified if precision on a hand-check of its fires is below 50%,
    the same threshold gates 2 and 3b were held to.
"""
import re, io, os, glob, sys, json, collections

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SPAN = re.compile(r'<span class="u[^"]*" data-d="(\d)" data-l="s(\d+)([a-z]+\d*)[^"]*">'
                  r'([^<]*)</span>')

ORD = {
    "dang po": "first", "gnyis pa": "second", "gsum pa": "third",
    "bzhi pa": "fourth", "lnga pa": "fifth", "drug pa": "sixth",
    "bdun pa": "seventh", "brgyad pa": "eighth", "dgu pa": "ninth",
    "bcu pa": "tenth",
}


def main():
    fires = []
    for course, sub in (("C01", "pages_c01"), ("C02", "pages"),
                        ("C03", "pages_c03"), ("C04", "pages_c04")):
        for page in sorted(glob.glob(os.path.join(ROOT, "data", "alignment",
                                                  sub, "*.html"))):
            html = io.open(page, encoding="utf-8").read()
            seg = collections.defaultdict(lambda: collections.defaultdict(list))
            for m in SPAN.finditer(html):
                seg[int(m.group(2))][m.group(3)].append(m.group(4))
            for s, labs in seg.items():
                sides = {k: {"tib": v[0], "eng": v[1]}
                         for k, v in labs.items() if len(v) >= 2}
                if not sides:
                    continue
                all_eng = " ".join(x["eng"] for x in sides.values())
                for lab, sp in sides.items():
                    for tib_ord, en_ord in ORD.items():
                        if tib_ord not in sp["tib"]:
                            continue
                        if re.search(r"\b%s\b" % en_ord, sp["eng"], re.I):
                            continue          # holds its own ordinal: fine
                        # the ordinal word is somewhere else on this segment
                        holder = [l for l, x in sides.items()
                                  if l != lab
                                  and re.search(r"\b%s\b" % en_ord, x["eng"], re.I)]
                        if not holder:
                            continue          # GMR simply did not render it
                        fires.append({
                            "ref": "%s:%d" % (course, s),
                            "page": os.path.basename(page),
                            "span": lab, "tib": sp["tib"], "eng": sp["eng"],
                            "ordinal": tib_ord, "english_ordinal": en_ord,
                            "held_instead_by": [
                                {"span": h, "tib": sides[h]["tib"],
                                 "eng": sides[h]["eng"]} for h in holder],
                        })
    out = sys.argv[1] if len(sys.argv) > 1 else None
    if out:
        json.dump(fires, io.open(out, "w", encoding="utf-8"),
                  ensure_ascii=False, indent=1)
    sys.stderr.write("ordinal-displacement fires: %d\n" % len(fires))
    sys.stderr.write("volume criterion : %s (falsified above 60)\n"
                     % ("FALSIFIED" if len(fires) > 60 else "survives"))
    by = collections.Counter(f["ref"].split(":")[0] for f in fires)
    for c, n in by.most_common():
        sys.stderr.write("   %-5s %d\n" % (c, n))


if __name__ == "__main__":
    main()
