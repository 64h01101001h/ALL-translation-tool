#!/usr/bin/env python3
"""build_spine.py — compile the HGM release package + parallel corpus into the
ALL Translation Tool's SQLite (FTS5) spine database.

Roadmap item 1. The app IMPORTS a release; this builder is the importer.
Provenance rule: every entry's complete source JSON is preserved verbatim in
entries.raw — the relational columns are a query surface, never a replacement.

Inputs  (data/):  hgm_dictionary_v27_2.json.gz · full_parallel_corpus_v29.json.gz
                  hgm_reverse_index_v27_2.json
Output  (build/): hgm_spine_v27_2.db
"""
import json, gzip, sqlite3, sys, time
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
DATA = ROOT / 'data'
BUILD = ROOT / 'build'
MASTER = DATA / 'hgm_dictionary_v27_2.json.gz'
CORPUS = DATA / 'full_parallel_corpus_v29.json.gz'
REVERSE = DATA / 'hgm_reverse_index_v27_2.json'
DB = BUILD / 'hgm_spine_v27_2.db'

SCHEMA = """
PRAGMA journal_mode=WAL;

CREATE TABLE meta (key TEXT PRIMARY KEY, value TEXT);

CREATE TABLE entries (
  id INTEGER PRIMARY KEY,            -- stable ordinal in release order
  wylie TEXT NOT NULL,
  tibetan TEXT,
  tibetan_source TEXT,               -- NULL = source-attested; else generated-*
  acip TEXT,
  pronunciation TEXT,
  pronunciation_source TEXT,
  sanskrit TEXT,
  hopkins_english TEXT,
  hgm_gloss TEXT,                    -- JSON array; BINDING equivalents (rule 1)
  tier TEXT,                         -- curated | glossary | auto-aligned | NULL
  hgm_source TEXT,
  status TEXT,
  tenses TEXT,
  flags TEXT,                        -- JSON array
  corpus_courses TEXT,               -- JSON array
  corpus_n_segments INTEGER,
  raw TEXT NOT NULL                  -- complete source entry JSON, verbatim
);
CREATE INDEX idx_entries_wylie ON entries(wylie);
CREATE INDEX idx_entries_tibetan ON entries(tibetan);
CREATE INDEX idx_entries_acip ON entries(acip);
CREATE INDEX idx_entries_tier ON entries(tier);

CREATE TABLE entry_variants (
  entry_id INTEGER NOT NULL REFERENCES entries(id),
  wylie TEXT NOT NULL
);
CREATE INDEX idx_variants_wylie ON entry_variants(wylie);
CREATE INDEX idx_variants_entry ON entry_variants(entry_id);

-- headword search: wylie/variants/tibetan/acip/pronunciation
CREATE VIRTUAL TABLE entries_fts USING fts5(
  wylie, variants, tibetan, acip, pronunciation,
  content='', tokenize="unicode61 tokenchars ''''"
);

-- English-side search; column filters keep BINDING hgm_gloss separable from
-- reference layers (rule 1)
CREATE VIRTUAL TABLE english_fts USING fts5(
  hgm_gloss, hopkins_english, sanskrit,
  content='', tokenize='unicode61'
);

CREATE TABLE corpus_segments (
  id INTEGER PRIMARY KEY,
  course TEXT NOT NULL,
  seq INTEGER NOT NULL,              -- position within course (line number for NEAR)
  acip TEXT,
  wylie TEXT,
  english TEXT,
  raw TEXT NOT NULL
);
CREATE INDEX idx_corpus_course ON corpus_segments(course, seq);

CREATE VIRTUAL TABLE corpus_fts USING fts5(
  wylie, english, acip,
  content='', tokenize="unicode61 tokenchars ''''"
);

-- release reverse index (english phrase -> entries), kept as shipped
CREATE TABLE reverse_index (
  english TEXT NOT NULL,
  wylie TEXT NOT NULL,
  pronunciation TEXT,
  tier TEXT
);
CREATE INDEX idx_reverse_english ON reverse_index(english);
"""


