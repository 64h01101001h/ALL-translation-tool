"""glyph_decoder.py - HGM Dictionary project tool.
Decode TibetanMachine glyph-name runs (pypdf text extraction of ACI Tibetan
Language Study Guide PDFs) into EWTS wylie. Validation: round-trip through
pron_engine.pronounce against the card's own phonetic; fallback corroboration
against unified-entry wylie lexicon. Never guesses: unvalidated cards keep
best-lexicon-scored wylie with a flag.

Usage:
    from glyph_decoder import harvest, best_decode
    cards = harvest('C7Tibetan.pdf')            # [(glyph_run, phonetic)]
    wylie, status = best_decode(run, phon, LEX_WORDS)
"""
import re, itertools, sys
from pypdf import PdfReader
from pron_engine import pronounce

VOWELS = {'gi.gu':'i','sna.ro':'o',"'greng.bu":'e'}
SPECIAL = {'ta.log':'T','na.log':'N','da.log':'D','sha.log':'Sh','dh':'dh','bh':'bh','ny':'ny','gh':'gh'}
SUFFIXES = {'g','ng','d','n','b','m',"'",'r','l','s','N','T'}

def harvest(path):
    r = PdfReader(path); cards = []
    for pg in r.pages:
        t = pg.extract_text() or ''
        lines = [l.strip() for l in t.split('\n') if l.strip()]
        pend = []
        for l in lines:
            if '/' in l and re.search(r"/(tsheg|shad|gi\.gu|sna\.ro|zhabs\.kyu|'greng\.bu|achung)", l):
                pend.append(l[l.index('/'):])
            elif re.fullmatch(r"[a-z][a-z\-',\. ]*", l):
                if pend: cards.append((' '.join(pend), l)); pend = []
    return cards

def glyph_component(g):
    g = re.sub(r'_\d+$','',g).strip()
    if g == 'sna.ro+thig.le': return ('V','oM')
    if 'thig.le' in g: return ('C','M')
    if 'zhabs.kyu' in g: return ('V','u')
    for k,v in VOWELS.items():
        if g==k or g.endswith('.'+k) or (('.'+k+'.') in g): return ('V',v)
    if g == 'achung': return ('C',"'")
    if g in ('a.chen','achen'): return ('C','@')
    if g in SPECIAL: return ('C',SPECIAL[g])
    g2 = g[6:] if g.startswith('short_') else g
    g2 = re.sub(r"^tsa\.'bru\.", '', g2)
    if re.fullmatch(r"[bcdghjklmnprstwyz'\.]*a", g2):
        return ('C', g2[:-1].replace('.',''))
    return ('?', g)

def _join(cons):
    out = ''
    for c in cons:
        if out.endswith('g') and c == 'y': out += '.'
        out += c
    return out

def _insert_a(cons):
    from pron_engine import CLUSTERS, SIMPLE
    VALID = set(CLUSTERS) | set(SIMPLE)
    def onset_ok(o):
        if o in VALID: return True
        return any(o.startswith(p) and o[len(p):] in VALID for p in ("b","d","g","m","'"))
    cands = []
    for i in range(1, len(cons)+1):
        pre, post = cons[:i], cons[i:]
        if len(post) > 2: continue
        if post and post[0] not in SUFFIXES: continue
        if len(post) == 2 and not (post[1]=='s' and post[0] in ('g','ng','b','m')): continue
        o = _join(pre)
        if not onset_ok(o): continue
        cands.append(o+'a'+''.join(post))
    if not cands and cons: cands.append(_join(cons)+'a')
    return cands

def decode_syllable(glyphs):
    comps = [glyph_component(g) for g in glyphs]
    if any(t=='?' for t,_ in comps): return []
    if comps and comps[0]==('C','@'):
        rest = comps[1:]
        if rest and rest[0][0]=='V':
            return [rest[0][1] + ''.join(c for t,c in rest[1:] if t!='V')]
        return ['a'+''.join(c for t,c in rest)]
    for j in range(1, len(comps)):
        if comps[j]==('C',"'"):
            left = comps[:j]
            lv = [i for i,(t,_) in enumerate(left) if t=='V']
            if lv:
                i = lv[0]
                lstr = [_join([c for t,c in left[:i]]) + left[i][1] + ''.join(c for t,c in left[i+1:] if t!='V')]
            else:
                lstr = _insert_a([c for t,c in left])
            after = comps[j+1:]
            if after and after[0][0]=='V':
                rv = after[0][1]; rsuf = ''.join(c for t,c in after[1:] if t!='V')
                return [l + "'" + rv + rsuf for l in lstr]
            elif after:                        # mi'am: achung carries inherent a
                return [l + "'a" + ''.join(c for t,c in after) for l in lstr]
            else:                              # final achung: mtha'
                return [l + "'" for l in lstr]
    vpos = [i for i,(t,_) in enumerate(comps) if t=='V']
    cons = [c for t,c in comps]
    if vpos:
        i = vpos[0]
        return [_join(cons[:i]) + comps[i][1] + ''.join(c for t,c in comps[i+1:] if t!='V')]
    return _insert_a(cons)

def decode_run(run):
    toks = [t.strip() for t in run.strip('/').split('/') if t.strip()]
    syls, cur = [], []
    for t in toks:
        base = re.sub(r'_\d+$','',t)
        if base=='tsheg' or base.startswith('shad') or base.startswith('tsheg'):
            if cur: syls.append(cur); cur=[]
        else: cur.append(t)
    if cur: syls.append(cur)
    return [decode_syllable(s) for s in syls]

def _flat(p): return re.sub(r"[\s\-,\.]", '', p.lower())

def best_decode(run, phon, lex_words):
    per = decode_run(run)
    if any(not c for c in per): return None, 'undecodable-glyph'
    total = 1
    for c in per: total *= len(c)
    if total > 8192: per = [c[:3] for c in per]
    target = _flat(phon)
    best_lex, best_score = None, -1
    for combo in itertools.product(*per):
        w = ' '.join(combo)
        for wv in (w, w.replace('g.y','g-y')):
            try:
                if _flat(pronounce(wv)) == target: return w, 'validated'
            except Exception: pass
        toks = w.split()
        s = sum(1 for t in toks if t in lex_words or t.replace('.','') in lex_words)/max(1,len(toks))
        if s > best_score: best_lex, best_score = w, s
    try:
        if best_lex and target and _flat(pronounce(best_lex)).startswith(target):
            return best_lex, 'validated-prefix'
    except Exception: pass
    if best_score >= 0.99: return best_lex, 'lexicon-validated'
    return best_lex, 'unvalidated'
