#!/usr/bin/env python3
"""Cross-check pron_engine.py against the Kawachen reader's own phonology.

The Kawachen Tibetan Reader (itibet.org) indexes its recordings by SOUND, not
by spelling: every spelling that reads the same way is served the same file.
Decoding that index (see docs/KAWACHEN_READER_FORMAT.md) yields a set of
equivalence classes built by Tibetan teachers with no connection to the ACIP
conventions our engines follow — the first independent second opinion we have
on how pron_engine.py handles consonant stacks.

This script asks two questions of our engine:

  1. Where the site MERGES, do we merge?  If they serve one recording for
     kla, gla, bla, rla and sla, our engine should read all five the same.
     A split here is a genuine disagreement worth a ruling.

  2. Where the site SPLITS, do we split?  The site separates rga from ga
     because a superscript raises the tone.  GMR's convention does not mark
     tone, so we EXPECT to merge these, and that is recorded as an expected
     difference rather than a fault.

Nothing here changes the engine.  It reports, and the report is evidence for
a human decision.
"""
import os
import sys
from collections import defaultdict

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', 'engines'))
sys.path.insert(0, HERE)
from pron_engine import pronounce                       # noqa: E402
from kawachen_labels import STACKS                      # noqa: E402

# Consonant index -> the Wylie spellings the site serves from it, read from
# data/kawachen_labels.json so this table cannot drift from the manifest's.
# The stems there carry no inherent a (they are built to take a vowel), so it
# is added back here to make each one a pronounceable syllable.
CLASSES = {idx: [st + 'a' for st in stem.split('/')]
           for idx, (_spellings, stem) in STACKS.items() if stem}

# Spellings the site folds onto a plain root letter — a strong claim, since it
# says two quite different spellings are simply the same sound.
FOLDED = {'pya': 'ca', 'phya': 'cha', 'mya': 'nya'}

# The site gives these their own index only because of tone.  GMR's convention
# does not mark tone, so our engine is expected to merge each pair.
TONE_ONLY = [('rga', 'ga'), ('rnga', 'nga'), ('rja', 'ja'), ('rnya', 'nya'),
             ('rda', 'da'), ('rna', 'na'), ('rba', 'ba'), ('rma', 'ma'),
             ('rdza', 'dza')]


def main():
    splits, folds_ok, folds_bad = [], [], []

    print('=' * 66)
    print('1. Where the reader MERGES — does our engine agree?')
    print('=' * 66)
    for idx in sorted(CLASSES):
        spellings = CLASSES[idx]
        if len(spellings) < 2:
            continue
        reads = {w: pronounce(w) for w in spellings}
        distinct = set(reads.values())
        if len(distinct) == 1:
            print('  ok   %-3d %-34s -> %s'
                  % (idx, ', '.join(spellings), distinct.pop()))
        else:
            splits.append((idx, reads))
            print('  DIFF %-3d %s' % (idx, ', '.join(spellings)))
            for w, p in reads.items():
                print('           %-8s -> %s' % (w, p))

    print()
    print('=' * 66)
    print('2. Spellings the reader folds onto a root letter')
    print('=' * 66)
    for w, root in FOLDED.items():
        a, b = pronounce(w), pronounce(root)
        if a == b:
            folds_ok.append(w)
            print('  ok   %-6s = %-6s -> %s' % (w, root, a))
        else:
            folds_bad.append((w, root, a, b))
            print('  DIFF %-6s -> %-10s but %-6s -> %s' % (w, a, root, b))

    print()
    print('=' * 66)
    print('3. Tone-only splits — we EXPECT to merge these')
    print('=' * 66)
    unexpected = []
    for hi, lo in TONE_ONLY:
        a, b = pronounce(hi), pronounce(lo)
        mark = 'as expected' if a == b else 'we split too'
        if a != b:
            unexpected.append((hi, lo, a, b))
        print('  %-14s %-6s -> %-10s %-6s -> %s' % (mark, hi, a, lo, b))

    print()
    print('=' * 66)
    print('VERDICT')
    print('=' * 66)
    merged = sum(1 for i in CLASSES if len(CLASSES[i]) > 1)
    print('  reader classes with more than one spelling : %d' % merged)
    print('  ... where our engine agrees                : %d' % (merged - len(splits)))
    print('  ... where it disagrees                     : %d' % len(splits))
    print('  folds onto a root letter, agreed           : %d of %d'
          % (len(folds_ok), len(FOLDED)))
    print('  tone-only splits we also split             : %d of %d'
          % (len(unexpected), len(TONE_ONLY)))
    if splits or folds_bad:
        print('\n  Disagreements are listed above. They are not proof our engine')
        print('  is wrong — ACIP convention and Lhasa teaching practice differ')
        print('  in places by design — but each one deserves a ruling.')
    return 0


if __name__ == '__main__':
    sys.exit(main())
