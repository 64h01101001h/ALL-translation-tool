#!/usr/bin/env python3
"""Geshe Michael is named, not pronouned -- and only where Geshe Michael is the referent.

Adam's ruling, 2026-09-15: "no more referring to Geshe Michael's translations
as 'his words' or anything similar. this is the rule everywhere throughout this
project." Clarified twice: it is the pronouns HE is the referent of that
change, and it must not touch the dictionary or the aligned English.

The first attempt at this was a blanket regex over the tree. It named Geshe
Michael where Preston, Whitney, Wilson, Je Tsongkapa, the 1st Dalai Lama, the
user, the reader, Milarepa and Adam were meant, and it corrupted two build
scripts. It was reverted wholesale. This gate exists so the rule survives
without anyone reaching for that regex again.

What it will NOT look at, ever:
  - anything under data/ -- the dictionary and the alignment bank carry Geshe
    Michael's own English and are nobody's to edit
  - quoted source: '>' blocks and table rows in docs are verbatim transcript
    and verbatim rulings; the pronouns inside them belong to whoever the
    speaker was talking about
  - comments in code: that is where the wrong-referent risk lives, because the
    prose around this code discusses Preston, Wilson, Hopkins and Napper
  - docs/digests/ -- sent digests are the record of what leadership received

Everything it does flag is a line where a human must decide the referent. A
line judged correct goes in the baseline with its reason, keyed by content
hash, so renumbering a file does not silently re-arm it.
"""
import hashlib
import glob
import io
import os
import re
import sys

NAMES = re.compile(r"Geshe Michael|\bGMR\b|\bHGM\b")
PRON = re.compile(r"\b(his|he|He)\b")
IN_STRING = re.compile(r'"[^"]*\b(his|he|He)\b')


def key(path, text):
    """Identity is the sentence, not the line number."""
    return hashlib.sha256(
        (os.path.basename(path) + " " + " ".join(text.split())
         ).encode("utf-8")).hexdigest()[:16]


def scan(root):
    hits = []

    def code(pat, comment_prefix):
        for p in sorted(glob.glob(os.path.join(root, pat), recursive=True)):
            for i, line in enumerate(io.open(p, encoding="utf-8",
                                             errors="replace"), start=1):
                s = line.rstrip("\n")
                if s.lstrip().startswith(comment_prefix):
                    continue
                if IN_STRING.search(s):
                    hits.append((os.path.relpath(p, root), i, s.strip()))

    code("app/main.cpp", "//")
    code("app/*.inc", "//")
    code("ios/**/*.swift", "//")
    # core/ was outside this gate entirely until 2026-09-17, and the whole
    # library and its 124 batteries sat in the blind spot. Eight violations
    # were there: "every drill carries his English", "it is his glossary
    # reading", "battery I covers his mantra corpus". Same test as above —
    # STRINGS only, not comments — because a CHECK message is read by a person
    # when it fails, and a gloss label in the library can reach the screen.
    code("core/src/*.cpp", "//")
    code("core/include/allcore/*.h", "//")
    code("core/tests/*.cpp", "//")

    # tools/ prints messages a human reads when a gate fails, and carries
    # docstrings that explain the rules. Same standard, narrower test: both the
    # name and the pronoun must be on the line, because a lone "he" in a
    # tools/ docstring is usually a worked example about somebody else.
    for p in sorted(glob.glob(os.path.join(root, "tools/*.py"))):
        for i, line in enumerate(io.open(p, encoding="utf-8",
                                         errors="replace"), start=1):
            s = line.rstrip("\n")
            if s.lstrip().startswith("#"):
                continue
            if NAMES.search(s) and PRON.search(s):
                hits.append((os.path.relpath(p, root), i, s.strip()))

    for p in sorted(glob.glob(os.path.join(root, "docs/**/*.md"),
                              recursive=True)):
        rel = os.path.relpath(p, root)
        digest = rel.startswith("docs/digests/")
        for i, line in enumerate(io.open(p, encoding="utf-8",
                                         errors="replace"), start=1):
            s = line.rstrip("\n")
            t = s.lstrip()
            if t.startswith(">") or t.startswith("|"):
                continue          # verbatim transcript, verbatim rulings
            # A DIGEST is flagged on the pronoun ALONE. Every digest is about
            # Geshe Michael from its first line, so requiring his name on the
            # same line misses most of it -- which is exactly what happened on
            # 2026-09-16: I wrote digest #8, the gate passed it, and it said
            # "a word he has glossed" and "only 61 were his" in the document
            # that goes to leadership. The gate that skips the most visible
            # artifact is the wrong way round.
            #
            # The sent digests are baselined, not edited: they are the record
            # of what leadership actually received, and rewriting that record
            # would be a worse fault than the wording.
            if PRON.search(s) if digest else (NAMES.search(s) and PRON.search(s)):
                hits.append((rel, i, s.strip()))
    return hits


def main():
    root = sys.argv[1] if len(sys.argv) > 1 else "."
    base_path = os.path.join(root, "tools/naming_baseline.txt")
    accepted = {}
    if os.path.exists(base_path):
        for ln in io.open(base_path, encoding="utf-8"):
            ln = ln.strip()
            if not ln or ln.startswith("#"):
                continue
            h, _, why = ln.partition("  ")
            accepted[h] = why

    hits = scan(root)
    fresh = [(p, i, t) for (p, i, t) in hits if key(p, t) not in accepted]

    if "--write-baseline" in sys.argv:
        with io.open(base_path, "w", encoding="utf-8") as f:
            f.write("# Lines where a pronoun sits beside Geshe Michael's name "
                    "and a human has ruled on the referent.\n"
                    "# Keyed by content, not line number: edit the sentence "
                    "and it comes back for a fresh ruling.\n"
                    "# Written by tools/naming_check.py --write-baseline.\n")
            for p, i, t in hits:
                f.write("%s  %s:%d  %s\n" % (key(p, t), p, i, t[:96]))
        print("naming: baseline written with %d ruled line(s)" % len(hits))
        return 0

    if fresh:
        print("naming: %d line(s) put a pronoun beside Geshe Michael's name"
              % len(fresh))
        for p, i, t in fresh[:40]:
            print("  FAIL %s:%d  %s" % (p, i, t[:110]))
        print("")
        print("  Decide the referent by reading, never by regex.")
        print("  If it is Geshe Michael, name him. If it is anyone else -- the")
        print("  user, the reader, Preston, Wilson, Milarepa, a test fixture --")
        print("  leave it and run:")
        print("      python3 tools/naming_check.py . --write-baseline")
        return 1
    print("naming: all %d pronoun-beside-the-name line(s) ruled on" % len(hits))
    return 0


if __name__ == "__main__":
    sys.exit(main())
