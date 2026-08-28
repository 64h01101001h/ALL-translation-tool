#!/usr/bin/env python3
"""Find candidate REGISTER SPLITS: one Tibetan term, two English words that
almost never appear together.

*** UNSOUND AS BUILT. DO NOT USE ITS OUTPUT AS EVIDENCE. ***

Kept because the flaw is the finding, and it corrected a result this
project had already reported.

THE FLAW: this tool counts English words appearing in a segment whose
wylie contains the key. That is NOT the same as counting words that
RENDER the key. `thams cad` scores 254 "everything" against 45 "merit" --
but "merit" renders `bsod nams`, which merely sits in the same segments.
The tool cannot tell a rendering from a co-tenant, and after stemming and
a 20-segment floor it still emitted 684 candidates, nearly all spurious.

IT ALSO INVALIDATED ITS OWN SEED CASE. The `gus pa` split that motivated
it was measured the same way and reported as 34 "reverence" against 93
"respect". Re-checked: 25 of those 93 segments also contain another
respect-word (bkur sti, 'dud pa, zhe sa), so at most 69 are attributable.
The split direction survives; the number did not.

WHAT WOULD WORK: only span-level evidence attributes a rendering to a key,
and that is exactly what the alignment layer holds. But the layer covers
265 pages so far and holds ONE `gus pa` span. Register splits become
measurable when the scan is deep enough to supply witnesses on both sides
of a candidate -- not before.

The method is the one that found the `gus pa` split, generalised. For a
Tibetan key and two of its English renderings A and B, count across ALL
42,199 spine segments:

    nA  segments whose wylie has the key and whose English has A (not B)
    nB  segments whose wylie has the key and whose English has B (not A)
    nAB segments whose English has BOTH

If nA and nB are both substantial and nAB is near zero, the two words are
not free variants -- something in the context decides which one Geshe
Michael reaches for. That is the pattern CLAUDE.md requires be surfaced
with a warning and never auto-resolved (its example: `bsod nams`,
"goodness" in prayers against "merit" in prose).

Measured on the whole spine rather than on the alignment layer, so it sees
every course, not only the ones scanned so far.
"""
import json, io, os, re, sys, sqlite3, collections

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, "tools"))
import build_alignment_layer as B   # noqa: E402  (for SPINE)

LAYER = os.path.join(ROOT, "data", "alignment", "alignment_evidence_v1.json")

STOP = set("""a an the of to in on at by for with from and or but is are was
were be been being this that these those it its his her their our your my as
not no than then so if all any each some who whom which what when where how
why you we they he she i me him them us there here into onto upon over under
out up down off very just only even also more most such own same do does did
done has have had having will would can could may might must shall should
about after before between during without within per one two three""".split())

# words whose English form is a homograph of a different construction; the
# gus pa measurement was wrong by 9 segments until "respectively" and "with
# respect to" were excluded, so exclusions are named, not guessed.
EXCLUDE = {
    "respect": re.compile(r"\b(respectively|with respect to|in respect of)\b", re.I),
}


def stem(w):
    """Crude suffix strip. Enough to see that "perceiving", "perceives" and
    "perceived" are one word -- which the first run of this tool did not,
    and so reported 1,079 candidates that were mostly inflections."""
    for suf in ("ations", "ation", "ings", "ing", "ies", "ied", "es", "ed",
                "ly", "s"):
        if w.endswith(suf) and len(w) - len(suf) >= 4:
            return w[:-len(suf)]
    return w


def head(s):
    ws = [w for w in re.split(r"[^A-Za-z'’-]+", s.lower())
          if w and w not in STOP and len(w) > 2]
    return ws[0] if ws else None


def main():
    layer = json.load(io.open(LAYER, encoding="utf-8"))["pairs"]
    db = sqlite3.connect(B.SPINE)
    c = db.cursor()
    out = []
    keys = [(k, v) for k, v in layer.items() if len(v) >= 4 and len(k) >= 4]
    for tib, rends in keys:
        heads = collections.Counter()
        for r in rends:
            h = head(r["eng"])
            if h:
                heads[h] += 1
        if len(heads) < 2:
            continue
        c.execute("select english from corpus_segments where wylie like ?",
                  ("%" + tib + "%",))
        engs = [row[0] or "" for row in c.fetchall()]
        if len(engs) < 20:
            continue                      # too few witnesses to say anything
        for (a, _), (b, _) in [(x, y) for i, x in enumerate(heads.most_common(4))
                               for y in list(heads.most_common(4))[i + 1:]]:
            # inflections of one word are not two registers
            sa, sb = stem(a), stem(b)
            if sa == sb or sa.startswith(sb) or sb.startswith(sa):
                continue
            pa = re.compile(r"\b%s" % re.escape(a), re.I)
            pb = re.compile(r"\b%s" % re.escape(b), re.I)
            nA = nB = nAB = 0
            for e in engs:
                ea = EXCLUDE[a].sub("", e) if a in EXCLUDE else e
                eb = EXCLUDE[b].sub("", e) if b in EXCLUDE else e
                ha, hb = bool(pa.search(ea)), bool(pb.search(eb))
                if ha and hb:
                    nAB += 1
                elif ha:
                    nA += 1
                elif hb:
                    nB += 1
            if nA >= 20 and nB >= 20:
                overlap = nAB / float(nA + nB + nAB)
                out.append({"tib": tib, "a": a, "b": b, "nA": nA, "nB": nB,
                            "nAB": nAB, "overlap": round(overlap, 3),
                            "segments": len(engs)})
    out.sort(key=lambda x: (x["overlap"], -min(x["nA"], x["nB"])))
    dest = sys.argv[1] if len(sys.argv) > 1 else None
    if dest:
        json.dump(out, io.open(dest, "w", encoding="utf-8"),
                  ensure_ascii=False, indent=1)
    sys.stderr.write("keys examined            : %d\n" % len(keys))
    sys.stderr.write("candidate register splits: %d\n" % len(out))
    sys.stderr.write("\nlowest-overlap candidates (a human reading list):\n")
    for r in out[:16]:
        sys.stderr.write("  %-18s %-12s %3d | %-12s %3d | both %2d  (%.1f%% overlap)\n"
                         % (r["tib"][:18], r["a"], r["nA"], r["b"], r["nB"],
                            r["nAB"], 100 * r["overlap"]))


if __name__ == "__main__":
    main()
