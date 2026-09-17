#!/usr/bin/env python3
"""gloss_audit_against_illuminator.py — narrow 11,968 glossed headwords to a
few dozen worth a person's eyes, by asking where our English and the
Illuminator's are not merely different but a different KIND of thing.

WHY THIS EXISTS. Adam, reading the Walkthrough, asked why `gus` renders as
"i". It does because the master's glossary tier stores ['i','respect'] and the
pane shows the first. Nothing mechanical had flagged it, and he asked the right
follow-up: how do we find these systematically rather than one at a time.

WHAT IT IS NOT. It is not a defect detector, and the numbers say why. Compare
our equivalents against the Illuminator's word for word and 43% of 4,039
shared headwords "disagree" — because `kun tu bzang po` is "total goodness"
here and "totally good" there, and `kun nas dkris pa` is "chains, binding"
against "to entangle, to ensnare". Same meaning, different vocabulary. That
difference is Geshe Michael's plain register against a scholarly one, which is
the thing Adam values, not a fault. A check that fires on it is worthless.

WHAT IT LOOKS FOR. The `gus` shape specifically: OUR first equivalent is
entirely CLOSED-CLASS — a pronoun, article, preposition, auxiliary — where the
Illuminator's are content words. "i" for a word meaning reverence is that
shape; "chains" for a word meaning entanglement is not.

READ THE OUTPUT, DO NOT ACT ON IT. Around 55 headwords come out and most are
still fine, for two reasons worth knowing before you start:
  - HOMOGRAPHS. `shing` is "and/or" as a connective and "tree" as a noun;
    `nas` is "from" and also barley; `la`, `ka` and `ga` are particles and also
    letter names. Both entries are right about different words.
  - EXAMPLE SENTENCES. The Illuminator quotes usage as well as equivalents, so
    `des` comes back as "that boy threw a stone". That is its example, not its
    gloss, and this tool cannot always tell them apart.
The residue after those — `srab pa` glossed "s their", `bsgo ba` glossed "us
to do" — is the real yield, and it is small. That is the honest shape of it.

LICENSING. The Illuminator is copyrighted and licensable, and
docs/DICTIONARY_STRATEGY.md forbids shipping or paraphrasing it. This reads
Adam's own copy, cites a few words as evidence, and WRITES NOTHING into the
repository. Output goes to stdout. Do not redirect it into docs/.

RULE 1 IS UNTOUCHED. hgm_gloss is binding. Nothing here proposes replacing
Geshe Michael's English with anyone else's — it proposes that a person look.

Usage:
  python3 tools/gloss_audit_against_illuminator.py --archive <illuminator.sqlite>
"""
import gzip
import json
import os
import re
import sqlite3
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
MASTER = os.path.join(ROOT, "data", "hgm_dictionary_v27_2.json.gz")

CLOSED = set("""
i me my mine myself you your yours he him his she her hers it its they them their theirs we us our ours
this that these those who whom whose which what a an the and or but if then so of to in on at by for with
from as is are was were be been being am do does did done will would can could may might must shall should
not no yes here there now then very too also both each all any some one two three s ed ing ly ll re ve d t
""".split())


def norm(t):
    return " ".join(re.sub(r"[^a-z' \-]", " ", (t or "").lower()).split())


def closed_only(phrase):
    toks = phrase.split()
    return bool(toks) and all(t in CLOSED for t in toks)


def main():
    if "--archive" not in sys.argv:
        print("  usage: --archive <path to illuminator.sqlite>")
        return 2
    arch = sys.argv[sys.argv.index("--archive") + 1]
    if not os.path.exists(arch):
        print("  [SKIP] no Illuminator archive at %s. It is Adam's, outside "
              "this repo, and nothing here is committed from it." % arch)
        return 0
    if not os.path.exists(MASTER):
        print("  [SKIP] no master at %s (data/ is gitignored)" % MASTER)
        return 0

    d = json.load(gzip.open(MASTER, "rt", encoding="utf-8"))
    ours = {}
    for e in d["unified_entries"]:
        w, g = e.get("wylie"), e.get("hgm_gloss")
        if w and g and w not in ours:
            ours[w] = [x for x in g if isinstance(x, str)]

    con = sqlite3.connect(arch)
    theirs = {}
    for tr, summ, defn in con.execute(
            "SELECT transliteration, summary, definition_text FROM records"):
        if not tr or tr in theirs:
            continue
        q = [norm(x) for x in
             re.findall(r'"([^"]{2,60})"', (summ or "") + " " + (defn or ""))]
        q = [x for x in q if x]
        if q:
            theirs[tr] = q

    compared = 0
    hits = []
    for w, g in ours.items():
        if w not in theirs or not g:
            continue
        compared += 1
        first = norm(g[0])
        if not first or not closed_only(first):
            continue
        ill = theirs[w]
        if any(closed_only(x) for x in ill):      # they agree it is a function word
            continue
        hits.append((w, g, ill))

    print("  glossed headwords in the master ........... %d" % len(ours))
    print("  also carrying quoted equivalents there .... %d" % compared)
    print("  OUR first equivalent is closed-class where")
    print("  the Illuminator's are content words ....... %d" % len(hits))
    print()
    print("  These are CANDIDATES for a reader, not findings. Expect homographs")
    print("  (shing = and / tree; nas = from / barley) and the Illuminator's")
    print("  example sentences to be among them. Illuminator text is quoted")
    print("  briefly as evidence and is not written to any file.")
    print()
    for w, g, ill in sorted(hits):
        print("  %-22s ours: %s" % (w, ", ".join(g[:4])[:78]))
        print("  %-22s ill : %s" % ("", ", ".join(ill[:3])[:78]))
    return 0


if __name__ == "__main__":
    sys.exit(main())
