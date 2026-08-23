#!/usr/bin/env python3
"""build_reference_db.py — compile the extracted reference dictionaries
(TibetanDictionary.dic + THL bundle) into build/reference.db for the Lookup
pane's stacked multi-dictionary display.

LOCAL REFERENCE ONLY (Adam's decision 2026-08-06): these layers are
unlicensed compilations for personal in-house lookup. Every row keeps its
layer tag; the UI is required to label them "reference only".

WHERE IT MAY GO (corrected 2026-08-23, SQA BUILD-5). This header used to
read "The DB never ships" — and it was shipping: 80,756,736 bytes of it,
in every team DMG since the press gained a mode, while
OPEN_SOURCE_NOTICES.md said the THL payload was "NOT redistributed". The
truth, and the rule from here:

  * never merges into HGM releases                     (unchanged)
  * TEAM DMG only. docs/distribution/PAYLOAD_MANIFEST.txt carries the
    row `team ... build/reference.db`; a market press that finds it
    FAILS, and the team DMG's README.txt says in as many words that the
    image is not for redistribution and names this file.
  * the source extractions (data/extracted/thl_dicts.jsonl,
    tibetan_dictionary_dic.jsonl) never leave this machine — they are
    `drop` rows and the press removes them from the stage.
"""
import json
import sqlite3
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
TD = ROOT / 'data' / 'extracted' / 'tibetan_dictionary_dic.jsonl'
THL = ROOT / 'data' / 'extracted' / 'thl_dicts.jsonl'
DB = ROOT / 'build' / 'reference.db'

SCHEMA = """
PRAGMA journal_mode=WAL;
CREATE TABLE meta (key TEXT PRIMARY KEY, value TEXT);
CREATE TABLE ref_entries (
  id INTEGER PRIMARY KEY,
  layer TEXT NOT NULL,        -- 'TD' | THL dict tag(s), '+'-joined
  wylie TEXT NOT NULL,
  definition TEXT NOT NULL
);
CREATE INDEX idx_ref_wylie ON ref_entries(wylie);
CREATE VIRTUAL TABLE ref_fts USING fts5(
  wylie, definition, content='', tokenize="unicode61 tokenchars ''''");
"""


def main():
    if DB.exists():
        DB.unlink()
    con = sqlite3.connect(DB)
    con.executescript(SCHEMA)
    t0 = time.time()
    rid = 0
    rows, fts = [], []

    def add(layer, wylie, definition):
        nonlocal rid
        if not wylie or not definition:
            return
        rid += 1
        rows.append((rid, layer, wylie, definition))
        fts.append((rid, wylie, definition))

    n_td = 0
    for line in open(TD, encoding='utf-8'):
        r = json.loads(line)
        add('TD', r['wylie'], r['definition'])
        n_td += 1

    n_thl = 0
    for line in open(THL, encoding='utf-8'):
        r = json.loads(line)
        for d in r['defs']:
            add('+'.join(d['dicts']), r['wylie'], d['text'])
        n_thl += 1

    con.executemany('INSERT INTO ref_entries VALUES (?,?,?,?)', rows)
    con.executemany('INSERT INTO ref_fts(rowid, wylie, definition) VALUES (?,?,?)',
                    fts)
    con.executemany('INSERT INTO meta VALUES (?,?)', [
        ('built_utc', time.strftime('%Y-%m-%dT%H:%M:%SZ', time.gmtime())),
        ('n_rows', str(rid)),
        ('sources', 'TD (Hypercontext .dic) + THL bundle'),
        ('licensing', 'LOCAL REFERENCE ONLY — never redistribute'),
    ])
    con.commit()
    con.execute('PRAGMA journal_mode=DELETE')
    con.execute('VACUUM')
    con.close()
    print(f'{rid:,} rows ({n_td:,} TD entries, {n_thl:,} THL words) '
          f'in {time.time()-t0:.1f}s → {DB} ({DB.stat().st_size/1e6:.1f} MB)')


if __name__ == '__main__':
    main()
