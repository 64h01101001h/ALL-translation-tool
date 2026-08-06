#!/usr/bin/env python3
"""build_sanskrit_reference.py — dump the canonical sanskrit_convert.py outputs
for every distinct IAST term in the Lokesh Chandra extract, so the C++ port can
be diff-tested against the Python oracle (engines_battery, battery F).

Output: build/sanskrit_reference.tsv with columns
  iast  acip  devanagari  pronunciation  nextletter  inputcode  ewts  tibetan  rev_iast
None results are stored as the marker ∅ (the C++ side must fail identically).

Usage:  python3 tools/build_sanskrit_reference.py
"""
import csv
import os
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, 'engines'))

import sanskrit_convert as sk           # noqa: E402
from hgm_tools import acip_to_ewts      # noqa: E402

LC_TSV = os.path.expanduser(
    '~/Desktop/HGM DICTIONARY - TRANSLATION APP/Input/LokeshChandra/'
    'lokesh_chandra_tib_skt.tsv')
OUT = os.path.join(ROOT, 'build', 'sanskrit_reference.tsv')
NONE = '∅'


def main():
    terms = set()
    with open(LC_TSV, encoding='utf-8') as f:
        for row in csv.DictReader(f, delimiter='\t'):
            for part in (row.get('sanskrit') or '').split(';'):
                t = part.strip()
                if t and '\t' not in t and '\n' not in t:
                    terms.add(t)
    # the standard-doc examples ride along
    terms.update(['pramāṇa', 'oṃ maṇi padme hūṃ'])
    n_fail = 0
    with open(OUT, 'w', encoding='utf-8') as out:
        for t in sorted(terms):
            acip = sk.iast_to_acip(t)
            deva = sk.iast_to_devanagari(t)
            pron = sk.iast_to_pronunciation(t)
            nl = sk.iast_to_nextletter(t)
            ic = sk.iast_to_inputcode(t)
            ewts = acip_to_ewts(acip) if acip is not None else None
            tib = sk.iast_to_tibetan(t)
            rev = sk.acip_to_iast(acip) if acip is not None else None
            if acip is None:
                n_fail += 1
            row = [t] + [NONE if x is None else x
                         for x in (acip, deva, pron, nl, ic, ewts, tib, rev)]
            out.write('\t'.join(row) + '\n')
    print(f'wrote {OUT}: {len(terms)} terms ({n_fail} fail tokenization — '
          f'the port must fail on exactly the same ones)')


if __name__ == '__main__':
    main()
