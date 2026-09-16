#!/usr/bin/env python3
"""build_forward_reference.py — dump the canonical ewts_unicode.py output for a
stride-3 sample of the master's wylie headwords, so the C++ forward engine can
be diff-tested against the Python oracle (forward_battery, F2 leg A).

Output: build/forward_reference.tsv, columns
  wylie  wylie_to_unicode(wylie)

Written 2026-09-16 to close the third of the four BUILD-7 rows in
docs/FIXTURES.md ("no generator in this repo" — the dump existed on one laptop
only, and a clone could not rebuild it, so forward_battery could not pass on a
fresh checkout).

The stride and the ordering are not decoration: forward_battery pins a RATCHET
against this exact sample, so a generator that picked a different 35,211
headwords would move the floor without anyone changing the engine. Verified by
reconstruction against the dump the battery had been using.

Regenerate whenever engines/ewts_unicode.py changes, or forward_battery will
diff the port against a stale oracle.

Usage:  python3 tools/build_forward_reference.py
"""
import gzip
import json
import os
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, 'engines'))

from ewts_unicode import wylie_to_unicode       # noqa: E402

MASTER = os.path.join(ROOT, 'data', 'hgm_dictionary_v27_2.json.gz')
OUT = os.path.join(ROOT, 'build', 'forward_reference.tsv')
STRIDE = 3
OFFSET = 2


def main():
    with gzip.open(MASTER, 'rt', encoding='utf-8') as f:
        master = json.load(f)
    # The selection is NOT "sorted distinct headwords, every third one". It is
    # every third ENTRY of the master in its stored order, OFFSET 2, with the
    # wylie of those entries then sorted -- duplicates kept, which is why the
    # file has 35,211 lines for 35,207 distinct headwords.
    #
    # That is worth spelling out because I got it wrong first and the mistake
    # was nearly invisible: a sorted-distinct stride-3 sample is also ~35,200
    # headwords of the same dictionary, and forward_battery PASSED against it.
    # It would have been a different 35,200, sharing barely a third of its rows
    # with the banked fixture, and the battery pins a RATCHET to this file --
    # so the floor would have moved with nobody touching the engine. The three
    # candidate offsets were distinguished by comparing the produced sequence
    # to the banked one, not by whether the suite went green.
    ents = master['unified_entries']
    sample = sorted(e['wylie'] for e in ents[OFFSET::STRIDE] if e.get('wylie'))
    os.makedirs(os.path.dirname(OUT), exist_ok=True)
    with open(OUT, 'w', encoding='utf-8') as out:
        for w in sample:
            # (unicode, ok) — the ok flag is carried in the text
            # itself as the \u27e8 \u27e9 refusal marker, so the
            # fixture needs the string alone.
            out.write(w + '\t' + wylie_to_unicode(w)[0] + '\n')
    print('wrote %s: %d rows from every %dth entry at offset %d'
          % (OUT, len(sample), STRIDE, OFFSET))


if __name__ == '__main__':
    main()
