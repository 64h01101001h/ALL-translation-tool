import json, collections, re, itertools, statistics
ROOT='/Users/adamderickandrade/ALL-translation-tool'
d=json.load(open(ROOT+'/data/alignment/alignment_full_v1.json'))
L=[l for l in d['links'] if l['d']==5 and l.get('eng') and l.get('tib')]
rend=collections.defaultdict(collections.Counter); segs=collections.defaultdict(set)
for l in L:
    rend[l['tib']][l['eng']]+=1; segs[l['tib']].add((l['course'],l['seg']))
PUNCT=re.compile(r"[^a-z0-9' ]+")
STOP=set("the a an of to and or in on at for by with that this these those it its is are was were be been being there then they we you he she his her our your their my all any as from into one ones s t".split())
def stem(w):
    for s,k in (("ies",1),("ing",0),("ied",1),("es",0),("ed",0),("s",0)):
        if w.endswith(s) and len(w)-len(s)>=3:
            b=w[:-len(s)]
            if k: b+='y'
            if len(b)>1 and b[-1]==b[-2] and b[-1] not in 'sl': b=b[:-1]
            return b
    return w
def ct(e):
    ws=[stem(w) for w in PUNCT.sub(' ',e.lower()).split()]
    cs=[w for w in ws if w not in STOP and len(w)>1]
    return set(cs) if cs else set(ws)
def cl(es):
    T=[ct(e) for e in es]; par=list(range(len(es)))
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
COST=6.69
print('WORKED EXAMPLES — what a heavy headword costs today and what organising it costs')
print('%-16s %5s %6s %7s %9s %9s %10s'%('tib','occ','segs','K','freeClust','flatChars','clustChars'))
for w in ['sogs','chos','gsungs','skye ba','med','las','sdug bsngal','bsod nams']:
    es=list(rend[w]); groups=cl(es)
    flat=sum(len(e)+14 for e in es)
    # clustered presentation: exemplar + count per group + members folded
    clust=sum(len(max(g,key=lambda e:rend[w][e]))+24 for g in groups)
    print('%-16s %5d %6d %7d %9d %9d %10d'%(w,sum(rend[w].values()),len(segs[w]),len(es),len(groups),flat,clust))
# precedent-serving payload, bank-wide
flat=sum(len(e)+14 for w in rend for e in rend[w])
clust=sum(len(max(g,key=lambda e:rend[w][e]))+24 for w in rend for g in cl(list(rend[w])) )
print('\nPRECEDENT PAYLOAD, WHOLE BANK (what an agent greps today vs a clustered index)')
print('  flat rendering list      : %8d chars (~%d tokens)'%(flat,flat//4))
print('  cluster-exemplar list    : %8d chars (~%d tokens)  -> %.2fx smaller'%(clust,clust//4,flat/clust))
print('  raw alignment_full_v1.json on disk: 10,881,971 bytes (what is grepped today)')
print('  -> a precomputed d=5 precedent index is %.0fx smaller than the file agents grep'%(10881971/flat))
print('  -> clustered, %.0fx smaller'%(10881971/clust))
# attribution
nseg=len({(l['course'],l['seg']) for l in L})
print('\nATTRIBUTION OF TODAY\'S SPEND (Map 1 measured $%.2f/segment)'%COST)
print('  %d segments carry d=5 pairs; %d d=5 occurrences; %d distinct pairs'%(nseg,len(L),sum(len(v) for v in rend.values())))
print('  production cost of the CURRENT d=5 bank at that rate: $%.0f'%(nseg*COST))
print('  attributed $/d=5 occurrence: $%.3f ; $/distinct pair: $%.3f'%(nseg*COST/len(L),nseg*COST/sum(len(v) for v in rend.values())))
