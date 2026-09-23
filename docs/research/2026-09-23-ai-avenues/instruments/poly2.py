import json, collections, statistics
B='/Users/adamderickandrade/ALL-translation-tool/data/alignment/alignment_full_v1.json'
d=json.load(open(B)); L=d['links']
sub=[l for l in L if l['d']==5 and l.get('eng') and l.get('tib')]
occ=collections.Counter(l['tib'] for l in sub)
rend=collections.defaultdict(collections.Counter)
for l in sub: rend[l['tib']][l['eng']]+=1
N=len(rend)
print('d=5 headwords', N, 'occurrences(links)', len(sub))
# distribution of distinct renderings
hist=collections.Counter(len(v) for v in rend.values())
cum=0
print('\nK (distinct renderings) | headwords | %% | cum%%')
for k in sorted(hist):
    cum+=hist[k]
    if k<=12 or k%10==0:
        print('%3d  %6d  %5.2f%%  %5.1f%%'%(k,hist[k],100*hist[k]/N,100*cum/N))
print('max K', max(hist))
# occurrence concentration
tot=len(sub)
byocc=sorted(occ.items(), key=lambda x:-x[1])
print('\ntop-20 by occurrence count:')
for t,c in byocc[:20]:
    print('%5d occ  K=%3d  %r'%(c,len(rend[t]),t))
# what share of occurrences sit in headwords with K>=2
multi_occ=sum(occ[t] for t in rend if len(rend[t])>1)
print('\noccurrences in K>=2 headwords: %d/%d = %.1f%%'%(multi_occ,tot,100*multi_occ/tot))
for thr in (2,3,5,10,20):
    hs=[t for t in rend if len(rend[t])>=thr]
    print('K>=%2d : %5d headwords, %6d occurrences (%.1f%% of all occ)'%(thr,len(hs),sum(occ[t] for t in hs),100*sum(occ[t] for t in hs)/tot))
# hapax
print('\nheadwords occurring once:', sum(1 for t in occ if occ[t]==1), '=%.1f%%'%(100*sum(1 for t in occ if occ[t]==1)/N))
