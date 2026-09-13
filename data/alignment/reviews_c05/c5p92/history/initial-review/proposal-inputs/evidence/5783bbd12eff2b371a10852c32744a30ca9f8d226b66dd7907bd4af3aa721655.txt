#!/usr/bin/env python3
"""Ratchet gate: word-level English spans must not begin with a supplied
function word (the/a/an/and/or/his/our/your/I/you) unless the licensing
Tibetan word is named.

Rule 7 of docs/ALIGNMENT_LAYER_SPEC.md says an article or hinge GMR supplied
stays UNWRAPPED. The 2026-09-04 C03-stratified audit
(docs/AUDIT_2026-09-04_C03_STRATIFIED.md) confirmed 19 defects in 100 pairs;
eight were exactly this: a one-word article or hinge wrapped at d=5. The
same scan across the shipped layer found the practice is not rare:

    pages_c01  1,640 of 9,168 d=5 English spans (17.9%) begin this way
    pages         49 of   680                  ( 7.2%)
    pages_c03    577 of 8,192                  ( 7.0%)  + 36 at d=7

Some of those ARE licensed (a demonstrative `de`, a possessive `rang gi`,
a genitive), so a hard zero would be wrong and a repair of 2,300 spans is
not this gate's job. This gate is a RATCHET:

  * for the three shipped directories the count may not rise above the
    frozen baseline below (repairs lower it; the gate is re-frozen by hand
    when they land, in the same commit, with the new number in the ledger);
  * for any other pages directory (C04 onward) the count must be zero,
    except spans listed in data/alignment/span_head_allow.json with the
    Tibetan word that licenses the function word - the reconciler names it.

Counts d=5 and d=7 only. Depth 1-4 spans are sentences/clauses/phrases and
legitimately begin with articles.
"""
import re, io, os, glob, sys, json

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
ALIGN = os.path.join(ROOT, "data", "alignment")
ALLOW = os.path.join(ALIGN, "span_head_allow.json")
BASELINE = {"pages_c01": 1643, "pages": 50, "pages_c03": 603}   # frozen 2026-09-04; c03 613->603 after the audit repairs the same day
FN = r'(?:the|The|a|A|an|An|and|And|or|Or|his|His|our|Our|your|Your|I|you|You) '
PAT = re.compile(r'<span class="u[^"]*" data-d="([57])" data-l="(s\d+\w+)">'
                 r'(?:<span class="u[^"]*" data-d="7" data-l="s\d+\w+">)?' + FN)


def main():
    allow = json.load(io.open(ALLOW, encoding="utf-8")) if os.path.exists(ALLOW) else {}
    fail = False
    dirs = sorted(d for d in os.listdir(ALIGN)
                  if os.path.isdir(os.path.join(ALIGN, d)) and d.startswith("pages"))
    for d in dirs:
        hits = []
        for f in sorted(glob.glob(os.path.join(ALIGN, d, "*.html"))):
            page = os.path.basename(f)[:-5]
            for m in PAT.finditer(io.open(f, encoding="utf-8").read()):
                key = "%s/%s" % (page, m.group(2))
                if key in allow.get(d, {}):
                    continue
                hits.append((page, m.group(2), m.group(1)))
        if d in BASELINE:
            if len(hits) > BASELINE[d]:
                fail = True
                print("SUPPLIED SPAN HEAD ratchet broken in %s: %d > baseline %d"
                      % (d, len(hits), BASELINE[d]))
            else:
                print("%-10s %4d function-word-initial d5/d7 spans (baseline %d)"
                      % (d, len(hits), BASELINE[d]))
        else:
            if hits:
                fail = True
                print("SUPPLIED SPAN HEAD in %s: %d spans begin with an article or"
                      " hinge and name no licensor:" % (d, len(hits)))
                for h in hits[:40]:
                    print("  %s %s d=%s" % h)
                print("Either trim the span (rule 7) or add it to "
                      "data/alignment/span_head_allow.json under %r with the "
                      "Tibetan word that licenses the function word." % d)
            else:
                print("%-10s    0 function-word-initial d5/d7 spans" % d)
    sys.exit(1 if fail else 0)


if __name__ == "__main__":
    main()
