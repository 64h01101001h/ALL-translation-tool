#!/usr/bin/env python3
"""seed_prenasal_proposals.py — put the 205 machine-derived prenasal
m-forms into the proposal channel as pending pronunciation proposals,
so the review (task #28) happens in the app's Approval pane instead of
a markdown sheet.

Provenance is honest: the proposer is the derivation engine, named as
such — these are machine-derived candidates awaiting the authority
(rule 3: derived is flagged, never promoted silently). On approval the
app upgrades the register row's class from prenasal-derived to
approved in place; on decline it removes the derived row.

Writes into data/proposals/ by default (point the app's proposals
folder there, or copy the file into the team's shared Dropbox folder).
Re-running is safe: rows already present (same kind+wylie+value) are
skipped.

Usage: python3 tools/seed_prenasal_proposals.py [proposals_dir]
"""
import csv
import sys
from datetime import date
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
REGISTER = ROOT / 'data' / 'pron_colloquial' / 'colloquial_pron.tsv'
OUT_DIR = Path(sys.argv[1]) if len(sys.argv) > 1 else (
    ROOT / 'data' / 'proposals')
OUT = OUT_DIR / 'proposals.tsv'

HEADER = ('# id\tkind\tstatus\tproposer\tcreated\twylie\tvalue\tfield\t'
          'evidence\tapprover\truled\tcomment')
PROPOSER = 'prenasal derivation engine (machine)'
EVIDENCE = ('machine-derived: b(s)-coda prenasal collapses to m keeping '
            'its labial place (HGM: SKABS \'DIR = kamdir), located by '
            'prefix-pronunciation alignment against the corpus; engine '
            'baseline says {baseline}; review sheet '
            'docs/PRENASAL_REGISTER.md')


def esc(s):
    return (s.replace('\\', '\\\\').replace('\t', '\\t')
             .replace('\n', '\\n').replace('\r', '\\r'))


def main():
    rows = []
    with open(REGISTER, encoding='utf-8') as f:
        for line in f:
            line = line.rstrip('\n')
            if not line or line.startswith('#'):
                continue
            c = line.split('\t')
            if len(c) >= 4 and c[3] == 'prenasal-derived':
                rows.append((c[0], c[1], c[2]))   # colloquial, wylie, gmr
    if not rows:
        print('no prenasal-derived rows found — nothing to seed')
        return 1

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    existing = set()
    lines = []
    if OUT.exists():
        for line in open(OUT, encoding='utf-8'):
            line = line.rstrip('\n')
            if not line:
                continue
            lines.append(line)
            if line.startswith('#'):
                continue
            c = line.split('\t')
            if len(c) >= 7:
                existing.add((c[1], c[5], c[6]))   # kind, wylie, value
    else:
        lines.append(HEADER)

    today = date.today().isoformat()
    added = 0
    seq = sum(1 for l in lines if not l.startswith('#'))
    for colloquial, wylie, gmr in rows:
        key = ('pronunciation', esc(wylie), esc(colloquial))
        if key in existing:
            continue
        seq += 1
        pid = f'pronunciation-{today}-{seq}'
        ev = EVIDENCE.format(baseline=gmr or '(none)')
        lines.append('\t'.join([
            pid, 'pronunciation', 'pending', esc(PROPOSER), today,
            esc(wylie), esc(colloquial), '', esc(ev), '', '', '']))
        added += 1

    with open(OUT, 'w', encoding='utf-8') as f:
        f.write('\n'.join(lines) + '\n')
    print(f'seeded {added} pending pronunciation proposal(s) '
          f'({len(rows) - added} already present) -> {OUT}')
    print('point the app\'s proposals folder at', OUT_DIR)
    return 0


if __name__ == '__main__':
    sys.exit(main())
