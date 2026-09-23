import json, collections, re, itertools
d=json.load(open('/Users/adamderickandrade/ALL-translation-tool/data/alignment/alignment_full_v1.json'))
rend=collections.defaultdict(collections.Counter)
for l in d['links']:
    if l['d']==5 and l.get('eng') and l.get('tib'): rend[l['tib']][l['eng']]+=1
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
    return sorted(g.values(), key=lambda g:-sum(rend_cur[x] for x in g))
for w in ['chos','sdug bsngal',"'gro ba"]:
    rend_cur=rend[w]
    print('=== %s  (occ=%d, K=%d) -> free clusters:'%(w,sum(rend[w].values()),len(rend[w])))
    for g in cl(list(rend[w])):
        ex=max(g,key=lambda e:rend[w][e])
        print('  [%2d occ] exemplar %-28r  members: %s'%(sum(rend[w][e] for e in g),ex,' | '.join(sorted(g,key=lambda e:-rend[w][e]))[:130]))
    print()
