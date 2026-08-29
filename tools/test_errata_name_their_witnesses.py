#!/usr/bin/env python3
"""An erratum whose text also appears elsewhere must SAY where.

The register goes to ALL's document editors as a list of things to fix. The
single most dangerous way for it to be wrong is to report a PUBLISHED READING
as a local corruption — that is not a small error, it is us telling an editor
to change a text that is already correct.

The guard against it is the parallel witness: the same passage often appears
in C16, P1, EM or a TCS text, and if another witness carries the identical
string then the reading is either published, or the SAME DEFECT PROPAGATED to
both. Those two are genuinely different, and only a human reading the evidence
can tell them apart —

    E-092  C16:450 has the same "oo ka", AND its ACIP also has {SHl}. Same
           converter defect in both. The erratum stands, and is broader.
    E-095  C16:456 has the commas; C03:412 and P1:67 do not. Two witnesses
           to one. A textual VARIANT, not a defect. Downgraded to UNCERTAIN.

So this suite does NOT try to judge which case applies — a mechanical test
cannot, and the whole gate-trials programme records what happens when one
tries. It enforces the far weaker and entirely checkable thing: if a parallel
witness exists, the evidence must NAME IT by course:seq. An entry that quietly
fails to mention a witness it must have seen is an entry nobody checked.

Measured 2026-08-28: 27 of 77 document errata have their quoted text at
another segment, and all 27 name it. This holds that line.
"""
import importlib.util, io, json, os, re, sqlite3, sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SIDE = sys.argv[1] if len(sys.argv) > 1 else os.path.join(ROOT, 'docs/errata_register.json')
SPINE = sys.argv[2] if len(sys.argv) > 2 else os.path.join(ROOT, 'build/hgm_spine_v27_2.db')

# reuse the register's own quote-splitting so both suites agree on what a
# "quoted fragment" is
_spec = importlib.util.spec_from_file_location(
    '_q', os.path.join(ROOT, 'tools', 'test_errata_quotes_hold.py'))
Q = importlib.util.module_from_spec(_spec)
_spec.loader.exec_module(Q)

CITE = re.compile(r'\b([A-Z]{1,4}\d{0,2}:\d+)\b')
SEGID = re.compile(r'([A-Z][A-Za-z0-9]*):(\d+)')
MIN_FRAG = 18   # shorter than this and a coincidental match means nothing
SKIP_KINDS = ('NOT_AN_ERRATUM', 'UNVERIFIABLE', 'LAYER_ARTEFACT')

def main():
    reg = json.load(io.open(SIDE, encoding='utf-8'))
    rows = list(sqlite3.connect(SPINE).execute(
        'select course,seq,wylie,english,acip from corpus_segments'))
    blobs = [((c, s), ' ¶ '.join(x or '' for x in (w, e, a))) for c, s, w, e, a in rows]
    folded = [(k, Q.fold(b)) for k, b in blobs]

    checked = withwit = 0
    silent = []
    for e in reg:
        if e.get('kind') in SKIP_KINDS:
            continue
        m = SEGID.fullmatch((e.get('segment') or '').strip())
        if not m:
            continue
        own = (m.group(1), int(m.group(2)))
        frags = [f for f in Q.fragments(e.get('found')) if len(f) >= MIN_FRAG]
        if not frags:
            continue
        checked += 1
        longest = max(frags, key=len)
        lf = Q.fold(longest)
        others = [k for (k, b), (_, fb) in zip(blobs, folded)
                  if k != own and (longest in b or lf in fb)]
        if not others:
            continue
        withwit += 1
        named = set(CITE.findall((e.get('evidence') or '') + ' ' + (e.get('note') or '')))
        if not named:
            silent.append((e['item_id'], e['segment'], e.get('confidence'),
                           ['%s:%d' % o for o in others[:4]], longest[:60]))
    if silent:
        print('FAIL %d erratum/errata have a parallel witness and name none:'
              % len(silent))
        for iid, sid, conf, o, q in silent:
            print('      %s %-12s %-9s witness at %s' % (iid, sid, conf, o))
            print('           quote: %r' % q)
        return 1
    print('errata name their witnesses: %d entries with a checkable quote, '
          '%d have a parallel witness elsewhere in the spine, all %d name it'
          % (checked, withwit, withwit))
    return 0

if __name__ == '__main__':
    sys.exit(main())
