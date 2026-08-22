#!/usr/bin/env python3
"""spine_query.py — smoke-test CLI for the Diamond Cutter Translation Tool SQLite spine.

Usage:
  spine_query.py lookup  <headword>            wylie / Tibetan unicode / ACIP
  spine_query.py english <words...>            English reverse search
  spine_query.py corpus  <fts5-query> [-c COURSE] [-n LIMIT]
                                               e.g. 'bden pa' · 'sdug OR bsngal'
                                               · 'NEAR(byang sems, 5)'
  spine_query.py stats

Tier display is honest per project rule: auto-aligned glosses are PROVISIONAL
and are labeled as such; Hopkins/{C}/{PH}/{GD}/{LC} material is reference only.
"""
import argparse, json, re, sqlite3, sys
from pathlib import Path

DB = Path(__file__).resolve().parent.parent / 'build' / 'hgm_spine_v27_2.db'

TIER_LABEL = {
    'curated': 'HGM (curated)',
    'glossary': 'HGM (glossary)',
    'auto-aligned': 'PROVISIONAL (auto-aligned — machine match, unreviewed)',
    None: '',
}


def connect():
    if not DB.exists():
        sys.exit(f'spine not built: {DB} — run tools/build_spine.py first')
    con = sqlite3.connect(f'file:{DB}?mode=ro', uri=True)
    con.row_factory = sqlite3.Row
    return con


def fts_quote(term):
    """Quote each bare token for FTS5 (keeps ' safe); leaves operators alone."""
    parts = []
    for tok in term.split():
        if tok in ('OR', 'AND', 'NOT') or tok.startswith('NEAR('):
            parts.append(tok)
        else:
            parts.append('"' + tok.replace('"', '""') + '"')
    return ' '.join(parts)


def print_entry(con, row, verbose=True):
    e = json.loads(row['raw'])
    tib = row['tibetan'] or ''
    gen = ' [generated script]' if row['tibetan_source'] else ''
    print(f"\n▶ {row['wylie']}   {tib}{gen}")
    if row['pronunciation']:
        src = ' ⟪card⟫' if row['pronunciation_source'] else ''
        print(f"  pron: {row['pronunciation']}{src}")
    if row['acip']:
        print(f"  acip: {row['acip']}")
    variants = [v['wylie'] for v in con.execute(
        'SELECT wylie FROM entry_variants WHERE entry_id=?', (row['id'],))]
    variants = [v for v in variants if v != row['wylie']]
    if variants:
        print(f"  variants: {', '.join(variants[:6])}")
    if row['hgm_gloss']:
        label = TIER_LABEL.get(row['tier'], row['tier'] or '')
        for g in json.loads(row['hgm_gloss']):
            print(f"  ≡ {g}    [{label}]")
    else:
        print(f"  (no HGM equivalent — status: {row['status']})")
    if verbose:
        if row['sanskrit']:
            print(f"  sanskrit (reference): {row['sanskrit'][:120]}")
        if row['hopkins_english']:
            print(f"  hopkins (reference): {row['hopkins_english'][:120]}")
        occ = e.get('corpus_occurrences')
        if occ:
            n = occ.get('n_segments')
            courses = occ.get('courses') or []
            print(f"  corpus: {n} segments in {len(courses)} sources")
        if row['flags']:
            print(f"  ⚑ flags: {', '.join(json.loads(row['flags']))}")


def cmd_lookup(args):
    con = connect()
    term = args.headword.strip()
    if re.search(r'[ༀ-࿿]', term):
        col, val = 'tibetan', term.rstrip('་།')
    elif re.fullmatch(r"[A-Z'’\-+: ]+", term) and re.search(r'[A-Z]', term):
        col, val = 'acip', term
    else:
        col, val = 'wylie', term
    rows = list(con.execute(f'SELECT * FROM entries WHERE {col}=?', (val,)))
    if not rows and col == 'wylie':
        rows = list(con.execute(
            'SELECT e.* FROM entry_variants v JOIN entries e ON e.id=v.entry_id '
            'WHERE v.wylie=?', (val,)))
        if rows:
            print(f'(matched via wylie variant)')
    if not rows:
        q = fts_quote(val)
        rows = list(con.execute(
            'SELECT e.* FROM entries_fts f JOIN entries e ON e.id=f.rowid '
            'WHERE entries_fts MATCH ? ORDER BY rank LIMIT ?',
            (q, args.limit)))
        if rows:
            print(f'(no exact {col} match — showing FTS matches)')
    if not rows:
        print('no match'); return
    for r in rows[:args.limit]:
        print_entry(con, r)


