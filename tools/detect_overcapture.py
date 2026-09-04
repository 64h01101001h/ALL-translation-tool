#!/usr/bin/env python3
"""Gate 3 candidate: SELF-REFERENTIAL OVER-CAPTURE DETECTION.

The rule, stated before it was run (docs/GATE_TRIALS.md):

    An English span over-captures whenever it contains material whose
    Tibetan licensor is present in the same segment and already carries
    its own span.

The hard part is knowing what English a Tibetan word "licenses". This
detector answers it WITHOUT a dictionary and without a model, by using
the alignment layer as its own lexicon:

    span A on segment S has English E_A.
    span B on the same segment S has Tibetan key K_B.
    if E_A contains a phrase that the layer records ELSEWHERE as a
    rendering of K_B, then A has probably swallowed B's material.

Self-reference is the point. It needs no outside resource, it improves as
the layer grows, and it can only fire on evidence the project itself
banked. It also targets the mechanism BOTH audits measured as dominant
(span over-capture: 7 of 12, and 8 of 11), rather than a surface feature
like a stray quote mark.

FALSIFICATION CRITERIA, FIXED BEFORE THE FIRST RUN:
  - falsified if it fires on more than 1,000 triples. The layer-wide
    defect estimate is ~1,423 pairs, of which over-capture is ~58%
    (~825). A detector aimed at one mechanism that fires more often than
    that mechanism is estimated to occur is finding something else.
  - falsified if precision on a random draw of its fires is below 50%,
    the same threshold the numeral gate was held to.
"""
import json, io, os, re, sys, glob, collections

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
LAYER = os.path.join(ROOT, "data", "alignment", "alignment_evidence_v1.json")
DIRS = {"C01": "pages_c01", "C02": "pages", "C03": "pages_c03", "C04": "pages_c04"}

SPAN = re.compile(r'<span class="u[^"]*" data-d="(\d)" data-l="s(\d+)([a-z]+\d*)[^"]*">'
                  r'([^<]*)</span>')

# Function words are excluded as evidence: they occur in almost every
# rendering, so matching on them would fire everywhere and prove nothing.
STOP = set("""a an the of to in on at by for with from and or but is are was
were be been being this that these those it its his her their our your my
as not no than then so if all any each some who whom which what when where
how why you we they he she i me him them us there here into onto upon over
under out up down off very just only even also more most such own same do
does did done has have had having will would can could may might must shall
should about after before between during without within upon per""".split())


def phrases(s):
    """Content phrases of 1-4 words, lowercased, stripped of punctuation."""
    words = [w for w in re.split(r"[^A-Za-z'’-]+", s.lower()) if w]
    out = set()
    for n in (1, 2, 3, 4):
        for i in range(len(words) - n + 1):
            gram = words[i:i + n]
            if all(w in STOP for w in gram):
                continue
            if n == 1 and (gram[0] in STOP or len(gram[0]) < 3):
                continue
            out.add(" ".join(gram))
    return out


def main():
    layer = json.load(io.open(LAYER, encoding="utf-8"))["pairs"]

    # the layer as a lexicon: tibetan key -> set of phrases it renders,
    # tagged with the citations that attest each, so a rendering can be
    # excluded when the only evidence for it IS the pair under test.
    lex = collections.defaultdict(lambda: collections.defaultdict(set))
    for tib, rends in layer.items():
        for r in rends:
            for ph in phrases(r["eng"]):
                for ref in r["refs"]:
                    lex[tib][ph].add(ref)

    fires = []
    for course, sub in DIRS.items():
        for page in sorted(glob.glob(os.path.join(ROOT, "data", "alignment",
                                                  sub, "*.html"))):
            html = io.open(page, encoding="utf-8").read()
            # collect spans per segment; first occurrence of a label is the
            # Tibetan side, second is the English side (the page emits the
            # Tibetan block first, always).
            seg = collections.defaultdict(lambda: collections.defaultdict(list))
            for m in SPAN.finditer(html):
                d, s, lab, txt = m.group(1), int(m.group(2)), m.group(3), m.group(4)
                seg[s][lab].append((int(d), txt))
            for s, labs in seg.items():
                sides = {}
                for lab, occ in labs.items():
                    if len(occ) < 2:
                        continue
                    sides[lab] = {"d": occ[0][0], "tib": occ[0][1], "eng": occ[1][1]}
                ref = "%s:%d" % (course, s)
                for a, A in sides.items():
                    if not A["eng"].strip():
                        continue
                    ap = phrases(A["eng"])
                    if not ap:
                        continue
                    for b, B in sides.items():
                        if a == b or not B["tib"].strip():
                            continue
                        # B must not be a part of A: nested spans (a compound
                        # and its members) legitimately share English.
                        if B["tib"] in A["tib"] or A["tib"] in B["tib"]:
                            continue
                        if B["eng"] and B["eng"] in A["eng"]:
                            continue
                        for ph in ap & set(lex.get(B["tib"], {})):
                            # the attestation must come from SOMEWHERE ELSE,
                            # or we are just rediscovering this same pair.
                            others = lex[B["tib"]][ph] - {ref}
                            if not others:
                                continue
                            fires.append({
                                "ref": ref, "page": os.path.basename(page),
                                "over_key": A["tib"], "over_eng": A["eng"],
                                "stolen_phrase": ph,
                                "true_owner_key": B["tib"],
                                "true_owner_eng": B["eng"],
                                "attested_at": sorted(others)[:4],
                                "n_attestations": len(others),
                            })
                            break
    json.dump(fires, io.open(sys.argv[1] if len(sys.argv) > 1 else
                             "/dev/stdout", "w", encoding="utf-8"),
              ensure_ascii=False, indent=1)
    pop = sum(len(r["refs"]) for rends in layer.values() for r in rends)
    sys.stderr.write("population        : %d triples\n" % pop)
    sys.stderr.write("over-capture fires: %d (%.2f%%)\n"
                     % (len(fires), 100.0 * len(fires) / pop))
    sys.stderr.write("volume criterion  : %s (falsified above 1000)\n"
                     % ("FALSIFIED" if len(fires) > 1000 else "survives"))


if __name__ == "__main__":
    main()
