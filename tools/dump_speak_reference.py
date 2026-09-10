#!/usr/bin/env python3
"""Bank the Python resolver's answers so the C++ port can be proved against them.

Founding rule 2: a port is transcribed from the canonical file and proven by a
battery. tools/kawachen_speak.py is the canonical resolver; this dumps what it
says for every distinct syllable in the corpus and the dictionary, and
core/tests/speak_battery.cpp must reproduce it exactly.
"""
import io
import os
import re
import sqlite3
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
from kawachen_speak import Bank                                   # noqa: E402
sys.path.insert(0, os.path.join(HERE, "..", "engines"))
from pron_engine import parse_syl                                 # noqa: E402

TIER = {1: "exact", 2: "silent", 3: "ending", 4: "reduced", None: "none"}


def syllables_from(db):
    seen = set()
    conn = sqlite3.connect(db)
    for q in ("SELECT wylie FROM corpus_segments",
              "SELECT wylie FROM entries WHERE wylie<>''"):
        for (t,) in conn.execute(q):
            for s in re.split(r"[\s/|]+", (t or "")):
                s = s.strip(".,;:!?()[]")
                if s and s not in seen and parse_syl(s):
                    seen.add(s)
    return sorted(seen)


def main():
    db = sys.argv[1] if len(sys.argv) > 1 else "build/hgm_spine_v27_2.db"
    out = sys.argv[2] if len(sys.argv) > 2 else "build/speak_reference.tsv"
    bank = Bank()
    syls = syllables_from(db)
    n = 0
    with io.open(out, "w", encoding="utf-8") as f:
        f.write("# syllable\ttier\tfiles (basenames, | separated)\n")
        for s in syls:
            files, tier = bank.resolve(s)
            names = "|".join(os.path.basename(x) for x in (files or []))
            f.write("%s\t%s\t%s\n" % (s, TIER[tier], names))
            n += 1
    print("wrote %d syllables -> %s" % (n, out))


if __name__ == "__main__":
    main()
