import sqlite3, json, collections, re, pickle
REPO='/Users/adamderickandrade/ALL-translation-tool/'
db=sqlite3.connect(REPO+'build/hgm_spine_v27_2.db')

# --- lexicon from the master dictionary (Tibetan side only; no gloss used) ---
lex=set(); lex_by_len=collections.defaultdict(set)
n=0
for w, in db.execute("select wylie from entries"):
    if not w: continue
    w=w.strip()
    w=re.sub(r'^!+','',w).strip()
    if not w: continue
    parts=w.split()
    if len(parts)>6: continue
    if any(re.search(r'[^a-zA-Z\'\+\-\.~]', p) for p in parts): continue
    lex.add(tuple(parts)); lex_by_len[len(parts)].add(tuple(parts)); n+=1
print('lexicon entries usable:', len(lex), 'from', n)
MAXL=max(lex_by_len)
print('max len', MAXL, {k:len(v) for k,v in sorted(lex_by_len.items())})

def syls(w):
    out=[]
    for tok in re.split(r"\s+", w or ''):
        tok=tok.strip(",./;:()[]!?\"*-_0123456789")
        if tok: out.append(tok)
    return out

def munch(ss, maxl=4):
    i=0; out=[]
    while i<len(ss):
        for l in range(min(maxl, len(ss)-i), 0, -1):
            cand=tuple(ss[i:i+l])
            if l==1 or cand in lex:
                out.append(' '.join(cand)); i+=l; break
    return out

# --- validate segmenter against bank d=5 spans on landed segments ---
bank=json.load(open(REPO+'data/alignment/alignment_full_v1.json'))
L=[x for x in bank['links'] if x['seg'] is not None]
byseg=collections.defaultdict(list)
for x in L:
    if x['d'] in (5,6): byseg[(x['course'],x['seg'])].append(x)
rows={ (c,s):(w,e) for c,s,w,e in db.execute("select course,seq,wylie,english from corpus_segments") }
tot_gold=0; tot_hit=0; tot_pred=0
for k,v in list(byseg.items()):
    if k not in rows: continue
    gold=[' '.join(syls(x['tib'])) for x in v if x['tib']]
    gold=[g for g in gold if g]
    pred=munch(syls(rows[k][0]))
    gc=collections.Counter(gold); pc=collections.Counter(pred)
    hit=sum(min(gc[t],pc[t]) for t in gc)
    tot_gold+=len(gold); tot_pred+=len(pred); tot_hit+=hit
print(f"segmenter vs bank d=5/6 spans (multiset overlap): gold={tot_gold} pred={tot_pred} hit={tot_hit} recall={100*tot_hit/tot_gold:.1f}% precision={100*tot_hit/tot_pred:.1f}%")

pickle.dump({'lex':lex}, open('/private/tmp/claude-501/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2/scratchpad/lex.pkl','wb'))
