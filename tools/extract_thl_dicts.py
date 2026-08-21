#!/usr/bin/env python3
"""extract_thl_dicts.py — roadmap item 4: extract the THL Translation Tool
bundle (thl.wrd/thl.def/thl.dic, 2009) into JSONL with per-dictionary tags.

Format (reverse-engineered from DictionarySearchStandalone.jar bytecode,
org.thdl.tib.scanner.FileSyllableListTree / ByteDictionarySource, version 3):
  thl.wrd  big-endian syllable trie. Node:
             int32 childPos (-1 = leaf) · uint16 len + syllable (modified UTF)
             flag byte: 0x40 = has next sibling, low 6 bits = ndefs
             per def: dict-byte sequence (low 6 bits = dict number,
                      0x40 = another byte follows for this def)
             then ndefs × int32 offsets into thl.def
           file tail: int32 rootListPos · int32 0xFFFFFF03 (version marker)
  thl.def  definition records at those offsets, each a Java writeUTF string:
           uint16 byte-length + modified-UTF-8 bytes
  thl.dic  CSV: display label, tag — one line per dict number (in order)

LOCAL REFERENCE ONLY (Adam's decision 2026-08-06): per-dictionary licensing
varies (Hopkins fields, Rangjung Yeshe, Valby, Ives Waldo, Dan Martin…);
never merged into HGM releases, never redistributed.
"""
import json
import struct
import sys
from pathlib import Path

ROOT_DIR = Path("/Users/adamderickandrade/Tibetan Translation Tool/thl-dicts")
OUT = Path(__file__).resolve().parent.parent / 'data' / 'extracted'


def main():
    W = (ROOT_DIR / 'thl.wrd').read_bytes()
    D = (ROOT_DIR / 'thl.def').read_bytes()
    dic_lines = (ROOT_DIR / 'thl.dic').read_text().strip().splitlines()
    dict_names = []
    for line in dic_lines:
        parts = [p.strip() for p in line.split(',')]
        dict_names.append(parts[1] if len(parts) > 1 and parts[1] else parts[0])
    print('dictionaries:', dict_names)

    (version_word,) = struct.unpack('>i', W[-4:])
    assert (version_word >> 8) == -1 and (version_word & 0xFF) == 3, 'not a v3 file'
    (root,) = struct.unpack('>i', W[-8:-4])

    def read_node(off):
        (child,) = struct.unpack_from('>i', W, off); off += 4
        (sl,) = struct.unpack_from('>H', W, off); off += 2
        syl = W[off:off + sl].decode('utf-8', 'replace'); off += sl
        b = W[off]; off += 1
        has_brother = bool(b & 0x40)
        ndefs = b & 0x3F
        dictsets = []
        for _ in range(ndefs):
            ds = []
            while True:
                x = W[off]; off += 1
                ds.append(x & 0x3F)
                if not (x & 0x40):
                    break
            dictsets.append(ds)
        offs = []
        for _ in range(ndefs):
            (o,) = struct.unpack_from('>i', W, off); off += 4
            offs.append(o)
        return child, syl, has_brother, list(zip(dictsets, offs)), off

    def def_text(o):
        (length,) = struct.unpack_from('>H', D, o)
        return D[o + 2:o + 2 + length].decode('utf-8', 'replace').strip()

    OUT.mkdir(parents=True, exist_ok=True)
    out_path = OUT / 'thl_dicts.jsonl'
    n_words = 0
    n_defs = 0
    per_dict = {}
    with open(out_path, 'w', encoding='utf-8') as out:
        # iterative DFS over (listPos, prefix)
        stack = [(root, [])]
        while stack:
            pos, prefix = stack.pop()
            off = pos
            while True:
                child, syl, hb, defs, off = read_node(off)
                word = prefix + [syl]
                if defs:
                    rec = {
                        'wylie': ' '.join(word),
                        'defs': [{
                            'dicts': [dict_names[d] if d < len(dict_names)
                                      else f'#{d}' for d in ds],
                            'text': def_text(o),
                        } for ds, o in defs],
                        'provenance': 'thl.wrd/thl.def (THL Translation Tool '
                                      'bundle, 2009-08-28)',
                    }
                    out.write(json.dumps(rec, ensure_ascii=False) + '\n')
                    n_words += 1
                    n_defs += len(defs)
                    for ds, _ in defs:
                        for dnum in ds:
                            name = (dict_names[dnum] if dnum < len(dict_names)
                                    else f'#{dnum}')
                            per_dict[name] = per_dict.get(name, 0) + 1
                if child >= 0:
                    stack.append((child, word))
                if not hb:
                    break

    print(f'words with definitions: {n_words:,}   total defs: {n_defs:,}')
    print('defs per dictionary tag:')
    for name, cnt in sorted(per_dict.items(), key=lambda kv: -kv[1]):
        print(f'  {name:12s} {cnt:8,d}')
    print(f'wrote {out_path} ({out_path.stat().st_size/1e6:.1f} MB)')


if __name__ == '__main__':
    main()
