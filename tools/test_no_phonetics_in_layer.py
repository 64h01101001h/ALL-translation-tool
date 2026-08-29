#!/usr/bin/env python3
"""The dictionary layer must not carry phonetic transcriptions as glosses.

The liturgy pages align Geshe Michael's phonetic lines against the Tibetan
they sound out -- `byugs shing` beside "jukshing". That is intended and
valuable, but it is not dictionary content: presented as an entry it says
`bsod nams` means "sunam", when it means merit.

Adam caught 46% phonetic content in the depth-7 stratum of the published
view, 2026-08-28. The evidence layer itself was never affected because it
harvests d=5 only, but nothing was CHECKING that -- it held by luck of the
harvest depth, not by a gate. This is the gate.

Proper names are exempt and must be: `tsong kha pa` -> "Tsongkapa",
`dge bshes` -> "Geshe", `aa ti sha` -> "Atisha". For a name the
transliteration IS the English. They are distinguished by capitalisation,
which is Geshe Michael's own convention, not a list someone has to maintain.
"""
import json, io, os, re, sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, "engines"))
import pron_engine as P   # noqa: E402

LAYER = os.path.join(ROOT, "data", "alignment", "alignment_evidence_v1.json")
CEILING = 0.02          # 2% of pairs; it sits at 0.59% and all are names


def n(s):
    return re.sub(r"[^a-z]", "", (s or "").lower())


def main():
    pairs = json.load(io.open(LAYER, encoding="utf-8"))["pairs"]
    bad, names, total = [], 0, 0
    for tib, rends in pairs.items():
        try:
            ph = n(P.pronounce(tib))
        except Exception:
            continue
        for r in rends:
            total += 1
            e = r["eng"]
            ne = n(e)
            if not ph or not ne:
                continue
            if ph == ne or (len(ne) > 3 and (ph.startswith(ne) or ne.startswith(ph))):
                if e[:1].isupper():
                    names += 1          # a proper name; the correct rendering
                else:
                    bad.append((tib, e, r["refs"][0]))
    share = len(bad) / float(total) if total else 0
    print("dictionary layer: %d pairs" % total)
    print("  transliterations that are PROPER NAMES (exempt) : %d" % names)
    print("  lowercase transcriptions (phonetic pollution)   : %d  (%.2f%%)"
          % (len(bad), 100 * share))
    if share > CEILING:
        print("\nFAILED: phonetic transcriptions have entered the dictionary layer.")
        for t, e, r in bad[:25]:
            print("   %-26s -> %-22s %s" % (t, e, r))
        print("\nPhonetic pairs belong in the phonetics band, not the depth")
        print("strata. Check what the harvest depth is picking up.")
        sys.exit(1)
    print("  ceiling %.0f%% — clean" % (100 * CEILING))


if __name__ == "__main__":
    main()
