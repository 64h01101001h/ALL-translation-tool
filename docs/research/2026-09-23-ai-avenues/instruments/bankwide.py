import json, collections, re, statistics, itertools, time
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
    return tuple(cs) if cs else tuple(ws)
def comps(es, linked):
    par=list(range(len(es)))
    def f(x):
        while par[x]!=x: par[x]=par[par[x]]; x=par[x]
        return x
    for i,j in itertools.combinations(range(len(es)),2):
        if linked(i,j):
            a,b=f(i),f(j)
            if a!=b: par[a]=b
    return len({f(i) for i in range(len(es))})
t0=time.time()
tot_pairs=0; safe=0; A=0; B=0; occ=0
for w,c in rend.items():
    es=list(c); tot_pairs+=len(es); occ+=sum(c.values())
    T=[set(ctoks(e)) for e in es]
    key=[ctoks(e) for e in es]
    safe+=len(set(key))                                    # SAFE: identical content-token multiset after stemming
    B+=len({k[-1] if k else '' for k in key})              # head-token identity
    if len(es)<=1: A+=len(es)
    else: A+=comps(es, lambda i,j: bool(T[i]&T[j]))        # shared content token, connected components
el=time.time()-t0
print('d=5 occurrences %d -> distinct pairs %d (free, already built: %.2fx)'%(occ,tot_pairs,occ/tot_pairs))
print('SAFE  (identical content tokens after stemming): %d groups  -> %.2fx on pairs, %.2fx on occurrences'%(safe,tot_pairs/safe,occ/safe))
print('B     (head content token identity):             %d groups  -> %.2fx on pairs, %.2fx on occurrences'%(B,tot_pairs/B,occ/B))
print('A     (shared content token, components):        %d groups  -> %.2fx on pairs, %.2fx on occurrences'%(A,tot_pairs/A,occ/A))
print('compute time for the whole bank: %.1f s (stdlib only, no model)'%el)
