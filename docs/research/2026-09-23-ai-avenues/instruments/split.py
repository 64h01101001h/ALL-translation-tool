exec(open('suff.py').read().split("random.seed(7)")[0])
import random
random.seed(11)
for k in [1,2,3,5,8,10,15,20,30]:
    ag=[];n=0
    for t,v in hw_fam.items():
        fams=list(v.values())
        if len(fams)<2*k: continue
        n+=1
        for rep in range(40):
            random.shuffle(fams)
            a=top(fams[:k]); b=top(fams[k:2*k])
            if len(a)==1 and len(b)==1: ag.append(1 if a==b else 0)
            else: ag.append(0)
    print('k=%2d headwords=%4d  P(two disjoint k-family samples give the same unique top rendering)=%.3f'%(k,n,sum(ag)/max(len(ag),1)))
# show a few high-frequency headwords and their rendering spread
top10=sorted(hw_fam, key=lambda t:-len(hw_fam[t]))[:12]
for t in top10:
    c=collections.Counter()
    for s in hw_fam[t].values():
        for e in s: c[e]+=1
    print(t, len(hw_fam[t]), c.most_common(5))
