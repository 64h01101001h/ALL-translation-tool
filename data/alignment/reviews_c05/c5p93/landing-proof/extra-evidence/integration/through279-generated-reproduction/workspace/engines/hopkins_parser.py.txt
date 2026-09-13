"""hopkins_parser.py — style-aware re-parse of the Hopkins/Uma dictionary PDF (v0.26).

Entry anchor: 16pt XenoTypeTibetanNew headword. Styles: I(talic)=wylie/sanskrit,
B(old)=English gloss, R(oman)=prose/tagged glosses, T=inline Tibetan, S=separator.
Field rules (validated against print):
  wylie   = first italic segment(s) before the first ▪
  tenses  = italic run of 3-5 bare tokens directly after inline-Tibetan tense block
  sanskrit= italic segments (with {TAG} prefixes) between wylie and first bold/▫-block
  hopkins = bold segments + {TAG}-prefixed roman segments (English), ';'-joined;
            prose blocks (Definitions/Divisions/examples/quotes) excluded
"""
import pdfplumber, re, json
from collections import defaultdict

SEP = {'▫', '▪'}

def style(c):
    f = c['fontname']
    if 'Tibetan' in f: return 'H' if c['size'] >= 14 else 'T'
    if c['text'] in SEP: return 'S'
    if '-SC' in f: return 'D'          # smallcaps labels (Definitions, Divisions…)
    if 'Italic' in f: return 'I'
    if 'Bold' in f: return 'B'
    return 'R'

def page_lines(pg):
    """Two-column line assembly; returns lines in reading order as lists of chars.
    Clusters by baseline (bottom) so 16pt Tibetan and 10pt Latin on one visual
    line stay together."""
    mid = pg.width / 2
    lines = []
    for ci in (0, 1):
        chars = [c for c in pg.chars
                 if 60 <= c['top'] <= 725 and (c['x0'] < mid) == (ci == 0)]
        # true baseline from the text matrix: same visual line = same baseline
        for c in chars:
            c['_bl'] = round(c['matrix'][5], 1)
        chars.sort(key=lambda c: (-c['_bl'], c['x0']))
        cur, last_bl = [], None
        for c in chars:
            if last_bl is not None and abs(c['_bl'] - last_bl) > 2.5:
                if cur: lines.append(sorted(cur, key=lambda x: x['x0']))
                cur = []
            cur.append(c)
            last_bl = c['_bl']
        if cur: lines.append(sorted(cur, key=lambda x: x['x0']))
    return lines

def parse_pages(p0, p1):
    pdf = pdfplumber.open('hopkins.pdf')
    entries = []
    cur = None
    for pi in range(p0, p1):
        for line in page_lines(pdf.pages[pi]):
            nonsp = [c for c in line if not c['text'].isspace()]
            has_head = (bool(nonsp) and style(nonsp[0]) == 'H'
                        and (nonsp[0]['x0'] <= 76 or 306 <= nonsp[0]['x0'] <= 316))
            if has_head:
                if cur: entries.append(cur)
                cur = {'page': pi + 1, 'runs': []}
            if cur is None: continue
            prev_st = None
            for c in line:
                st = prev_st if (c['text'].isspace() and prev_st) else style(c)
                runs = cur['runs']
                if runs and runs[-1][0] == st:
                    runs[-1][1] += c['text']
                else:
                    runs.append([st, c['text']])
                prev_st = st
            cur['runs'].append(['NL', ''])
        pdf.flush_cache()
    if cur: entries.append(cur)
    return entries

TAG = re.compile(r'\{[A-Za-z0-9,: ]{1,14}\}')
def clean(s):
    s = s.replace('\u2018', "'").replace('\u2019', "'")
    s = re.sub(r'\s+', ' ', s).strip(' ;,')
    s = re.sub(r'(\w)- (\w)', r'\1-\2', s)      # rejoin hyphen line-breaks
    return s.strip()

PROSE = re.compile(r'^\s*(Def\.?:|Div\.?:|Definitions?|Divisions?|Comment:|Examples?[:;]|e\.g\.|\{[A-Z]{1,4}\} ?p?p?\.? ?\d)', re.I)

def extract_fields(entry):
    """Walk runs → wylie, tenses, sanskrit, hopkins."""
    # collapse NL: newline inside a segment = space
    segs = []   # (style, text) segments split at separators
    buf_style, buf = None, ''
    for st, tx in entry['runs']:
        if st == 'NL':
            buf += ' '
            continue
        if st == 'S':
            if buf_style: segs.append((buf_style, buf))
            segs.append(('SEP', tx.strip()))
            buf_style, buf = None, ''
        elif st == buf_style:
            buf += tx
        else:
            if buf_style: segs.append((buf_style, buf))
            buf_style, buf = st, tx
    if buf_style: segs.append((buf_style, buf))

    wylie = None; tenses = None; skt = []; eng = []
    pending_tag = ''
    stop = False           # once a prose block starts, only a new ▪ gloss resumes
    seen_bold = False
    for i, (st, tx) in enumerate(segs):
        t = clean(tx)
        if not t and st != 'SEP': continue
        if st == 'H' or st == 'T':
            continue
        if st == 'D':
            stop = True; pending_tag = ''
            continue
        if st == 'SEP':
            if tx == '▪': stop = False
            continue
        if st == 'I':
            if wylie is None:
                wylie = t; continue
            if stop: continue
            toks = re.split(r'[ /]+', t)
            if tenses is None and not skt and not seen_bold and 3 <= len(toks) <= 5 \
               and all(re.fullmatch(r"[a-z'’‘\.\-]+", x) for x in toks) \
               and any(s == 'T' for s, _ in segs[:i]):
                tenses = t.replace('’', "'"); continue
            if not seen_bold:
                skt.append((pending_tag + t).strip()); pending_tag = ''
            continue
        if st == 'B':
            seen_bold = True
            if not stop:
                eng.append((pending_tag + t).strip()); pending_tag = ''
            continue
        if st == 'R':
            if PROSE.match(t):
                stop = True; pending_tag = ''; continue
            m = re.match(r'^(\{[A-Za-z0-9,:\. ]{1,14}\})\s*(.*)$', t)
            if m and not stop:
                body = m.group(2).strip()
                if body:
                    eng.append(m.group(1) + body)
                else:
                    pending_tag = m.group(1)
                continue
            # bare roman text: prose → stop
            if len(t) > 2: stop = True
            continue
    return {'wylie': clean(wylie or ''), 'tenses': tenses,
            'sanskrit': '; '.join(x for x in skt if x) or None,
            'hopkins': '; '.join(x for x in eng if x) or None,
            'page': entry['page']}

if __name__ == '__main__':
    import sys
    p0, p1 = int(sys.argv[1]), int(sys.argv[2])
    es = parse_pages(p0, p1)
    out = [extract_fields(e) for e in es]
    json.dump(out, open(f'hopkins_parsed_{p0}_{p1}.json', 'w'), ensure_ascii=False, indent=1)
    print(f'pages {p0}-{p1}: {len(out)} entries')
