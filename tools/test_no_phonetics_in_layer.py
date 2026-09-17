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
# 2026-09-17: this was a SHARE against a 2% ceiling. It sat at 0.08% and
# passed, and never named the twelve pairs it was letting through -- a ratchet
# twenty-five times above its own measurement, with room for ~280 more to
# arrive without a word. Reading the twelve was the whole lesson: three were
# not phonetic at all (the detector fires when a gloss merely STARTS WITH the
# phonetic reading, so "summoned", "cherishes" and "long" matched), and most of
# the rest are ordinary English loanwords -- lama, geshe, guru, mani.
#
# So the ceiling is now a backstop, not the judgement. Every flagged pair must
# carry a written ruling in tools/phonetics_baseline.txt; anything unruled
# fails and is named. Same shape as tools/naming_baseline.txt, which works
# here for the same reason: a provenance gate asks "has a person decided this",
# which is answerable, instead of "is this correct", which is not.
CEILING = 0.02          # backstop only; the baseline is what actually judges
BASELINE = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))),
                        "tools", "phonetics_baseline.txt")


def ruled():
    """(tib, eng) pairs a person has already ruled on, with the reason."""
    out = {}
    if not os.path.exists(BASELINE):
        return out
    for ln in io.open(BASELINE, encoding="utf-8"):
        ln = ln.rstrip("\n")
        if not ln.strip() or ln.lstrip().startswith("#"):
            continue
        parts = ln.split("\t")
        if len(parts) >= 3:
            out[(parts[0].strip(), parts[1].strip())] = parts[2].strip()
    return out


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
    known = ruled()
    fresh = [(t, e, r) for (t, e, r) in bad if (t, e) not in known]
    print("  of those, already ruled on by a person             : %d"
          % (len(bad) - len(fresh)))
    if fresh:
        print("\nFAILED: %d flagged pair(s) nobody has ruled on." % len(fresh))
        for t, e, r in fresh[:25]:
            print("   %-26s -> %-22s %s" % (t, e, r))
        print("\nRead each one and decide, then add a line to")
        print("  tools/phonetics_baseline.txt   as  <tib> TAB <eng> TAB <ruling>")
        print("The detector fires when a gloss merely STARTS WITH the phonetic")
        print("reading, so check it is really a transcription before acting --")
        print("three of the first twelve were ordinary English.")
        sys.exit(1)
    if share > CEILING:
        print("\nFAILED: flagged share %.2f%% exceeds the %.0f%% backstop even "
              "though every pair is ruled on. Something has changed in bulk."
              % (100 * share, 100 * CEILING))
        sys.exit(1)
    stale = [k for k in known if k not in {(t, e) for (t, e, _) in bad}]
    if stale:
        print("  rulings that no longer match any pair (%d) - the gloss changed "
              "or the entry left the layer:" % len(stale))
        for t, e in stale[:5]:
            print("      %s -> %s" % (t, e))
    print("  every flagged pair carries a written ruling — clean")


if __name__ == "__main__":
    main()
