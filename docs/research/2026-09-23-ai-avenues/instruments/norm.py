import json, collections, re, statistics
B='/Users/adamderickandrade/ALL-translation-tool/data/alignment/alignment_full_v1.json'
d=json.load(open(B)); L=d['links']
sub=[l for l in L if l['d']==5 and l.get('eng') and l.get('tib')]
rend=collections.defaultdict(collections.Counter)
for l in sub: rend[l['tib']][l['eng']]+=1

PUNCT=re.compile(r'[^a-z0-9\' ]+')
LEAD=r"(the|a|an|of|to|and|or|in|on|at|for|with|by|that|this|these|those|it|its|is|are|was|were|be|been|being|there|then|they|we|you|he|she|his|her|our|your|their|my|all|any|as|from|into|his|one's|one)"
LEADRE=re.compile(r'^(?:%s\s+)+'%LEAD)
TRAILRE=re.compile(r'\s+(?:%s)$'%LEAD)
def n1(e):  # case + punctuation + whitespace
    return PUNCT.sub(' ', e.lower()).strip()
def n2(e):  # + strip supplied leading/trailing function words
    s=n1(e)
    prev=None
    while s!=prev:
        prev=s
        s=LEADRE.sub('',s); s=TRAILRE.sub('',s); s=s.strip()
    return s or n1(e)
def stem(w):
    for suf,keep in (("ies",1),("ing",0),("ied",1),("es",0),("ed",0),("s",0),("'s",0)):
        if w.endswith(suf) and len(w)-len(suf)>=3:
            base=w[:-len(suf)]
            if keep: base+='y'
            if len(base)>1 and base[-1]==base[-2] and base[-1] not in 'sl': base=base[:-1]
            return base
    return w
def n3(e):  # + crude stemming of every token
    return ' '.join(stem(w) for w in n2(e).split()) or n2(e)
def n4(e):  # + head token only (last content token)
    t=n3(e).split()
    return t[-1] if t else n3(e)

levels=[('L0 raw',lambda e:e),('L1 case+punct',n1),('L2 +function words',n2),('L3 +stemming',n3),('L4 +head token',n4)]
rows=[]
for name,f in levels:
    tot=0; multi=0; ks=[]
    for t,c in rend.items():
        k=len({f(e) for e in c})
        tot+=k; ks.append(k)
        if k>1: multi+=1
    rows.append((name,tot,multi,statistics.mean(ks),max(ks)))
print('%-22s %8s %8s %8s %6s'%('normalisation','total K','K>=2 hw','mean K','max K'))
for r in rows:
    print('%-22s %8d %8d %8.2f %6d'%r)
base=rows[0][1]
print()
for name,tot,multi,mn,mx in rows:
    print('%-22s collapses %.1f%% of distinct renderings; %d/%d headwords still K>=2 (%.1f%%)'%(name,100*(1-tot/base),multi,len(rend),100*multi/len(rend)))

# per-headword for the showcase terms
print('\n--- collapse on the heaviest headwords ---')
for t in ['sogs','chos','gsungs','skye ba','med','don','yod','byed pa','las','sdug bsngal','bsod nams','stong pa nyid']:
    if t not in rend: print(t,'absent'); continue
    c=rend[t]
    print('%-14s occ=%3d  K0=%2d  K1=%2d  K2=%2d  K3=%2d  K4=%2d'%(t,sum(c.values()),len(c),len({n1(e) for e in c}),len({n2(e) for e in c}),len({n3(e) for e in c}),len({n4(e) for e in c})))
