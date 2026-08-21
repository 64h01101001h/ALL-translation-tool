#!/usr/bin/env python3
"""validate_engines.py — run the reproducible validation batteries from CLAUDE.md
against the local engine copies before any porting work.

Batteries:
  1. ewts_unicode  — variant-aware exact match on the master's ground-truth
     (wylie, tibetan) pairs (entries WITHOUT tibetan_source, i.e. source-attested
     Tibetan). Documented reference: 98.88% on 26,318 pairs.
  2. pron_engine   — pronounce(wylie) vs the 1,308 card-attested pronunciations
     (pronunciation_source present). Card-attested text includes segmentation
     overrides where the card governs over the engine, so <100% is expected;
     mismatches are classified, not hidden.
  3. sanskrit_convert — the official ACIP standard-doc examples (pramāṇa in all
     three styles) and the documented chain demo (oṃ maṇi padme hūṃ).
     NOTE: the 4,582-pair LC battery's pair-selection script is NOT banked in the
     Handoff and is not reproducible from artifacts on disk; flagged for Adam.
"""
import json, gzip, re, sys, collections, unicodedata
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / 'engines'))

from ewts_unicode import wylie_to_unicode
from pron_engine import pronounce
import sanskrit_convert as sk

MASTER = ROOT / 'data' / 'hgm_dictionary_v27_2.json.gz'


def load_master():
    with gzip.open(MASTER, 'rt', encoding='utf-8') as f:
        return json.load(f)


def norm_tib(s):
    """Normalize Tibetan for comparison: NFC, strip spaces and trailing tsheg/shad."""
    s = unicodedata.normalize('NFC', (s or '').strip())
    s = s.replace(' ', '').replace(' ', '')
    return s.rstrip('་།')


def battery_ewts(entries):
    gt = [e for e in entries if not e.get('tibetan_source')
          and e.get('tibetan') and e.get('wylie')]
    exact = variant = fail = 0
    fails = []
    for e in gt:
        # the ground-truth tibetan field may carry several renderings separated
        # by shad — variant-aware match accepts any of them
        targets = {norm_tib(t) for t in re.split(r'[།༔]', e['tibetan'])
                   if norm_tib(t)}
        targets.add(norm_tib(e['tibetan']))
        cands = [e['wylie']] + [v for v in (e.get('wylie_variants') or [])
                                if v and v != e['wylie']]
        hit = None
        for i, w in enumerate(cands):
            u, ok = wylie_to_unicode(w)
            if norm_tib(u) in targets:
                hit = 'exact' if i == 0 else 'variant'
                break
        if hit == 'exact':
            exact += 1
        elif hit == 'variant':
            variant += 1
        else:
            fail += 1
            if len(fails) < 12:
                u, ok = wylie_to_unicode(e['wylie'])
                fails.append((e['wylie'], e['tibetan'], u))
    n = len(gt)
    m = exact + variant
    print(f'\n=== BATTERY 1: ewts_unicode (wylie → Tibetan unicode) ===')
    print(f'ground-truth pairs (no tibetan_source): {n:,}')
    print(f'variant-aware exact match: {m:,} ({m/n*100:.2f}%)  '
          f'[headword-exact {exact:,}, via variant {variant:,}]')
    print(f'mismatches: {fail:,}   (reference: 98.88% on 26,318 pairs)')
    print('sample mismatches (wylie | ground truth | engine):')
    for w, t, u in fails[:8]:
        print(f'  {w!r:40s} | {t} | {u}')
    return n, m


def battery_pron(entries):
    card = [e for e in entries if e.get('pronunciation_source')]
    ok = fail = 0
    diffs = collections.Counter()
    samples = []
    for e in card:
        got = pronounce(e['wylie'])
        want = e['pronunciation']
        if got == want:
            ok += 1
        else:
            fail += 1
            # classify: same letters different segmentation vs phoneme-level
            kind = ('segmentation-only'
                    if got.replace(' ', '').replace('-', '') ==
                       (want or '').replace(' ', '').replace('-', '')
                    else 'phoneme-level')
            diffs[kind] += 1
            if len(samples) < 12:
                samples.append((e['wylie'], want, got, kind))
    n = len(card)
    print(f'\n=== BATTERY 2: pron_engine vs card-attested pronunciations ===')
    print(f'card-attested entries (pronunciation_source set): {n:,}')
    print(f'engine reproduces card text exactly: {ok:,} ({ok/n*100:.2f}%)')
    print(f'differences: {fail:,}  {dict(diffs)}')
    print('  (card-governed segmentation overrides are banked as evidence with the')
    print('   engine deliberately untouched — differences are expected, not drift)')
    print('sample differences (wylie | card | engine | class):')
    for w, want, got, kind in samples[:10]:
        print(f'  {w!r} | {want!r} | {got!r} | {kind}')
    return n, ok


def battery_sanskrit():
    print(f'\n=== BATTERY 3: sanskrit_convert — official standard-doc examples ===')
    results = []
    # ACIP Sanskrit Input Code Standards doc examples (CLAUDE.md):
    checks = [
        ('input-code   pramāṇa', sk.iast_to_inputcode('pramāṇa'), 'prama#n%a'),
        ('next-letter  pramāṇa', sk.iast_to_nextletter('pramāṇa'), 'prama-n.a'),
        ('tibetanized  pramāṇa', sk.iast_to_acip('pramāṇa'), "PRA M'A nA"),
    ]
    # chain demo from the roadmap
    mani = 'oṃ maṇi padme hūṃ'
    checks.append(('tibetanized  oṃ maṇi padme hūṃ', sk.iast_to_acip(mani),
                   "AOm MA nI PAD ME H'Um"))
    checks.append(('tibetan      oṃ maṇi padme hūṃ',
                   sk.iast_to_tibetan(mani), 'ཨོཾ་མ་ཎི་པད་མེ་ཧཱུཾ'))
    checks.append(('devanagari   oṃ maṇi padme hūṃ',
                   sk.iast_to_devanagari(mani), 'ओं मणि पद्मे हूं'))
    # round trip
    checks.append(('roundtrip    inputcode→iast', sk.inputcode_to_iast('prama#n%a'),
                   'pramāṇa'))
    npass = 0
    for label, got, want in checks:
        ok = (got or '').replace('​', '') == want
        npass += ok
        results.append(ok)
        print(f'  [{"PASS" if ok else "FAIL"}] {label}: {got!r}'
              + ('' if ok else f'   expected {want!r}'))
    print(f'standard-doc examples: {npass}/{len(checks)}')
    print('  NOTE: the 92.2%/4,582-pair LC battery is NOT reproducible from banked')
    print('  artifacts — the pair-selection script was never exported to the Handoff.')
    return len(checks), npass


def main():
    d = load_master()
    entries = d['unified_entries']
    print(f"master: {d['meta']['title']} v{d['meta']['version']} "
          f"({len(entries):,} unified entries)")
    battery_ewts(entries)
    battery_pron(entries)
    battery_sanskrit()


if __name__ == '__main__':
    main()
