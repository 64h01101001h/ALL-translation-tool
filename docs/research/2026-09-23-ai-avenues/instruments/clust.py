import json, collections, re, statistics, sqlite3, itertools
d=json.load(open('/Users/adamderickandrade/ALL-translation-tool/data/alignment/alignment_full_v1.json'))
rend=collections.defaultdict(collections.Counter)
for l in d['links']:
    if l['d']==5 and l.get('eng') and l.get('tib'): rend[l['tib']][l['eng']]+=1
PUNCT=re.compile(r"[^a-z0-9' ]+")
STOP=set("the a an of to and or in on at for by with that this these those it its is are was were be been being there then they we you he she his her our your their my all any as from into one ones s t not no".split())
def stem(w):
    for suf,keep in (("ies",1),("ing",0),("ied",1),("es",0),("ed",0),("s",0)):
        if w.endswith(suf) and len(w)-len(suf)>=3:
            b=w[:-len(suf)]
            if keep: b+='y'
            if len(b)>1 and b[-1]==b[-2] and b[-1] not in 'sl': b=b[:-1]
            return b
    return w
def toks(e):
    ws=[stem(w) for w in PUNCT.sub(' ',e.lower()).split()]
    c=[w for w in ws if w not in STOP and len(w)>1]
    return set(c) if c else set(ws)
def components(items, linked):
    par={i:i for i in range(len(items))}
    def f(x):
        while par[x]!=x: par[x]=par[par[x]]; x=par[x]
        return x
    for i,j in itertools.combinations(range(len(items)),2):
        if linked(items[i],items[j]):
            a,b=f(i),f(j)
            if a!=b: par[a]=b
    g=collections.defaultdict(list)
    for i in range(len(items)): g[f(i)].append(items[i])
    return list(g.values())
def methodA(es):  # share any stemmed content token (project-sanctioned mechanical overlap)
    T={e:toks(e) for e in es}
    return components(es, lambda x,y: bool(T[x]&T[y]))
def methodB(es):  # head (last content) token identity
    g=collections.defaultdict(list)
    for e in es:
        t=[w for w in (stem(w) for w in PUNCT.sub(' ',e.lower()).split()) if w not in STOP and len(w)>1]
        g[t[-1] if t else e.lower()].append(e)
    return list(g.values())
def rand_agree(p1,p2,items):
    a1={}; a2={}
    for i,c in enumerate(p1):
        for e in c: a1[e]=i
    for i,c in enumerate(p2):
        for e in c: a2[e]=i
    same=0; tot=0
    for x,y in itertools.combinations(items,2):
        tot+=1
        if (a1[x]==a1[y])==(a2[x]==a2[y]): same+=1
    return same/tot if tot else 1.0

c=sqlite3.connect('/Users/adamderickandrade/ALL-translation-tool/build/hgm_spine_v27_2.db')
cur={}
for w,g in c.execute("SELECT wylie,hgm_gloss FROM entries WHERE tier='curated' AND hgm_gloss IS NOT NULL AND hgm_gloss NOT IN ('[]','null')"):
    try: cur[w]=json.loads(g)
    except Exception: cur[w]=[g]
sel=[w for w in cur if w in rend and len(rend[w])>=3]
print('curated headwords with >=3 bank renderings: %d'%len(sel))
print('\n%-20s %5s %6s %6s %6s %6s %7s'%('wylie','occ','K_bank','K_cur','A','B','A~B'))
rows=[]
for w in sorted(sel, key=lambda w:-sum(rend[w].values()))[:30]:
    es=list(rend[w]); A=methodA(es); B=methodB(es)
    ag=rand_agree(A,B,es)
    rows.append((len(es),len(cur[w]),len(A),len(B),ag))
    print('%-20s %5d %6d %6d %6d %6d %7.2f'%(w,sum(rend[w].values()),len(es),len(cur[w]),len(A),len(B),ag))
allrows=[]
for w in sel:
    es=list(rend[w]); A=methodA(es); B=methodB(es)
    allrows.append((len(es),len(cur[w]),len(A),len(B),rand_agree(A,B,es)))
print('\nover all %d: mean K_bank %.1f -> A %.1f clusters, B %.1f clusters; human K_cur %.2f'%(
    len(allrows),statistics.mean(r[0] for r in allrows),statistics.mean(r[2] for r in allrows),
    statistics.mean(r[3] for r in allrows),statistics.mean(r[1] for r in allrows)))
print('mean pairwise agreement between the two free methods: %.3f (median %.3f)'%(statistics.mean(r[4] for r in allrows),statistics.median(r[4] for r in allrows)))
over=sum(1 for r in allrows if r[2]>r[1]); under=sum(1 for r in allrows if r[2]<r[1])
print('method A vs human count: over-splits %d, under-splits %d, exact %d of %d'%(over,under,len(allrows)-over-under,len(allrows)))
