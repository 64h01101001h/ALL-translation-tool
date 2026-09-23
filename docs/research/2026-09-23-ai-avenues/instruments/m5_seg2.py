import sqlite3, json, collections, re, pickle
REPO='/Users/adamderickandrade/ALL-translation-tool/'
db=sqlite3.connect(REPO+'build/hgm_spine_v27_2.db')
lex=pickle.load(open('/private/tmp/claude-501/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2/scratchpad/lex.pkl','rb'))['lex']
def syls(w):
    out=[]
    for tok in re.split(r"\s+", w or ''):
        tok=tok.strip(",./;:()[]!?\"*-_0123456789")
        if tok: out.append(tok)
    return out
def munch(ss, L, maxl=4):
    i=0; out=[]
    while i<len(ss):
        for l in range(min(maxl, len(ss)-i), 0, -1):
            cand=tuple(ss[i:i+l])
            if l==1 or cand in L:
                out.append(' '.join(cand)); i+=l; break
    return out
bank=json.load(open(REPO+'data/alignment/alignment_full_v1.json'))
L=[x for x in bank['links'] if x['seg'] is not None]
byseg=collections.defaultdict(list)
for x in L:
    if x['d'] in (5,6): byseg[(x['course'],x['seg'])].append(x)
rows={ (c,s):(w,e) for c,s,w,e in db.execute("select course,seq,wylie,english from corpus_segments") }
# corpus-attested lexicon: ngrams that actually occur in the whole spine
allsyl={}
ngc=collections.Counter()
for (c,s),(w,e) in rows.items():
    ss=syls(w)
    for l in range(2,5):
        for i in range(len(ss)-l+1): ngc[tuple(ss[i:i+l])]+=1
attested={t for t in lex if len(t)==1 or ngc.get(t,0)>0}
print('attested-lexicon size:', len(attested))
for name,LL in (('full',lex),('attested',attested)):
    for maxl in (2,3,4):
        tg=th=tp=0
        for k,v in byseg.items():
            if k not in rows: continue
            gold=[' '.join(syls(x['tib'])) for x in v if x['tib']]; gold=[g for g in gold if g]
            pred=munch(syls(rows[k][0]), LL, maxl)
            gc=collections.Counter(gold); pc=collections.Counter(pred)
            th+=sum(min(gc[t],pc[t]) for t in gc); tg+=len(gold); tp+=len(pred)
        print(f"  {name} maxl={maxl}: recall={100*th/tg:.1f}% precision={100*th/tp:.1f}% F1={200*th/(tg+tp):.1f}")
pickle.dump({'attested':attested}, open('/private/tmp/claude-501/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2/scratchpad/lex2.pkl','wb'))
