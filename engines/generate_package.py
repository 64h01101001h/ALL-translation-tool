#!/usr/bin/env python3
"""Canonical HGM Dictionary package generator — round-trip validated against v0.20 (see README v0.21).
Usage: python3 generate_package.py MASTER.json OUTDIR VERSION "HEADLINE" [PART1_SOURCE.md]
Produces: json, tsv, reverse index, full md (P1 carried verbatim from PART1_SOURCE), compact, attested.
Format rules recovered by round-trip validation — DO NOT reimplement from memory; edit here only:
- TSV: csv.writer delimiter='\t' lineterminator='\r\n' QUOTE_MINIMAL; pron_source renders as 'card';
  joined gloss .replace('"','',2) if starts with '"'; sanskrit[:200]; hopkins uncapped.
- Reverse index: keys per-gloss ';'-parts, .strip().lower(), 2<=len<=60, must contain [a-z]; entry order.
- Full md: P2 = first 25,899 entries; P3 = rest sorted by wylie; block fmt in entry_line_full;
  skt[:110], hopkins[:150]; contexts joined ','; candidate rendered ⟨candidate: …⟩.
- Compact/attested: head only if acip/pron; gloss first 4 items + tag; hopkins {tag}-stripped,
  bracket-filtered, first 3 items, rstrip('.'); skt {tag}-stripped, ';'->' ·', ci-dedup, first 2 items,
  skipped if starts '['.
"""
import json, re, io, csv, sys
from collections import OrderedDict

def tier_str(e):
    ev = e.get('hgm_evidence')
    if not isinstance(ev, dict): return ''
    t = ev.get('tier','')
    return 'auto-aligned:' + ev.get('method','') if t=='auto-aligned' else t

def gen_tsv(m):
    buf = io.StringIO(); w = csv.writer(buf, delimiter='\t', lineterminator='\r\n')
    w.writerow(['tibetan','wylie','acip','pronunciation','pron_source','status','hgm_glosses','hgm_evidence','corpus_courses','sanskrit','hopkins_english'])
    for e in m['unified_entries']:
        gl = '; '.join(str(x) for x in (e.get('hgm_gloss') or []))
        if gl.startswith('"'): gl = gl.replace('"','',2)
        co = e.get('corpus_occurrences')
        crs = ','.join(co.get('courses', [])) if isinstance(co, dict) else ''
        w.writerow([e.get('tibetan') or '', e.get('wylie') or '', e.get('acip') or '',
                    e.get('pronunciation') or '', 'card' if e.get('pronunciation_source') else '',
                    e.get('status') or '', gl, tier_str(e), crs,
                    (e.get('sanskrit') or '')[:200], e.get('hopkins_english') or ''])
    return buf.getvalue()

def gen_ri(m):
    r = OrderedDict()
    for e in m['unified_entries']:
        gl = e.get('hgm_gloss')
        if not gl: continue
        ev = e.get('hgm_evidence') or {}
        tier = ev.get('tier','') if isinstance(ev, dict) else ''
        for item in gl:
            for part in str(item).split(';'):
                k = part.strip().lower()
                if not (2 <= len(k) <= 60) or not re.search(r'[a-z]', k): continue
                r.setdefault(k, []).append({'wylie': e.get('wylie',''), 'pronunciation': e.get('pronunciation',''), 'tier': tier})
    return r

def entry_line_full(e):
    parts = [f"** {e.get('tibetan') or ''} ** ▫ {e.get('wylie') or ''} ▫ {e.get('acip') or ''} ▫ *{e.get('pronunciation') or ''}*"]
    if e.get('pronunciation_source'): parts.append(' ⟪card⟫')
    parts.append(' — ')
    gl = e.get('hgm_gloss')
    if gl:
        parts.append('**HGM:** ' + '; '.join(str(x) for x in gl))
        ts = tier_str(e)
        if ts: parts.append(' 〔' + ts.replace('auto-aligned:', 'auto-aligned·') + '〕')
    else:
        parts.append('⟨awaiting HGM equivalent⟩')
        if e.get('auto_candidate'): parts.append(' ⟨candidate: ' + str(e['auto_candidate']) + '⟩')
        co = e.get('corpus_occurrences')
        if isinstance(co, dict) and co.get('courses'):
            parts.append(' ⟨contexts: ' + ','.join(co['courses']) + '⟩')
    tail = []
    if e.get('sanskrit'): tail.append('*Skt.* ' + e['sanskrit'][:110])
    if e.get('hopkins_english'): tail.append('*Hopkins:* ' + e['hopkins_english'][:150])
    if tail: parts.append(' | ' + ' | '.join(tail))
    return ''.join(parts)

