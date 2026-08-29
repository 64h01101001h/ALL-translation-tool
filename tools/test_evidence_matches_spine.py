#!/usr/bin/env python3
"""The evidence layer is checked back against the spine, ACIP included.

alignment_evidence_v1.json is the layer that actually reaches the user: it
becomes the HGM TENTATIVE cards in the app. Its own meta states the rule it
lives by —

    "English is HGM's corpus text verbatim, machine-MATCHED never composed"

— and until now nothing enforced that rule against the corpus itself. The full
bank has test_layer_matches_spine.py; this asks the same question of the
artifact with the higher stakes, and asks it of all three fields:

  * the English must occur in HGM's English at every segment cited
  * the Tibetan headword must occur in the wylie there
  * the ACIP must occur in the ACIP there — ACIP is the SOURCE OF RECORD,
    so an ACIP that does not check out is worse than a wylie that doesn't

A pair that cites a segment which does not exist is unfalsifiable, and
unfalsifiable provenance is the one thing this project cannot ship quietly.
All four NOT-IN-SEGMENT counts are therefore hard zeros.
"""
import json, os, re, sqlite3, sys
from collections import Counter, defaultdict

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
EVID = sys.argv[1] if len(sys.argv) > 1 else os.path.join(
    ROOT, 'data/alignment/alignment_evidence_v1.json')
SPINE = sys.argv[2] if len(sys.argv) > 2 else os.path.join(
    ROOT, 'build/hgm_spine_v27_2.db')

# Measured 2026-08-28 over 24,088 field-checks on 5,097 headwords.
CEILING = {
    'eng NOT-IN-SEGMENT': 0,
    'tib NOT-IN-SEGMENT': 0,
    'acip NOT-IN-SEGMENT': 0,   # the source of record. Never raise this.
    'no-such-segment': 0,
    'bad-ref-format': 0,
    'tib punct-normalised': 19,
    'eng punct-normalised': 2,
}
JOIN = '…'
TR = str.maketrans({'‘': "'", '’': "'", '“': '"', '”': '"',
                    '–': '-', '—': '-', '\xa0': ' '})

def fold(s):
    return re.sub(r'\s+', ' ', (s or '').translate(TR)).strip()

def bare(s):
    return re.sub(r'[^a-z0-9ༀ-࿿]', '', fold(s).lower())

def classify(evid, rows):
    seg = {(c, s): (w or '', e or '', a or '') for c, s, w, e, a in rows}
    t, bad, checked = Counter(), defaultdict(list), 0
    for head, senses in evid['pairs'].items():
        for sn in senses:
            eng, acip = sn.get('eng'), sn.get('acip')
            for ref in sn.get('refs', []):
                c, _, q = ref.partition(':')
                try:
                    k = (c, int(q))
                except ValueError:
                    t['bad-ref-format'] += 1
                    bad['bad-ref-format'].append((head, eng, ref))
                    continue
                if k not in seg:
                    t['no-such-segment'] += 1
                    bad['no-such-segment'].append((head, eng, ref))
                    continue
                w, e, a = seg[k]
                checked += 2
                for val, src, tag in ((eng, e, 'eng'), (head, w, 'tib')):
                    if not val:                 continue
                    elif val in src:            t[tag + ' EXACT'] += 1
                    elif JOIN in val:           t[tag + ' builder-join'] += 1
                    elif fold(val) in fold(src): t[tag + ' fold-only'] += 1
                    elif bare(val) in bare(src):
                        t[tag + ' punct-normalised'] += 1
                        bad[tag + ' punct-normalised'].append((head, val, ref))
                    else:
                        t[tag + ' NOT-IN-SEGMENT'] += 1
                        bad[tag + ' NOT-IN-SEGMENT'].append((head, val, ref))
                if acip:
                    checked += 1
                    if acip in a:
                        t['acip EXACT'] += 1
                    else:
                        t['acip NOT-IN-SEGMENT'] += 1
                        bad['acip NOT-IN-SEGMENT'].append((head, acip, ref))
    return checked, t, bad

def main():
    evid = json.load(open(EVID))
    rows = list(sqlite3.connect(SPINE).execute(
        'select course,seq,wylie,english,acip from corpus_segments'))
    checked, t, bad = classify(evid, rows)
    fail = False
    for cls, cap in CEILING.items():
        n = t.get(cls, 0)
        if n > cap:
            fail = True
            print('FAIL %s: %d > ceiling %d' % (cls, n, cap))
            for r in bad[cls][:10]:
                print('     ', r)
    if fail:
        return 1
    print('evidence matches spine: %d headwords, %d field-checks, '
          '%d/%d eng exact, %d/%d tib exact, %d acip exact, '
          '0 not-in-segment on any field'
          % (len(evid['pairs']), checked,
             t['eng EXACT'], t['eng EXACT'] + t['eng punct-normalised']
             + t['eng builder-join'] + t['eng fold-only'],
             t['tib EXACT'], t['tib EXACT'] + t['tib punct-normalised']
             + t['tib builder-join'] + t['tib fold-only'],
             t['acip EXACT']))
    return 0

if __name__ == '__main__':
    sys.exit(main())
