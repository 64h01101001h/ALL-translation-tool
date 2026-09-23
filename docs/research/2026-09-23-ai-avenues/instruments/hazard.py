import json, collections, re, itertools, sqlite3, statistics, math
d=json.load(open('/Users/adamderickandrade/ALL-translation-tool/data/alignment/alignment_full_v1.json'))
rend=collections.defaultdict(collections.Counter)
for l in d['links']:
    if l['d']==5 and l.get('eng') and l.get('tib'): rend[l['tib']][l['eng']]+=1
PUNCT=re.compile(r"[^a-z0-9' ]+")
STOP=set("the a an of to and or in on at for by with that this these those it its is are was were be been being there then they we you he she his her our your their my all any as from into one ones s t".split())
def stem(w):
    for suf,keep in (("ies",1),("ing",0),("ied",1),("es",0),("ed",0),("s",0)):
        if w.endswith(suf) and len(w)-len(suf)>=3:
            b=w[:-len(suf)]
            if keep: b+='y'
            if len(b)>1 and b[-1]==b[-2] and b[-1] not in 'sl': b=b[:-1]
            return b
    return w
def ctoks(e):
    ws=[stem(w) for w in PUNCT.sub(' ',e.lower()).split()]
    cs=[w for w in ws if w not in STOP and len(w)>1]
    return set(cs) if cs else set(ws)
def clusters(es):
    T=[ctoks(e) for e in es]; par=list(range(len(es)))
    def f(x):
        while par[x]!=x: par[x]=par[par[x]]; x=par[x]
        return x
    for i,j in itertools.combinations(range(len(es)),2):
        if T[i]&T[j]:
            a,b=f(i),f(j)
            if a!=b: par[a]=b
    g=collections.defaultdict(list)
    for i,e in enumerate(es): g[f(i)].append(e)
    return list(g.values())
c=sqlite3.connect('/Users/adamderickandrade/ALL-translation-tool/build/hgm_spine_v27_2.db')
cur={}
for w,g in c.execute("SELECT wylie,hgm_gloss FROM entries WHERE tier='curated' AND hgm_gloss IS NOT NULL AND hgm_gloss NOT IN ('[]','null')"):
    try: cur[w]=json.loads(g)
    except Exception: cur[w]=[g]
sel=[w for w in cur if w in rend and len(rend[w])>=3]
print('=== UNDER-SPLITS: free clustering makes FEWER groups than the human sense count ===')
n=0
for w in sel:
    cl=clusters(list(rend[w]))
    if len(cl)<len(cur[w]):
        n+=1
        print('\n%-22s K_bank=%d  free groups=%d  HUMAN senses=%d %s'%(w,len(rend[w]),len(cl),len(cur[w]),cur[w]))
        for g in sorted(cl,key=len,reverse=True)[:3]:
            print('    group(%d): %s'%(len(g),' | '.join(sorted(g)[:14])))
print('\nunder-split heads: %d of %d'%(n,len(sel)))
# fit K_cur ~ K_bank
xs=[len(rend[w]) for w in cur if w in rend]; ys=[len(cur[w]) for w in cur if w in rend]
print('\n=== curated sense count vs bank rendering count (n=%d) ==='%len(xs))
lx=[math.log(x) for x in xs]; ly=[math.log(y) for y in ys]
mx=statistics.mean(lx); my=statistics.mean(ly)
b=sum((a-mx)*(c2-my) for a,c2 in zip(lx,ly))/sum((a-mx)**2 for a in lx); k=math.exp(my-b*mx)
print('power fit  K_cur = %.3f * K_bank^%.3f'%(k,b))
for kb in (2,5,10,20,46,63): print('   K_bank=%2d -> predicted human senses %.2f'%(kb,k*kb**b))
