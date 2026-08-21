#!/usr/bin/env python3
"""validate_release.py — gate a new HGM release package before the app
imports it (leadership recommendation #8).

The app imports releases and never owns the data; this is the customs
check at that border. Given a candidate package (master json.gz +
parallel corpus json.gz + reverse index json), it verifies:

  A. STRUCTURE   — files parse; meta carries title/version/date; the
                   master carries unified_entries; shapes are right.
  B. PROVENANCE  — tier and status vocabularies contain no unknown
                   values (rule 4: provenance is sacred — a new tag is
                   surfaced, never silently imported); tibetan_source
                   values are known; hgm_gloss is always a list of
                   strings and only tiered entries carry one (rule 1).
  C. NO SHRINK   — entries, glossed entries, corpus segments, courses,
                   and reverse-index keys never decrease versus the
                   currently banked release without --allow-shrink
                   (data loss must be a deliberate, stated decision).
  D. ENGINES     — the wylie→unicode engine is spot-run against the
                   candidate's own ground-truth pairs (tibetan_source
                   empty), variant-aware, same standard as the full
                   battery (reference 98.88%); below --ewts-threshold
                   (default 98.0) is a FAIL.
  E. CORPUS      — every segment carries course/acip/wylie/english;
                   no empty course names; per-course counts reported.

Exit 0 = PASS (import may proceed); exit 1 = FAIL with reasons.

Usage:
  python3 tools/validate_release.py                       # validate banked
  python3 tools/validate_release.py --master NEW.json.gz \
      --corpus NEW_corpus.json.gz --reverse NEW_rev.json  # gate a candidate
"""
import argparse
import gzip
import json
import random
import re
import sys
import unicodedata
from collections import Counter
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
DATA = ROOT / 'data'
sys.path.insert(0, str(ROOT / 'engines'))

BANKED_MASTER = DATA / 'hgm_dictionary_v27_2.json.gz'
BANKED_CORPUS = DATA / 'full_parallel_corpus_v32.json.gz'
BANKED_REVERSE = DATA / 'hgm_reverse_index_v27_2.json'

KNOWN_TIERS = {None, 'curated', 'glossary', 'auto-aligned'}
KNOWN_TIB_SOURCES_RE = re.compile(r'^generated-ewts-v\d+-\d+$')

failures = []
warnings = []


def fail(msg):
    failures.append(msg)
    print(f'  [FAIL] {msg}')


def warn(msg):
    warnings.append(msg)
    print(f'  [warn] {msg}')


def ok(msg):
    print(f'  [ok]   {msg}')


def load_json(path):
    if str(path).endswith('.gz'):
        with gzip.open(path, 'rt', encoding='utf-8') as f:
            return json.load(f)
    return json.load(open(path, encoding='utf-8'))


def norm_tib(s):
    s = unicodedata.normalize('NFC', (s or '').strip())
    s = s.replace(' ', '').replace(' ', '')
    return s.rstrip('་།')


def check_structure(master, corpus, reverse):
    print('\n== A. structure ==')
    meta = master.get('meta') or {}
    for k in ('title', 'version', 'date'):
        if meta.get(k):
            ok(f'meta.{k} = {meta[k]!r}' if k != 'title'
               else f'meta.title present')
        else:
            fail(f'meta.{k} missing or empty')
    if not re.match(r'^\d+\.\d+(\.\d+)?$', str(meta.get('version', ''))):
        fail(f'meta.version {meta.get("version")!r} is not N.N[.N]')
    entries = master.get('unified_entries')
    if not isinstance(entries, list) or not entries:
        fail('unified_entries missing or empty')
        return None
    ok(f'{len(entries):,} unified entries')
    if not isinstance(corpus, list) or not corpus:
        fail('corpus is not a non-empty list')
    else:
        ok(f'{len(corpus):,} corpus segments')
    if not isinstance(reverse, dict) or not reverse:
        fail('reverse index is not a non-empty dict')
    else:
        ok(f'{len(reverse):,} reverse-index English keys')
    return entries


