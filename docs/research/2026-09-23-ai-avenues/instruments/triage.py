import json, collections, re, statistics, itertools, sqlite3, math
d=json.load(open('/Users/adamderickandrade/ALL-translation-tool/data/alignment/alignment_full_v1.json'))
links=[l for l in d['links'] if l['d']==5 and l.get('eng') and l.get('tib')]
rend=collections.defaultdict(collections.Counter)
refs=collections.defaultdict(lambda: collections.defaultdict(list))
for l in links:
    rend[l['tib']][l['eng']]+=1
    refs[l['tib']][l['eng']].append('%s:%s'%(l['course'],l['seg']))
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
def ncomp(es):
    if len(es)<=1: return len(es)
    T=[ctoks(e) for e in es]; par=list(range(len(es)))
    def f(x):
        while par[x]!=x: par[x]=par[par[x]]; x=par[x]
        return x
    for i,j in itertools.combinations(range(len(es)),2):
        if T[i]&T[j]:
            a,b=f(i),f(j)
            if a!=b: par[a]=b
    return len({f(i) for i in range(len(es))})

buckets=collections.Counter(); payload=collections.Counter(); heads=collections.defaultdict(list)
for w,c in rend.items():
    es=list(c); k=len(es); a=ncomp(es)
    if k==1: b='T0 monosemous (1 rendering)'
    elif a==1: b='T1 free-clusterable to ONE group'
    elif a<=3: b='T2 free-clusters to 2-3 groups'
    else: b='T3 free-clusters to >3 groups'
    buckets[b]+=1; heads[b].append(w)
    # payload: headword + each rendering + count + first 3 refs
    pl=len(w)+sum(len(e)+12+sum(len(r)+2 for r in refs[w][e][:3]) for e in es)
    payload[b]+=pl
tot=sum(buckets.values())
print('%-34s %6s %6s %12s %10s'%('triage bucket','heads','%','chars payload','~tokens'))
for b in sorted(buckets):
    print('%-34s %6d %5.1f%% %12d %10d'%(b,buckets[b],100*buckets[b]/tot,payload[b],payload[b]//4))
print('%-34s %6d        %12d %10d'%('TOTAL',tot,sum(payload.values()),sum(payload.values())//4))
print()
for b in sorted(buckets):
    occ=sum(sum(rend[w].values()) for w in heads[b]); pr=sum(len(rend[w]) for w in heads[b])
    print('%-34s occurrences %6d  distinct renderings %6d'%(b,occ,pr))
# the T3 heads are the ones a frontier pass would actually be aimed at
T3=heads['T3 free-clusters to >3 groups']
print('\nT3 heads: %d, renderings %d, occurrences %d, payload %d chars (~%d tokens)'%(
  len(T3),sum(len(rend[w]) for w in T3),sum(sum(rend[w].values()) for w in T3),payload['T3 free-clusters to >3 groups'],payload['T3 free-clusters to >3 groups']//4))
print('largest T3 heads by renderings:', ', '.join('%s(K=%d)'%(w,len(rend[w])) for w in sorted(T3,key=lambda w:-len(rend[w]))[:12]))