def build():
    BUILD.mkdir(exist_ok=True)
    if DB.exists():
        DB.unlink()
    for w in BUILD.glob(DB.name + '-*'):
        w.unlink()
    con = sqlite3.connect(DB)
    con.executescript(SCHEMA)

    t0 = time.time()
    print(f'loading {MASTER.name} …')
    with gzip.open(MASTER, 'rt', encoding='utf-8') as f:
        master = json.load(f)
    entries = master['unified_entries']
    meta = master['meta']

    print(f'inserting {len(entries):,} entries …')
    erows, vrows, e_fts, g_fts = [], [], [], []
    for i, e in enumerate(entries, start=1):
        ev = e.get('hgm_evidence') or {}
        gloss = e.get('hgm_gloss')
        occ = e.get('corpus_occurrences') or {}
        variants = [v for v in (e.get('wylie_variants') or []) if v]
        erows.append((
            i, e.get('wylie') or '', e.get('tibetan'), e.get('tibetan_source'),
            e.get('acip'), e.get('pronunciation'), e.get('pronunciation_source'),
            e.get('sanskrit'), e.get('hopkins_english'),
            json.dumps(gloss, ensure_ascii=False) if gloss else None,
            ev.get('tier'), ev.get('source'), e.get('status'), e.get('tenses'),
            json.dumps(e.get('flags'), ensure_ascii=False) if e.get('flags') else None,
            json.dumps(occ.get('courses'), ensure_ascii=False) if occ.get('courses') else None,
            occ.get('n_segments'),
            json.dumps(e, ensure_ascii=False),
        ))
        vrows += [(i, v) for v in variants]
        e_fts.append((i, e.get('wylie') or '', ' '.join(variants),
                      e.get('tibetan') or '', e.get('acip') or '',
                      e.get('pronunciation') or ''))
        g_fts.append((i, ' ; '.join(gloss) if gloss else '',
                      e.get('hopkins_english') or '', e.get('sanskrit') or ''))

    con.executemany('INSERT INTO entries VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)', erows)
    con.executemany('INSERT INTO entry_variants VALUES (?,?)', vrows)
    con.executemany('INSERT INTO entries_fts(rowid,wylie,variants,tibetan,acip,pronunciation) '
                    'VALUES (?,?,?,?,?,?)', e_fts)
    con.executemany('INSERT INTO english_fts(rowid,hgm_gloss,hopkins_english,sanskrit) '
                    'VALUES (?,?,?,?)', g_fts)

    print(f'loading {CORPUS.name} …')
    with gzip.open(CORPUS, 'rt', encoding='utf-8') as f:
        corpus = json.load(f)
    print(f'inserting {len(corpus):,} corpus segments …')
    seq_by_course = {}
    crows, c_fts = [], []
    for i, s in enumerate(corpus, start=1):
        course = s.get('course') or '?'
        seq = seq_by_course.get(course, 0) + 1
        seq_by_course[course] = seq
        crows.append((i, course, seq, s.get('acip'), s.get('wylie'),
                      s.get('english'), json.dumps(s, ensure_ascii=False)))
        c_fts.append((i, s.get('wylie') or '', s.get('english') or '',
                      s.get('acip') or ''))
    con.executemany('INSERT INTO corpus_segments VALUES (?,?,?,?,?,?,?)', crows)
    con.executemany('INSERT INTO corpus_fts(rowid,wylie,english,acip) VALUES (?,?,?,?)', c_fts)

    print(f'loading {REVERSE.name} …')
    rev = json.load(open(REVERSE, encoding='utf-8'))
    rrows = [(eng, hit.get('wylie') or '', hit.get('pronunciation'), hit.get('tier'))
             for eng, hits in rev.items() for hit in hits]
    con.executemany('INSERT INTO reverse_index VALUES (?,?,?,?)', rrows)

    mrows = [
        ('release_title', meta.get('title', '')),
        ('release_version', meta.get('version', '')),
        ('release_date', meta.get('date', '')),
        ('source_master', MASTER.name),
        ('source_corpus', CORPUS.name),
        ('source_reverse_index', REVERSE.name),
        ('built_utc', time.strftime('%Y-%m-%dT%H:%M:%SZ', time.gmtime())),
        ('n_entries', str(len(entries))),
        ('n_corpus_segments', str(len(corpus))),
        ('n_reverse_keys', str(len(rev))),
        ('spine_schema_version', '1'),
    ]
    con.executemany('INSERT INTO meta VALUES (?,?)', mrows)

    con.commit()
    con.execute('PRAGMA journal_mode=DELETE')   # single-file artifact
    con.execute('VACUUM')
    con.close()
    size = DB.stat().st_size / 1e6
    print(f'done in {time.time()-t0:.1f}s → {DB} ({size:.1f} MB)')


if __name__ == '__main__':
    build()