def compact_block(e, attested_only=False):
    gl = e.get('hgm_gloss'); ev = e.get('hgm_evidence')
    if attested_only and not gl: return None
    acip, pron = e.get('acip') or '', e.get('pronunciation') or ''
    head = f'{acip}  ("{pron}")' if (acip or pron) else ''
    lines = [head]
    def skt2(s):
        c = re.sub(r'\{[^}]*\}\s*','',s).replace(';',' ·')
        items, seen = [], set()
        for x in (y.strip() for y in c.split(' ·')):
            if x and x.lower() not in seen: items.append(x); seen.add(x.lower())
        return ' · '.join(items[:2])
    def addskt():
        s = skt2(e['sanskrit']) if e.get('sanskrit') else ''
        if s and not s.startswith('['): lines.append('    Skt. ' + s)
    if gl:
        tag = {'curated':'[HGM·curated]','glossary':'[HGM]'}.get((ev or {}).get('tier') if isinstance(ev,dict) else '', '[HGM·auto]')
        lines.append('    ' + '; '.join(str(x) for x in gl[:4]) + '   ' + tag); addskt()
    else:
        hop = ''
        if e.get('hopkins_english'):
            h = re.sub(r'\{[^}]*\}\s*','', e['hopkins_english'])
            items = [x.strip() for x in h.split(';') if x.strip() and not x.strip().startswith('[')]
            hop = '; '.join(items[:3]).rstrip('.')
        if hop: lines.append('    ' + hop + '   [Hopkins]'); addskt()
        else: lines.append('    —'); addskt()
    return '\n'.join(lines)

def main(master_path, outdir, version, headline, part1_src):
    M = json.load(open(master_path))
    st = M['meta']['stats']
    src = open(part1_src, encoding='utf-8').read()
    i2 = src.find('# Part II')
    part1 = src[src.find('# Part I — Curated Entries'):i2]
    n_gl = sum(1 for e in M['unified_entries'] if e.get('hgm_gloss'))
    json.dump(M, open(f'{outdir}/hgm_dictionary.json','w', encoding='utf-8'), ensure_ascii=False)
    open(f'{outdir}/hgm_dictionary.tsv','w', encoding='utf-8', newline='').write(gen_tsv(M))
    json.dump(gen_ri(M), open(f'{outdir}/hgm_reverse_index.json','w', encoding='utf-8'), ensure_ascii=False)
    hdr = (f"# The Holy Geshe Michael Roach Dictionary\n## v{version} — {headline}\n\n"
           f"*25,899 entries; {n_gl:,} with HGM English; {st['corpus_segments']:,}-segment corpus; "
           f"**{st['card_attested']:,} card-attested pronunciations** ⟪card⟫; {len(M['unified_entries']):,} entries incl. 423 card-derived + 11 scaffold additions.*\n\n---\n\n")
    p2 = '\n\n---\n\n'.join(entry_line_full(e) for e in M['unified_entries'][:25899])
    tail = sorted(M['unified_entries'][25899:], key=lambda e: e.get('wylie',''))
    p3h = ("# Part III — Card-Derived & Scaffold Additions (v0.17/v0.26.1)\n\n*Entries recovered by glyph-decoding the Language Study Guide PDFs, "
           "plus 11 Uma-scaffold headwords recovered v0.26.1; English awaiting HGM equivalent. Interleave into Part II at next full regeneration.*\n\n")
    open(f'{outdir}/hgm_dictionary.md','w', encoding='utf-8').write(
        hdr + part1 + '# Part II — Unified Dictionary\n\n' + p2 + '\n\n---\n\n' + p3h +
        '\n\n---\n\n'.join(entry_line_full(e) for e in tail) + '\n')
    ch = (f"THE HOLY GESHE MICHAEL ROACH DICTIONARY\nCompact Edition — v{version}\n\n"
          "Tags: [HGM·curated] cited · [HGM] glossary-attested · [HGM·auto] machine-aligned, provisional · [Hopkins] reference only\n\n" + '='*60 + '\n')
    open(f'{outdir}/hgm_dictionary_compact.md','w', encoding='utf-8').write(
        ch + '\n' + '\n\n'.join(compact_block(e) for e in M['unified_entries']) + '\n')
    ah = (f"THE HOLY GESHE MICHAEL ROACH DICTIONARY\nAttested Edition — v{version}\n\nEvery entry carries HGM English.\n\n" + '='*60 + '\n')
    ab = [b for b in (compact_block(e, True) for e in M['unified_entries']) if b is not None]
    open(f'{outdir}/hgm_dictionary_attested.md','w', encoding='utf-8').write(ah + '\n' + '\n\n'.join(ab) + '\n')
    print(f'generated v{version}: glossed {n_gl}, attested blocks {len(ab)}')

if __name__ == '__main__':
    main(*sys.argv[1:6])
