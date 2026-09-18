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
BENIGN = {'eng EXACT', 'tib EXACT', 'acip EXACT', 'eng builder-join',
          'tib builder-join', 'acip builder-join', 'eng fold-only',
          'tib fold-only', 'acip fold-only'}
CEILING = {
    'eng NOT-IN-SEGMENT': 0,
    'tib NOT-IN-SEGMENT': 0,
    'acip NOT-IN-SEGMENT': 0,   # the source of record. Never raise this.
    'no-such-segment': 0,
    'bad-ref-format': 0,
    'tib punct-normalised': 0,
    'eng punct-normalised': 2,
}
JOIN = '…'
TR = str.maketrans({'‘': "'", '’': "'", '“': '"', '”': '"',
                    '–': '-', '—': '-', '\xa0': ' '})

def fold(s):
    return re.sub(r'\s+', ' ', (s or '').translate(TR)).strip()

def bare(s, tibetan=False):
    """Strip punctuation and spacing so a quoting artefact does not read as a
    missing string. NEVER case-folds Tibetan: in Wylie a capital is a DIFFERENT
    LETTER (N is retroflex Na, Sh is retroflex sha, A/I/U are the long vowels),
    so lowercasing hides a changed consonant as a punctuation difference. It
    did exactly that - 16 headwords, among them paN chen, nA ro pa, shA ri'i bu
    and maNDla, were shipped lowercased and this gate scored them as clean."""
    s = fold(s)
    if not tibetan:
        s = s.lower()
    return re.sub(r'[^A-Za-z0-9ༀ-࿿]' if tibetan else r'[^a-z0-9ༀ-࿿]', '', s)

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
                    elif bare(val, tag == 'tib') in bare(src, tag == 'tib'):
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

def _spine_usable(path):
    """Present is not enough: sqlite3.connect CREATES the file, so the first
    check to run leaves an empty database behind and every later one finds it
    'there'. That is how all five of these went from a clean skip to an
    unhandled OperationalError on a fresh clone. Ask for the table."""
    import os as _os
    import sqlite3 as _sq
    if not _os.path.exists(path):
        return False
    try:
        con = _sq.connect("file:%s?mode=ro" % path, uri=True)
        con.execute("SELECT 1 FROM corpus_segments LIMIT 1").fetchone()
        con.close()
        return True
    except Exception:
        return False


def main():
    if not _spine_usable(SPINE):
        print("  SKIPPED: no usable spine at %s — build/ is gitignored, so a\n             fresh clone has none. NOT a pass: nothing was checked." % SPINE)
        return 77
    evid = json.load(open(EVID))
    rows = list(sqlite3.connect(SPINE).execute(
        'select course,seq,wylie,english,acip from corpus_segments'))
    checked, t, bad = classify(evid, rows)
    fail = False
    # EVERY CLASS IS EITHER BENIGN OR CAPPED. builder-join is the BUILDER's
    # own " ... " convention for one headword with two exponents on a page, so
    # it is benign by design and named here rather than left implicit. Anything
    # else that appears in the tally has no ceiling, and an uncapped class is
    # an unbounded one: it can grow to any size while the gate prints a pass,
    # which is the same fault as a ceiling set above its measurement, one level
    # up. A new verdict string now has to be classified by a person.
    unknown = sorted(c for c in t
                     if c not in CEILING and c not in BENIGN)
    if unknown:
        fail = True
        print('UNCAPPED CLASS(ES) %s: tallied but neither benign nor under a '
              'ceiling. Add each to CEILING (with the measured count) or to '
              'BENIGN (with the reason it is not a defect).' % unknown)
    for cls, cap in CEILING.items():
        n = t.get(cls, 0)
        if n > cap:
            fail = True
            print('FAIL %s: %d > ceiling %d' % (cls, n, cap))
            for r in bad[cls][:10]:
                print('     ', r)
        elif cap and n < cap:
            # A CEILING ABOVE ITS MEASUREMENT IS REGRESSION ROOM NOBODY
            # GRANTED. 2026-09-18: test_no_supplied_span_head was carrying one
            # unit of it, set by me in the very commit that fixed that gate's
            # blindness, and it printed as a pass for a day. The audit found
            # the same shape twice more (forward_battery's floor under its own
            # measured value; no_phonetics_in_layer at 0.08% against a 2%
            # ceiling, twenty-five times headroom). A ceiling only ratchets if
            # something makes it follow the measurement down.
            fail = True
            print('SLACK %s: measured %d, ceiling %d. Tighten the ceiling to '
                  '%d in a commit -- %d more could land in silence.'
                  % (cls, n, cap, n, cap - n))
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