def cmd_english(args):
    con = connect()
    phrase = ' '.join(args.words).strip().lower()
    hits = list(con.execute(
        'SELECT * FROM reverse_index WHERE english=? ORDER BY '
        "CASE tier WHEN 'curated' THEN 0 WHEN 'glossary' THEN 1 ELSE 2 END",
        (phrase,)))
    if hits:
        print(f'reverse index — exact phrase "{phrase}":')
        for h in hits[:args.limit]:
            label = TIER_LABEL.get(h['tier'], h['tier'] or '')
            print(f"  {h['wylie']:30s} {h['pronunciation'] or '':24s} [{label}]")
    q = fts_quote(phrase)
    rows = list(con.execute(
        'SELECT e.* FROM english_fts f JOIN entries e ON e.id=f.rowid '
        'WHERE english_fts MATCH ? ORDER BY rank LIMIT ?',
        ('hgm_gloss:(' + q + ')', args.limit)))
    if rows:
        print(f'\nHGM glosses matching "{phrase}" (binding layer):')
        for r in rows:
            print_entry(con, r, verbose=False)
    else:
        rows = list(con.execute(
            'SELECT e.* FROM english_fts f JOIN entries e ON e.id=f.rowid '
            'WHERE english_fts MATCH ? ORDER BY rank LIMIT ?', (q, args.limit)))
        if rows:
            print(f'\n(no HGM-gloss match — reference layers only:)')
            for r in rows:
                print_entry(con, r, verbose=False)
        elif not hits:
            print('no match')


def cmd_corpus(args):
    con = connect()
    q = args.query if re.search(r'\bNEAR\(|"|\bOR\b|\bAND\b', args.query) \
        else fts_quote(args.query)
    sql = ('SELECT s.* FROM corpus_fts f JOIN corpus_segments s ON s.id=f.rowid '
           'WHERE corpus_fts MATCH ?')
    params = [q]
    if args.course:
        sql += ' AND s.course=?'; params.append(args.course)
    sql += ' ORDER BY rank LIMIT ?'; params.append(args.limit)
    rows = list(con.execute(sql, params))
    if not rows:
        print('no corpus match'); return
    for s in rows:
        print(f"\n[{s['course']}:{s['seq']}]")
        print(f"  T: {(s['wylie'] or '').strip()[:160]}")
        print(f"  E: {(s['english'] or '').strip()[:160]}")


def cmd_stats(args):
    con = connect()
    print('spine meta:')
    for k, v in con.execute('SELECT key, value FROM meta ORDER BY rowid'):
        print(f'  {k}: {v}')
    for label, sql in [
        ('entries', 'SELECT COUNT(*) FROM entries'),
        ('  with HGM gloss', 'SELECT COUNT(*) FROM entries WHERE hgm_gloss IS NOT NULL'),
        ('  tier curated', "SELECT COUNT(*) FROM entries WHERE tier='curated'"),
        ('  tier glossary', "SELECT COUNT(*) FROM entries WHERE tier='glossary'"),
        ('  tier auto-aligned (PROVISIONAL)',
         "SELECT COUNT(*) FROM entries WHERE tier='auto-aligned'"),
        ('  with source-attested Tibetan',
         'SELECT COUNT(*) FROM entries WHERE tibetan_source IS NULL AND tibetan IS NOT NULL'),
        ('  with generated Tibetan',
         'SELECT COUNT(*) FROM entries WHERE tibetan_source IS NOT NULL'),
        ('variants', 'SELECT COUNT(*) FROM entry_variants'),
        ('corpus segments', 'SELECT COUNT(*) FROM corpus_segments'),
        ('corpus courses', 'SELECT COUNT(DISTINCT course) FROM corpus_segments'),
        ('reverse-index rows', 'SELECT COUNT(*) FROM reverse_index'),
    ]:
        print(f'  {label}: {con.execute(sql).fetchone()[0]:,}')


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = ap.add_subparsers(dest='cmd', required=True)
    p = sub.add_parser('lookup'); p.add_argument('headword')
    p.add_argument('-n', '--limit', type=int, default=5); p.set_defaults(fn=cmd_lookup)
    p = sub.add_parser('english'); p.add_argument('words', nargs='+')
    p.add_argument('-n', '--limit', type=int, default=8); p.set_defaults(fn=cmd_english)
    p = sub.add_parser('corpus'); p.add_argument('query')
    p.add_argument('-c', '--course'); p.add_argument('-n', '--limit', type=int, default=8)
    p.set_defaults(fn=cmd_corpus)
    p = sub.add_parser('stats'); p.set_defaults(fn=cmd_stats)
    args = ap.parse_args()
    args.fn(args)


if __name__ == '__main__':
    main()
