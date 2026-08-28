#!/usr/bin/env python3
"""Battery for span-level ACIP recovery (TODO.md item A).

The equivalents must key on ACIP, the source of record, not on the
derived Wylie. This proves that for any wrapped Wylie span we can
recover the exact ACIP substring it came from, and that the recovery
round-trips: acip_to_ewts(recovered) == the Wylie span it came from.

Refuses on any failure. Run: python3 tools/test_acip_recovery.py
"""
import os
import sqlite3
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, "engines"))
sys.path.insert(0, os.path.join(ROOT, "tools"))
from hgm_tools import acip_to_ewts                      # noqa: E402
from build_alignment_layer import acip_span, norm_w     # noqa: E402

SPINE = os.path.join(ROOT, "build", "hgm_spine_v27_2.db")

# (course, seq, wylie span, expected ACIP span) — hand-checked
CASES = [
    ("C01", 485, "bshes gnyen bsten tshul", "BSHES GNYEN BSTEN TSUL"),
    ("C01", 485, "rtsa ba",                 "RTZA BA"),
    ("C01", 485, "sbyor ba'i chos drug",    "SBYOR BA'I CHOS DRUG"),
    ("C01", 485, "mchod brjod",             "MCHOD BRJOD"),
    ("C03",   2, "me tog",                  "ME TOG"),
    ("C03",   2, "spos kyis",               "SPOS KYIS"),
    ("C03",  25, "gser gling pa'i",         "GSER GLING PA'I"),
    ("C03",  25, "sku gsung thugs",         "SKU GSUNG THUGS"),
    # REGRESSION (2026-08-28): "shog" also occurs inside "tshogs"
    # (t-SHOG-s). Naive first-hit recovered the ACIP of TSOGS and the
    # round-trip refused it. Token-boundary hits must be tried first.
    ("C01", 339, "shog",                     "SHOG"),
]

# Known limit, pinned so it is a decision and not a surprise: the
# Sanskrit long-vowel apostrophe (sh'A -> ShA) does not survive
# prefix-offset conversion, so this span has no provable ACIP form and
# acip_span returns None rather than guessing (rule 3). 1 of 4,039
# headwords as of 2026-08-28.
KNOWN_NONE = [("C01", 41, "manydzu goh ShA")]

def main():
    con = sqlite3.connect(SPINE)
    fails = []
    for course, seq, wspan, want in CASES:
        row = con.execute("SELECT acip, wylie FROM corpus_segments "
                          "WHERE course=? AND seq=?",
                          (course, seq)).fetchone()
        if not row:
            fails.append("%s:%d missing from spine" % (course, seq))
            continue
        acip, wyl = row
        got = acip_span(acip, wyl, wspan)
        if got != want:
            fails.append("%s:%d %r -> %r, wanted %r"
                         % (course, seq, wspan, got, want))
            continue
        # the round-trip is the actual proof, not the literal match
        back = norm_w(acip_to_ewts(got))
        if back != norm_w(wspan):
            fails.append("%s:%d round-trip: %r -> %r != %r"
                         % (course, seq, got, back, wspan))
    for course, seq, wspan in KNOWN_NONE:
        acip, wyl = con.execute(
            "SELECT acip, wylie FROM corpus_segments "
            "WHERE course=? AND seq=?", (course, seq)).fetchone()
        got = acip_span(acip, wyl, wspan)
        if got is not None:
            fails.append("%s:%d %r was expected to have NO provable "
                         "ACIP form but recovered %r — if the recovery "
                         "improved, move it into CASES"
                         % (course, seq, wspan, got))
    con.close()
    if fails:
        for f in fails:
            print("FAIL " + f)
        sys.exit("acip recovery battery: %d/%d FAILED"
                 % (len(fails), len(CASES)))
    print("acip recovery battery: %d/%d pass, %d known-none held"
          % (len(CASES), len(CASES), len(KNOWN_NONE)))

if __name__ == "__main__":
    main()
