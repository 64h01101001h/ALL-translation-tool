import json, collections, statistics
B='/Users/adamderickandrade/ALL-translation-tool/data/alignment/alignment_full_v1.json'
d=json.load(open(B)); L=d['links']
sub=[l for l in L if l['d']==5 and l.get('eng') and l.get('tib')]
rend=collections.defaultdict(collections.Counter)
for l in sub: rend[l['tib']][l['eng']]+=1
pairs=[(t,e,n) for t,c in rend.items() for e,n in c.items()]
print('distinct (tib,eng) pairs', len(pairs))
fh=collections.Counter(n for _,_,n in pairs)
tot=len(pairs)
print('pair frequency: n=1 %d (%.1f%%)  n=2 %d  n>=3 %d'%(fh[1],100*fh[1]/tot,fh[2],sum(v for k,v in fh.items() if k>=3)))
print('occurrences carried by hapax pairs: %d / %d = %.1f%%'%(fh[1],len(sub),100*fh[1]/len(sub)))

# coverage curve within polysemous headwords
print('\n--- within-headword rendering concentration (K>=2 headwords, n=%d) ---'%sum(1 for c in rend.values() if len(c)>1))
c80=[];c90=[];top1=[]
for t,c in rend.items():
    if len(c)<2: continue
    N=sum(c.values()); cum=0; k80=k90=None
    for i,(e,n) in enumerate(c.most_common(),1):
        cum+=n
        if k80 is None and cum>=0.8*N: k80=i
        if k90 is None and cum>=0.9*N: k90=i
    c80.append(k80/len(c)); c90.append(k90/len(c)); top1.append(c.most_common(1)[0][1]/N)
print('median share of occurrences taken by the SINGLE most common rendering: %.2f  (mean %.2f)'%(statistics.median(top1),statistics.mean(top1)))
print('median fraction of K needed for 80%% of occurrences: %.2f ; for 90%%: %.2f'%(statistics.median(c80),statistics.median(c90)))

# the heavy tail: headwords with many occurrences
print('\n--- heavy headwords: K vs occurrences vs hapax renderings ---')
print('%-16s %5s %4s %6s %8s %8s'%('tib','occ','K','hapaxK','top1share','K/occ'))
for t,c in sorted(rend.items(), key=lambda x:-sum(x[1].values()))[:15]:
    N=sum(c.values()); hp=sum(1 for e,n in c.items() if n==1)
    print('%-16s %5d %4d %6d %8.2f %8.2f'%(t,N,len(c),hp,c.most_common(1)[0][1]/N,len(c)/N))
# aggregate over K>=10
hs=[t for t,c in rend.items() if len(c)>=10]
N=sum(sum(rend[t].values()) for t in hs); K=sum(len(rend[t]) for t in hs)
hp=sum(sum(1 for n in rend[t].values() if n==1) for t in hs)
print('\nK>=10 headwords: %d, occurrences %d, distinct renderings %d, of which hapax %d (%.1f%%)'%(len(hs),N,K,hp,100*hp/K))
