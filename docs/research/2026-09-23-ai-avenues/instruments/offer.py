import json,sqlite3,collections,re
R='/Users/adamderickandrade/ALL-translation-tool/'
d=json.load(open(R+'data/alignment/alignment_full_v1.json'))
L=[l for l in d['links'] if l.get('seg') is not None]
db=sqlite3.connect(R+'build/hgm_spine_v27_2.db'); c=db.cursor()
gloss=collections.defaultdict(set); heads=set()
c.execute("select wylie,hgm_gloss from entries")
for w,g in c.fetchall():
    if w: heads.add(w)
    if g:
        try:
            for s in json.loads(g): gloss[w].add(s)
        except Exception: pass
bankheads=set(l['tib'] for l in L if l['d']==5)
lex=heads|bankheads
att=collections.defaultdict(collections.Counter)
for l in L:
    if l['d']==5 and l.get('eng'): att[l['tib']][l['eng']]+=1
def spans(w):
    S=[(m.start(),m.end()) for m in re.finditer(r"[^\s]+",w)]
    res=set()
    for i in range(len(S)):
        for j in range(i+1,min(i+8,len(S))+1):
            t=w[S[i][0]:S[j-1][1]].strip().strip(',').strip()
            if t in lex: res.add(t)
    return res
landed=set((l['course'],l['seg']) for l in L)
courses=['C%02d'%i for i in range(1,19)]
c.execute("select course,seq,wylie,english from corpus_segments where course in (%s)"%','.join('?'*18),courses)
rows=c.fetchall()
print('course  segs  lattice-spans/seg  OFFER-RATE(span has >=1 verbatim-present candidate)')
agg={}
for co in courses:
    rs=[r for r in rows if r[0]==co]
    tot=0;off=0;ns=0
    for _,sq,w,e in rs:
        e=e or ''
        sp=spans(w)
        if not sp: continue
        ns+=1
        for t in sp:
            tot+=1
            cands={x for x in (set(att.get(t,{}))|gloss.get(t,set())) if x and x in e}
            if cands: off+=1
    agg[co]=(len(rs),tot/max(ns,1),100*off/max(tot,1))
    print('  %s  %5d   %5.1f   %5.1f%%'%(co,len(rs),tot/max(ns,1),100*off/max(tot,1)))
