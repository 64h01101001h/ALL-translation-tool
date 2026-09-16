#!/usr/bin/env python3
"""build_pron_reference.py — dump the canonical pron_engine.py output for every
distinct wylie headword in the master, so the C++ port can be diff-tested
against the Python oracle (engines_battery, battery pron).

Output: build/pron_reference.tsv, sorted, columns
  wylie  pronounce(wylie)

Written 2026-09-10 to close the BUILD-7 gap docs/FIXTURES.md recorded for this
fixture ("no generator in this repo" — the dump existed on one laptop only).
Verified by reconstruction: run against the pre-fix engine it reproduced the
banked 2026-08-05 fixture on 105,576/105,576 rows, byte for byte.

Regenerate whenever pron_engine.py changes, or engines_battery will diff the
port against a stale oracle.

Usage:  python3 tools/build_pron_reference.py
"""
import gzip
import json
import os
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, 'engines'))

from pron_engine import pronounce       # noqa: E402

MASTER = os.path.join(ROOT, 'data', 'hgm_dictionary_v27_2.json.gz')
OUT = os.path.join(ROOT, 'build', 'pron_reference.tsv')


def main():
    with gzip.open(MASTER, 'rt', encoding='utf-8') as f:
        master = json.load(f)
    terms = sorted({e['wylie'] for e in master['unified_entries']
                    if e.get('wylie')})
    os.makedirs(os.path.dirname(OUT), exist_ok=True)
    with open(OUT, 'w', encoding='utf-8') as out:
        for w in terms:
            out.write(w + '\t' + pronounce(w) + '\n')
    print(f'wrote {OUT}: {len(terms)} wylie headwords')


if __name__ == '__main__':
    main()
