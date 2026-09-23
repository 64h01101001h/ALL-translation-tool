exec(open('suff.py').read().split("random.seed(7)")[0])
def fold(e):
    return ' '.join(w[:-1] if len(w)>3 and w.endswith('s') and not w.endswith('ss') else w for w in e.split())
for lab,fn in (('raw',lambda e:e),('plural-folded',fold)):
    rows_=[]
    for t,v in hw_fam.items():
        fams=[set(fn(e) for e in s) for s in v.values()]
        T=len(fams)
        if T<2: continue
        inc=collections.Counter(e for s in fams for e in s)
        U=sum(inc.values()); Q1=sum(1 for x in inc.values() if x==1); Q2=sum(1 for x in inc.values() if x==2)
        C=(1-(Q1/U)*((T-1)*Q1/((T-1)*Q1+2*Q2))) if Q1 else 1.0
        rows_.append((T,C))
    for lo,hi in ((2,4),(5,9),(10,19),(20,49),(50,999)):
        cs=[c for T,c in rows_ if lo<=T<=hi]
        if cs: print(lab,'fams %d-%d: n=%d  median coverage=%.2f  share>=0.8: %.2f  share>=0.9: %.2f'%(lo,hi,len(cs),statistics.median(cs),sum(c>=0.8 for c in cs)/len(cs),sum(c>=0.9 for c in cs)/len(cs)))
