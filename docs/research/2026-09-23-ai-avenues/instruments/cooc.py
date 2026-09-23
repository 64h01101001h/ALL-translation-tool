import json, sqlite3, re, random, collections, time
t0=time.time()
R='/Users/adamderickandrade/ALL-translation-tool/'
con=sqlite3.connect('file:'+R+'build/hgm_spine_v27_2.db?mode=ro',uri=True)
rows=con.execute("select id,course,seq,wylie,english from corpus_segments").fetchall()
def tsyl(w): return re.sub(r"[/,;|_*!]+"," ",(w or '')).split()
def etok(e): return re.findall(r"[a-z0-9']+",(e or '').lower())
seg={}
for i,c,s,w,e in rows: seg[(c,s)]=(tsyl(w),etok(e))
print('segments',len(seg), round(time.time()-t0,1))
STOP=set("the a an of to and in is that this it be as for by with on or are was his her their its from at which who what not but".split())
# English n-gram df (n<=3)
edf=collections.Counter()
for ts,es in seg.values():
    s=set()
    for n in (1,2,3):
        for i in range(len(es)-n+1): s.add(' '.join(es[i:i+n]))
    edf.update(s)
print('eng ngrams',len(edf), round(time.time()-t0,1))
bank=json.load(open(R+'data/alignment/alignment_full_v1.json'))['links']
cand=[x for x in bank if x['d']==5 and x.get('eng') and x.get('tib')]
random.seed(7); sample=random.sample(cand,2000)
tset=set(' '.join(tsyl(x['tib'])) for x in sample)
tpost=collections.defaultdict(list)
for k,(ts,es) in seg.items():
    s=set()
    for n in range(1,7):
        for i in range(len(ts)-n+1):
            g=' '.join(ts[i:i+n])
            if g in tset: s.add(g)
    for g in s: tpost[g].append(k)
print('tib postings built', round(time.time()-t0,1))
engcache={}
def egrams(k):
    if k not in engcache:
        es=seg[k][1]; s=set()
        for n in (1,2,3):
            for i in range(len(es)-n+1): s.add(' '.join(es[i:i+n]))
        engcache[k]=s
    return engcache[k]
res=collections.Counter(); byfreq=collections.defaultdict(collections.Counter)
random.seed(11)
for x in sample:
    k=(x['course'],x['seg'])
    if k not in seg: res['noseg']+=1; continue
    t=' '.join(tsyl(x['tib'])); e=' '.join(etok(x['eng']))
    nw=len(e.split())
    if nw==0 or nw>3: res['eng_len_gt3_or_0']+=1; continue
    post=tpost.get(t,[])
    if not post: res['tib_not_found']+=1; continue
    dft=len(post); scale=1.0
    P=post
    if dft>3000: P=random.sample(post,3000); scale=dft/3000
    joint=collections.Counter()
    for kk in P: joint.update(egrams(kk))
    cands=[g for g in egrams(k) if not all(w in STOP for w in g.split())]
    if e not in egrams(k): res['eng_not_in_seg_tokens']+=1; continue
    sc={g:2*joint[g]*scale/(dft+edf[g]) for g in cands}
    ranked=sorted(sc,key=lambda g:-sc[g])
    r=ranked.index(e)+1 if e in sc else 999
    band='df1' if dft==1 else 'df2-5' if dft<=5 else 'df6-50' if dft<=50 else 'df51+'
    res['scored']+=1
    for kname,ok in (('top1',r==1),('top3',r<=3),('top10',r<=10)):
        if ok: res[kname]+=1; byfreq[band][kname]+=1
    byfreq[band]['n']+=1
print(dict(res))
for b in ('df1','df2-5','df6-50','df51+'):
    c=byfreq[b]; n=c['n'] or 1
    print(b,'n',c['n'],'top1 %.1f%% top3 %.1f%% top10 %.1f%%'%(100*c['top1']/n,100*c['top3']/n,100*c['top10']/n))
n=res['scored']; print('ALL top1 %.1f%% top3 %.1f%% top10 %.1f%%'%(100*res['top1']/n,100*res['top3']/n,100*res['top10']/n), round(time.time()-t0,1))
