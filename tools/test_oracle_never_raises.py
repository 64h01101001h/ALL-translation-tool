#!/usr/bin/env python3
"""The canonical Python engines must FLAG, never RAISE.

Rule 3 says a conversion that cannot be done is flagged as ⟨wylie⟩ and never
approximated. Raising is not flagging either: it is a third outcome the rule
does not allow, it cannot be ported faithfully — a C++ port can only turn it
into a coarse whole-input refusal — and any Python-side tooling over the
corpus dies on the row.

It happened. engines/ewts_unicode.py raised KeyError on 8 corpus rows where an
invalid character stranded a Sanskrit final (M anusvara, H visarga) in onset
position: tokenize emits those as consonant tokens though they are keys of
neither CONS nor SUB, and every lookup in native_onset and the explicit-stack
path indexed directly. The port meanwhile caught its own std::out_of_range and
flagged the WHOLE line with the raw un-normalised string, so the two engines
disagreed on exactly those rows while every battery stayed green — because no
battery ran the oracle over the corpus and asked whether it survived.

This does. It is not a correctness check; a flagged row is a pass. The only
failure it knows is an exception.
"""
import io
import os
import sqlite3
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, "engines"))

SPINE = os.path.join(ROOT, "build", "hgm_spine_v27_2.db")


def main():
    if not os.path.exists(SPINE):
        print("  [SKIP] no spine at %s — build/ is gitignored" % SPINE)
        return 0
    from ewts_unicode import wylie_to_unicode
    import hgm_tools

    con = sqlite3.connect(SPINE)
    checks = [
        ("wylie column, straight through the converter",
         "SELECT wylie FROM corpus_segments WHERE wylie IS NOT NULL",
         lambda v: wylie_to_unicode(v)),
        ("acip column, through acip_to_ewts and then the converter",
         "SELECT acip FROM corpus_segments WHERE acip IS NOT NULL",
         lambda v: wylie_to_unicode(hgm_tools.acip_to_ewts(v))),
    ]
    bad = 0
    for name, sql, fn in checks:
        n = raised = 0
        first = []
        for (v,) in con.execute(sql):
            n += 1
            try:
                fn(v)
            except Exception as e:                       # noqa: BLE001
                raised += 1
                if len(first) < 3:
                    first.append((type(e).__name__, str(e)[:40], v[:70]))
        if raised:
            bad += 1
            print("  [FAIL] %s: %d of %d rows RAISED" % (name, raised, n))
            for t, m, v in first:
                print("           %s: %s\n             on: %s" % (t, m, v))
        else:
            print("  [PASS] %s: %d rows, none raised" % (name, n))
    print("oracle never raises: %s" % ("ALL PASS" if not bad else "FAIL"))
    return 1 if bad else 0


if __name__ == "__main__":
    sys.exit(main())
