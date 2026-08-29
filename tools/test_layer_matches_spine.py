#!/usr/bin/env python3
"""Every banked field must be VERBATIM SOURCE — checked against the spine itself.

The four page gates check pages for internal well-formedness: no broken words,
no split syllables, no degenerate members, no phonetics. None of them ever asked
the question that actually matters for Rule 1 and Rule 10:

    does the text we SHIPPED still occur in the text we CLAIM it came from?

A page can be perfectly well-formed and still bank a string the source does not
contain — if it was built from a stale spine, or if the analyst quietly repaired
the source's punctuation while quoting it. That is exactly what this found in the
legacy C01/C02 pages, and nothing else was watching for it.

NOT-IN-CORPUS is a hard zero. It is Rule 10 mechanised at the layer: a banked
string absent from its own course is a falsehood, however well-formed.
The softer classes are ceilinged at their measured values so they cannot grow.
"""
import json, re, sqlite3, sys, os
from collections import Counter, defaultdict

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
# Taken as argv so CMake can pass them and BUILD-7's add_test wrapper can SEE
# the fixture dependency. A path hardcoded here would be invisible to the
# wrapper, and a checkout without the spine would crash instead of skipping.
LAYER = sys.argv[1] if len(sys.argv) > 1 else os.path.join(
    ROOT, 'data/alignment/alignment_full_v1.json')
SPINE = sys.argv[2] if len(sys.argv) > 2 else os.path.join(
    ROOT, 'build/hgm_spine_v27_2.db')

# Measured 2026-08-28 over 42,648 fields. Each is a legacy defect with an errata
# row; the ceiling stops the class SPREADING while the rows are worked off.
CEILING = {
    'punct-or-hyphen-normalised': 29,   # legacy C01/C02 silently repaired source punctuation
    'annotation-in-verbatim-field': 2,  # project commentary stored in a verbatim field
    'no-such-segment': 4,               # links carrying seg: None
    'stale-citation': 0,
    'NOT-IN-CORPUS': 0,                 # Rule 10. Never raise this.
}
# " … " is the BUILDER's join for one headword with two exponents on a page
# (build_alignment_layer.py:412) - a designed convention, not a quotation.
JOIN = '…'
TR = str.maketrans({'‘': "'", '’': "'", '“': '"', '”': '"', '–': '-', '—': '-', '\xa0': ' '})

def fold(s):
    return re.sub(r'\s+', ' ', (s or '').translate(TR)).strip()

def bare(s):
    return re.sub(r'[^a-z0-9ༀ-࿿]', '', fold(s).lower())

def classify(layer, spine_rows):
    seg = {(c, s): (w or '', e or '') for c, s, w, e in spine_rows}
    course = defaultdict(lambda: ['', ''])
    for c, s, w, e in spine_rows:
        course[c][0] += ' ¶ ' + fold(w)
        course[c][1] += ' ¶ ' + fold(e)
    tally, rec, checked = Counter(), defaultdict(list), 0
    for l in layer['links']:
        k = (l['course'], l['seg'])
        for fld, idx in (('tib', 0), ('eng', 1)):
            v = l.get(fld)
            if not v:
                continue
            checked += 1
            if l['seg'] is None or k not in seg:      verdict = 'no-such-segment'
            elif v in seg[k][idx]:                    verdict = 'EXACT'
            elif JOIN in v:                           verdict = 'builder-join'
            elif re.search(r'\[[A-Z][A-Z -]{3,}', v): verdict = 'annotation-in-verbatim-field'
            elif fold(v) in fold(seg[k][idx]):        verdict = 'whitespace/quote-fold-only'
            elif bare(v) in bare(seg[k][idx]):        verdict = 'punct-or-hyphen-normalised'
            elif bare(v) in bare(course[l['course']][idx]): verdict = 'stale-citation'
            else:                                     verdict = 'NOT-IN-CORPUS'
            tally[verdict] += 1
            if verdict not in ('EXACT', 'builder-join', 'whitespace/quote-fold-only'):
                rec[verdict].append((l['course'], l['seg'], l.get('page'), fld, v[:80]))
    return checked, tally, rec

def main():
    layer = json.load(open(LAYER))
    rows = list(sqlite3.connect(SPINE).execute(
        'select course,seq,wylie,english from corpus_segments'))
    checked, tally, rec = classify(layer, rows)
    fail = False
    for cls, cap in CEILING.items():
        n = tally.get(cls, 0)
        if n > cap:
            fail = True
            print(f'FAIL {cls}: {n} > ceiling {cap}')
            for r in rec[cls][:10]:
                print('     ', r)
    if fail:
        return 1
    pct = 100.0 * tally['EXACT'] / checked
    print(f'layer matches spine: {tally["EXACT"]}/{checked} fields exact ({pct:.3f}%), '
          f'0 fabrications; {sum(tally.get(c,0) for c in CEILING)} legacy at/under ceiling')
    return 0

if __name__ == '__main__':
    sys.exit(main())
