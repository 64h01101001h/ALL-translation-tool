exec(open('suff.py').read().split("random.seed(7)")[0])
import random, re
def fold(e):
    w=e.split()
    if w:
        x=w[-1]
        if len(x)>3 and x.endswith('ies'): x=x[:-3]+'y'
        elif len(x)>3 and x.endswith('s') and not x.endswith('ss'): x=x[:-1]
        w[-1]=x
    return ' '.join(w)
FUNC={'de','dang','ni','kyang','yang','ste','te','nas','la','gyi','kyi','gi','yi','kyis','gyis','gis','yis','su','tu','du','ru','na','ma','mi','med','yin','red','dag','rnams',"'di",'de dag','zhes','ces','sogs','ba','pa','po','bo'}
for lab,f,excl in (('as-is',lambda e:e,False),('plural-fold',fold,False),('plural-fold, no function-word headwords',fold,True)):
    random.seed(11); out=[]
    for k in [1,5,10,20]:
        ag=[];n=0
        for t,v in hw_fam.items():
            if excl and t in FUNC: continue
            fams=[{f(e) for e in s} for s in v.values()]
            if len(fams)<2*k: continue
            n+=1
            for rep in range(40):
                random.shuffle(fams)
                a=top(fams[:k]); b=top(fams[k:2*k])
                ag.append(1 if (len(a)==1 and a==b) else 0)
        out.append('k=%d n=%d P=%.2f'%(k,n,sum(ag)/max(1,len(ag))))
    print(lab,' | '.join(out))