def check_provenance(entries):
    print('\n== B. provenance ==')
    tiers = Counter()
    statuses = Counter()
    tib_sources = Counter()
    bad_gloss_shape = 0
    gloss_without_tier = 0
    glossed = 0
    no_wylie = 0
    for e in entries:
        if not (e.get('wylie') or '').strip():
            no_wylie += 1
        ev = e.get('hgm_evidence') or {}
        tiers[ev.get('tier')] += 1
        statuses[e.get('status')] += 1
        tib_sources[e.get('tibetan_source')] += 1
        # gloss data model: None / '' / [] all mean "awaiting an HGM
        # equivalent"; a BINDING gloss is a non-empty list of non-empty
        # strings, and must carry an evidence tier (rule 1)
        g = e.get('hgm_gloss')
        if g:
            glossed += 1
            if not (isinstance(g, list) and
                    all(isinstance(x, str) and x.strip() for x in g)):
                bad_gloss_shape += 1
            if ev.get('tier') is None:
                gloss_without_tier += 1
        elif g is not None and not isinstance(g, (list, str)):
            bad_gloss_shape += 1
    if no_wylie:
        fail(f'{no_wylie} entries with empty wylie headword')
    else:
        ok('every entry has a wylie headword')
    unknown_tiers = set(tiers) - KNOWN_TIERS
    if unknown_tiers:
        fail(f'unknown tier value(s): {sorted(unknown_tiers)} — '
             'the app must be taught these before import')
    else:
        ok(f'tier vocabulary known: '
           f'{ {k: v for k, v in tiers.items() if k} }')
    unknown_src = [s for s in tib_sources
                   if s is not None and not KNOWN_TIB_SOURCES_RE.match(s)]
    if unknown_src:
        fail(f'unknown tibetan_source tag(s): {unknown_src}')
    else:
        ok(f'tibetan_source tags known '
           f'(attested: {tib_sources.get(None, 0):,}, generated: '
           f'{sum(v for k, v in tib_sources.items() if k):,})')
    # status vocabulary drifts legitimately as the data project works —
    # surface new values loudly, but do not fail the gate on them
    known_status = {'hgm-attested', 'awaiting-hgm-equivalent',
                    'hgm-auto (mined)', 'hgm-auto (quoted)',
                    'hgm-auto (direct)',
                    'awaiting-hgm-equivalent (candidate available)',
                    'hgm-attested (not in Great Dictionary)'}
    new_status = set(statuses) - known_status - {None}
    if new_status:
        warn(f'NEW status value(s) — confirm with the data project: '
             f'{sorted(new_status)}')
    else:
        ok('status vocabulary matches the known set')
    if bad_gloss_shape:
        fail(f'{bad_gloss_shape} non-empty hgm_gloss fields are not '
             'lists of non-empty strings (rule 1 shape)')
    else:
        ok(f'all {glossed:,} binding hgm_gloss fields are lists of '
           'strings')
    if gloss_without_tier:
        fail(f'{gloss_without_tier} entries carry a binding hgm_gloss '
             'but no evidence tier — binding English without provenance')
    else:
        ok('every binding-glossed entry carries an evidence tier')
    return glossed


def check_no_shrink(entries, glossed, corpus, reverse, allow_shrink):
    print('\n== C. no shrink vs banked release ==')
    try:
        base_master = load_json(BANKED_MASTER)
        base_corpus = load_json(BANKED_CORPUS)
        base_reverse = load_json(BANKED_REVERSE)
    except FileNotFoundError as ex:
        warn(f'banked baseline unavailable ({ex}) — shrink check skipped')
        return
    base_entries = base_master.get('unified_entries') or []
    base_glossed = sum(1 for e in base_entries if e.get('hgm_gloss'))
    base_courses = {s.get('course') for s in base_corpus}
    courses = {s.get('course') for s in corpus}
    checks = [
        ('entries', len(entries), len(base_entries)),
        ('glossed entries', glossed, base_glossed),
        ('corpus segments', len(corpus), len(base_corpus)),
        ('corpus courses', len(courses), len(base_courses)),
        ('reverse-index keys', len(reverse), len(base_reverse)),
    ]
    for name, now, before in checks:
        if now < before:
            msg = (f'{name} shrank: {before:,} → {now:,} '
                   f'(-{before - now:,})')
            if allow_shrink:
                warn(msg + '  [--allow-shrink]')
            else:
                fail(msg + '  (pass --allow-shrink if deliberate)')
        else:
            ok(f'{name}: {before:,} → {now:,}')
    lost_courses = base_courses - courses
    if lost_courses:
        (warn if allow_shrink else fail)(
            f'course(s) disappeared: {sorted(c for c in lost_courses if c)}')


