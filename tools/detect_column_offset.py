#!/usr/bin/env python3
"""*** THIS DETECTOR DOES NOT WORK. KEPT AS A RECORD OF WHAT WAS TRIED. ***

Five approaches were tried to detect column offsets automatically. All five
failed to find C13:63-82, a KNOWN offset confirmed by hand. See
docs/COLUMN_OFFSET_UNDETECTABLE.md for the full account and the
consequence, which is the important part.

Do not run this expecting a verdict. Do not extend it without reading that
document first -- the failure is structural, not a matter of tuning.

Sweep the whole corpus for TIBETAN/ENGLISH COLUMN OFFSETS.

C13:63-82 was found to carry its wylie one row below the English it
belongs to. Every segment in that run reads as coherent Tibetan beside
coherent English, and only someone who knows both texts would see they
are not the same text. No verbatim gate can catch it -- the wylie is
genuine wylie and the English genuine English, both byte-exact against
the spine.

It was found by accident, because an agent happened to cross-reference
C13 while working on C03. Nothing was looking for it. This looks.

THE ANCHOR IDEA. Geshe Michael frequently prints a Tibetan name inside
his English -- "(Ganden Hlagyama)", "('Heart Sutra')", "(Drolma, or
Tara)", "[shamata]". Those parentheticals say which Tibetan text the
English line is about WITHOUT reference to the wylie column, so they are
an independent witness to the pairing. If an anchor systematically
matches the wylie one row up or down instead of its own, the columns
have slipped.

The anchors are built from the corpus itself rather than hand-listed:
any parenthetical or bracketed phrase in the English is tested against a
transliteration of the wylie on its own row and its neighbours.
"""
import re, io, os, sys, sqlite3, collections

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, "tools"))
import build_alignment_layer as B   # noqa: E402

PAREN = re.compile(r"[\(\[]([^\)\]]{3,40})[\)\]]")

# Most parentheticals in this corpus are English explanations ("which is to
# say approached"), folio citations ("f. 117a, vv. VI.56b") or Wylie already
# written as Wylie ("Lam-gtzo rnam-gsum"). None of those is an independent
# anchor: the first two say nothing about which Tibetan the line is about,
# and the third is not independent of the wylie column at all. Only a
# PHONETIC transcription of a Tibetan name is a witness.
ENGLISH_WORD = set("""the a an of to in on at by for with from and or but is
are was were be this that which who what when where how why see also note
part one two three four five six seven eight nine ten first second third
page line verse chapter section folio english tibetan sanskrit translation
called meaning that is say approached something alive whichever living being
type deeds speech denied ends here her among all existing phenomena""".split())


def is_transliteration(a):
    """A phonetic transcription of a Tibetan or Sanskrit name, not English
    prose and not Wylie. Rejects anything containing a common English word,
    anything with digits or folio marks, and anything hyphenated in the
    Wylie style (Lam-gtzo)."""
    a = a.strip()
    if re.search(r"[0-9.;]", a):
        return False
    words = re.split(r"[^A-Za-z']+", a.lower())
    words = [w for w in words if w]
    if not words or len(words) > 4:
        return False
    if any(w in ENGLISH_WORD for w in words):
        return False
    if "-" in a:                       # Wylie-style hyphenation, not phonetic
        return False
    return all(len(w) >= 3 for w in words)
# Use the project's OWN pronunciation engine rather than a hand-rolled
# folding. pron_engine.py is the canonical wylie -> GMR-convention
# phonetics converter, validated on 2,118 card lines, and it renders
# `dga' ldan lha brgya ma` as "ganden hlagya ma" -- which is exactly how
# Geshe Michael writes it, "(Ganden Hlagyama)". A crude hand-rolled fold
# was tried first and failed in both directions at once: it flagged 13
# courses on noise AND missed C13, the one confirmed offset.
sys.path.insert(0, os.path.join(ROOT, "engines"))
import pron_engine as _P   # noqa: E402

_PC = {}


