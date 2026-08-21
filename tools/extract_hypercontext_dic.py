#!/usr/bin/env python3
"""extract_hypercontext_dic.py — roadmap item 3: extract TibetanDictionary.dic
(Ven. Phil Baker's ACIP Hypercontext Tool, 2012-14) into JSONL + a source
dossier for Adam's licensing decision.

Binary format (from dictfiles.h / verified by probe):
  .dic : int32le nRecs, then per record:
         int16le wylieLen | ASCII wylie | int16le defLen | UTF-8 tagged def
  .ndx : nRecs * (96-byte IndexKey + int32 offset)  [not needed here]
  CAVEAT (found the hard way): the int16 length fields count UTF-16 code
  units ([NSString length]), NOT bytes — the UTF-8 payload is longer when
  definitions contain non-ASCII. The walker below consumes characters.

Definition tag language (DisplayStringFormatter.m): <c N> canned grammatical
abbreviations, <sc XYZ> per-sense source citations, <t>…</t> embedded Tibetan,
<s …> Sanskrit, <b>/<i> styling, <de>/<d c=N> DWK verb definitions, <l …>
literal meaning, <sh> shad, <p> paragraph, <lp>/<rp> parens.

NOTHING here is merged into any release — extraction + identification only
(founding rule: licensing decision precedes reference-layer use).
"""
import collections
import json
import re
import struct
import sys
from pathlib import Path

DIC = Path("/Users/adamderickandrade/ACIP Software Developmeent/"
            "Ven. Phil's Development Folder/Mac_OSX/ACIPHypercontext Tool/"
            "ACIPHypercontextTool/en.lproj/TibetanDictionary.dic")
OUT = Path(__file__).resolve().parent.parent / 'data' / 'extracted'

SOURCES = {
    'JG': 'Jampa Gendun', 'DWK': 'Dorje Wanchuk Kharto',
    'EHS': 'Erik Hein Schmidt (Rangjung Yeshe)', 'GLS': 'Geshe Lhundrup Sopa',
    'JBW': 'Joe B. Wilson', 'JH': 'Jeffrey Hopkins',
    'HAJ': 'Heinrich A. Jaeschke', 'SCD': 'Sarat Chandra Das',
    'SVB': 'Stephan V. Beyer', 'GMR': 'Geshe Michael Roach',
    'RRJ': 'Roger R. Jackson', 'JPB': 'Ven. Philip Baker',
    'ACK': 'Anne Carolyn Klein', 'DSL': 'Donald S. Lopez, Jr.',
    'MHV': 'Mahavyutpatti', 'AB': 'Alexander Berzin', 'RAFT': 'Robert Thurman',
}


def read_units(data, off, n_units):
    """Consume n_units UTF-16 code units of UTF-8 text starting at off.
    Returns (text, new_off). Non-BMP chars count as 2 units."""
    units = 0
    start = off
    while units < n_units and off < len(data):
        b = data[off]
        if b < 0x80: size = 1
        elif b < 0xC0: size = 1          # stray continuation: count forward
        elif b < 0xE0: size = 2
        elif b < 0xF0: size = 3
        else: size = 4
        units += 2 if size == 4 else 1   # non-BMP = surrogate pair = 2 units
        off += size
    return data[start:off].decode('utf-8', 'replace'), off


def main():
    data = DIC.read_bytes()
    (n_recs,) = struct.unpack_from('<i', data, 0)
    off = 4
    OUT.mkdir(parents=True, exist_ok=True)
    out_path = OUT / 'tibetan_dictionary_dic.jsonl'
    sc_tally = collections.Counter()
    entries_with_sc = 0
    n_written = 0
    samples = []
    with open(out_path, 'w', encoding='utf-8') as out:
        for i in range(n_recs):
            (wl,) = struct.unpack_from('<h', data, off)
            off += 2
            wylie, off = read_units(data, off, wl)
            (dl,) = struct.unpack_from('<h', data, off)
            off += 2
            definition, off = read_units(data, off, dl)
            if not wylie and not definition:
                continue
            scs = re.findall(r'<sc ([A-Z]+)>', definition)
            for s in scs:
                sc_tally[s] += 1
            if scs:
                entries_with_sc += 1
            rec = {'rec': i, 'wylie': wylie, 'definition': definition,
                   'sources': sorted(set(scs)),
                   'provenance': 'TibetanDictionary.dic (ACIPHypercontext, '
                                 'Ven. Phil Baker, 2014-03-11)'}
            out.write(json.dumps(rec, ensure_ascii=False) + '\n')
            n_written += 1
            if len(samples) < 3 and i > 100:
                samples.append(rec)

    print(f'records declared: {n_recs}, written: {n_written}')
    print(f'entries with <sc> citations: {entries_with_sc}')
    print('source citation tally:')
    for code, cnt in sc_tally.most_common():
        print(f'  {code:5s} {SOURCES.get(code, "(unknown code)"):40s} {cnt:8,d}')
    print(f'\nwrote {out_path} ({out_path.stat().st_size/1e6:.1f} MB)')
    for s in samples:
        print('sample:', s['wylie'], '|', s['definition'][:110])


if __name__ == '__main__':
    main()