def check_engines(entries, sample_n, threshold, seed):
    print('\n== D. engine spot-battery (wylie → unicode) ==')
    try:
        from ewts_unicode import wylie_to_unicode
    except ImportError as ex:
        fail(f'cannot import canonical engine ewts_unicode: {ex}')
        return
    gt = [e for e in entries if not e.get('tibetan_source')
          and e.get('tibetan') and e.get('wylie')]
    if not gt:
        fail('no ground-truth (source-attested) pairs to battery against')
        return
    rng = random.Random(seed)
    sample = gt if len(gt) <= sample_n else rng.sample(gt, sample_n)
    match = 0
    fails_sample = []
    for e in sample:
        targets = {norm_tib(t) for t in re.split(r'[།༔]', e['tibetan'])
                   if norm_tib(t)}
        targets.add(norm_tib(e['tibetan']))
        cands = [e['wylie']] + [v for v in (e.get('wylie_variants') or [])
                                if v and v != e['wylie']]
        hit = False
        for w in cands:
            u, _ = wylie_to_unicode(w)
            if norm_tib(u) in targets:
                hit = True
                break
        if hit:
            match += 1
        elif len(fails_sample) < 6:
            fails_sample.append(e['wylie'])
    pct = match / len(sample) * 100
    line = (f'{match:,}/{len(sample):,} variant-aware matches '
            f'({pct:.2f}%; threshold {threshold}%, reference 98.88%)')
    if pct >= threshold:
        ok(line)
    else:
        fail(line)
        print(f'         sample misses: {fails_sample}')


def check_corpus(corpus):
    print('\n== E. corpus integrity ==')
    missing = Counter()
    empty_course = 0
    for s in corpus:
        for k in ('course', 'acip', 'wylie', 'english'):
            if k not in s:
                missing[k] += 1
        if not (s.get('course') or '').strip():
            empty_course += 1
    if missing:
        fail(f'segments missing fields: {dict(missing)}')
    else:
        ok('every segment carries course/acip/wylie/english')
    if empty_course:
        fail(f'{empty_course} segments with empty course name')
    else:
        counts = Counter(s['course'] for s in corpus)
        ok(f'{len(counts)} courses; largest '
           f'{counts.most_common(1)[0][0]} ({counts.most_common(1)[0][1]:,} '
           f'segments)')


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('--master', default=str(BANKED_MASTER))
    ap.add_argument('--corpus', default=str(BANKED_CORPUS))
    ap.add_argument('--reverse', default=str(BANKED_REVERSE))
    ap.add_argument('--allow-shrink', action='store_true',
                    help='downgrade shrink failures to warnings '
                         '(deliberate, stated data removal)')
    ap.add_argument('--ewts-threshold', type=float, default=98.0)
    ap.add_argument('--ewts-sample', type=int, default=4000)
    ap.add_argument('--seed', type=int, default=27,
                    help='sample seed (fixed for reproducible gates)')
    args = ap.parse_args()

    print(f'validating release package:\n  master : {args.master}\n'
          f'  corpus : {args.corpus}\n  reverse: {args.reverse}')
    try:
        master = load_json(args.master)
        corpus = load_json(args.corpus)
        reverse = load_json(args.reverse)
    except Exception as ex:
        print(f'  [FAIL] package does not parse: {ex}')
        sys.exit(1)

    entries = check_structure(master, corpus, reverse)
    if entries is None:
        print('\nRELEASE GATE: FAIL (structure)')
        sys.exit(1)
    glossed = check_provenance(entries)
    self_check = (Path(args.master).resolve() == BANKED_MASTER.resolve())
    check_no_shrink(entries, glossed, corpus, reverse,
                    allow_shrink=args.allow_shrink or self_check)
    check_engines(entries, args.ewts_sample, args.ewts_threshold, args.seed)
    check_corpus(corpus)

    print()
    for w in warnings:
        print(f'WARNING: {w}')
    if failures:
        print(f'\nRELEASE GATE: FAIL — {len(failures)} problem(s); '
              'do NOT import')
        sys.exit(1)
    print(f'\nRELEASE GATE: PASS — safe to rebuild the spine '
          f'(tools/build_spine.py)'
          + (f'  [{len(warnings)} warning(s) above]' if warnings else ''))
    sys.exit(0)


if __name__ == '__main__':
    main()