def fold(s):
    """Phonetic skeleton of a wylie string, via the canonical engine."""
    if s not in _PC:
        try:
            _PC[s] = re.sub(r"[^a-z]", "", _P.pronounce(s).lower())
        except Exception:
            _PC[s] = re.sub(r"[^a-z]", "", s.lower())
    return _PC[s]


def fold_en(s):
    """Phonetic skeleton of an English transcription -- already phonetic,
    so only normalised."""
    return re.sub(r"[^a-z]", "", s.lower())


def score(anchor, wylie):
    """Crude: does the folded anchor share a long consonant skeleton with
    the folded wylie? Deliberately blunt -- this flags candidates for a
    human, it does not adjudicate."""
    a, w = fold_en(anchor), fold(wylie)
    if len(a) < 5 or not w:
        return 0
    best = 0
    for n in range(len(a), 3, -1):
        for i in range(len(a) - n + 1):
            if a[i:i + n] in w:
                best = max(best, n)
                break
        if best:
            break
    return best


def main():
    db = sqlite3.connect(B.SPINE)
    c = db.cursor()
    c.execute("select course, seq, wylie, english from corpus_segments "
              "where wylie is not null and english is not null "
              "order by course, seq")
    by = collections.defaultdict(dict)
    for co, s, w, e in c.fetchall():
        by[co][s] = (w, e)

    # Record a verdict per anchor, keyed by (course, seq), then look for
    # RUNS. A column offset is contiguous: it starts where a row was gained
    # or lost and continues until something resyncs it. Course-level totals
    # hide that -- C13's offset run is 20 segments inside a course of
    # hundreds, so its own totals scored 5 aligned against 2 offset and the
    # course never tripped a threshold. Runs are the right unit.
    marks = collections.defaultdict(dict)
    for co, d in by.items():
        for s in sorted(d):
            w, e = d[s]
            for m in PAREN.finditer(e):
                anchor = m.group(1)
                if not is_transliteration(anchor):
                    continue
                here = score(anchor, w)
                up = score(anchor, d[s - 1][0]) if s - 1 in d else 0
                dn = score(anchor, d[s + 1][0]) if s + 1 in d else 0
                if here >= 5 and here >= up and here >= dn:
                    marks[co][s] = ("same", anchor)
                elif up >= 5 and up > here:
                    marks[co][s] = ("below", anchor)
                elif dn >= 5 and dn > here:
                    marks[co][s] = ("above", anchor)

    print("COLUMN-OFFSET SWEEP")
    print("Parenthetical phonetic transcriptions in the English, matched against")
    print("the wylie via pron_engine, looking for CONTIGUOUS RUNS of offset.\n")

    runs = []
    for co in sorted(marks):
        pts = sorted(marks[co])
        cur = []
        for s in pts:
            kind, anchor = marks[co][s]
            if kind == "same":
                if len(cur) >= 2:
                    runs.append((co, cur))
                cur = []
                continue
            if cur and (s - cur[-1][0]) > 25:
                if len(cur) >= 2:
                    runs.append((co, cur))
                cur = []
            if cur and cur[-1][1] != kind:
                if len(cur) >= 2:
                    runs.append((co, cur))
                cur = []
            cur.append((s, kind, anchor))
        if len(cur) >= 2:
            runs.append((co, cur))

    total = sum(len(v) for v in marks.values())
    off = sum(1 for v in marks.values() for k in v.values() if k[0] != "same")
    print("  anchors usable            : %d" % total)
    print("  anchors offset            : %d" % off)
    print("  CONTIGUOUS OFFSET RUNS    : %d\n" % len(runs))

    if runs:
        for co, r in sorted(runs, key=lambda x: -len(x[1])):
            lo, hi = r[0][0], r[-1][0]
            print("  *** %s:%d..%d — %d anchors, wylie sits one row %s"
                  % (co, lo, hi, len(r), r[0][1].upper()))
            for s, kind, anchor in r[:6]:
                print("        seq %-6d %r" % (s, anchor[:40]))
        print("\nRead each run by hand before reporting it. This detector")
        print("flags candidates; it does not adjudicate.")
        sys.exit(1)
    print("no contiguous offset run detected")


if __name__ == "__main__":
    main()
