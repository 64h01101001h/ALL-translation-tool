import json, sqlite3, collections, math, re, random
REPO='/Users/adamderickandrade/ALL-translation-tool'
d=json.load(open(REPO+'/data/alignment/alignment_full_v1.json'))
seg_heads=collections.defaultdict(set)
for l in d['links']:
    t,e=l.get('tib'),l.get('eng')
    if t and e and l.get('seg') is not None: seg_heads[(l['course'],l['seg'])].add(t)
con=sqlite3.connect(REPO+'/build/hgm_spine_v27_2.db')
rows={(c,s):(w or '',e or '') for c,s,w,e in con.execute("select course,seq,wylie,english from corpus_segments")}
hgm=set(); allw=set()
for w,g in con.execute("select wylie,hgm_gloss from entries"):
    if w:
        allw.add(w)
        if g: hgm.add(w)
print('dictionary headwords {:,}; with hgm_gloss {:,}'.format(len(allw),len(hgm)))
SPLIT=re.compile(r"[\s/|,;:()\[\]!?.*@#$%^&_+=\"'<>{}-]+")
syls=lambda w:[x for x in SPLIT.split(w) if x]
bank=set(); prior_syl=set()
for k in seg_heads:
    if k[0]!='C05':
        bank|=seg_heads[k]
        if k in rows: prior_syl|=set(syls(rows[k][0]))
test=sorted([k for k in seg_heads if k[0]=='C05'], key=lambda k:k[1])
allnew=set()
for k in test: allnew|=(seg_heads[k]-bank)
N=len(allnew)
raw=[len(rows[k][0])+len(rows[k][1]) for k in test]; mean=sum(raw)/len(raw)
cost=[322727*r/mean for r in raw]; TOT=sum(cost)
F={}
for i,k in enumerate(test):
    u=set(syls(rows[k][0]))
    F.setdefault('novel-syl/token',[]).append(len(u-prior_syl)/cost[i])
    F.setdefault('no-hgm-gloss syl/token',[]).append(len([x for x in u if x not in hgm])/cost[i])
    F.setdefault('not-in-dict syl/token',[]).append(len([x for x in u if x not in allw])/cost[i])
    F.setdefault('distinct-syl/token',[]).append(len(u)/cost[i])
    F.setdefault('combo',[]).append((len(u-prior_syl)+0.3*len([x for x in u if x not in hgm]))/cost[i])
def cap(idx,bf):
    B=TOT*bf; cov=set(); sp=0
    for i in idx:
        if sp+cost[i]>B: continue
        sp+=cost[i]; cov|=(seg_heads[test[i]]-bank)
    return len(cov)/N
print('%-24s %8s %8s'%('ranker','20% tok','30% tok'))
for n,f in F.items():
    idx=sorted(range(len(test)),key=lambda i:-f[i])
    print('%-24s %7.1f%% %7.1f%%'%(n,100*cap(idx,.2),100*cap(idx,.3)))
