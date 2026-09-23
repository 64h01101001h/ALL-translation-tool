exec(open('cov.py').read().split("def incidence_stats")[0])
import collections
landed=set((l['course'],l['seg']) for l in L if l['seg'] is not None)
MAND=['C%02d'%i for i in range(1,19) if i!=13]
seen_pairs=set(rows[k] for k in landed)
rem=[];seenr=set()
for (c,s),(w,e) in sorted(rows.items()):
    if c not in MAND or (c,s) in landed: continue
    if (w,e) in seen_pairs or (w,e) in seenr: continue
    seenr.add((w,e)); rem.append((c,s))
print('remaining unique', len(rem))
def sylls(t): return norm_tib(t).split()
def cover(text_sylls, H, maxlen):
    i=0; unc=[]; runs=[]; cur=[]
    n=len(text_sylls)
    while i<n:
        m=0
        for ln in range(min(maxlen,n-i),0,-1):
            if tuple(text_sylls[i:i+ln]) in H: m=ln;break
        if m: 
            if cur: runs.append(tuple(cur)); cur=[]
            i+=m
        else:
            unc.append(text_sylls[i]); cur.append(text_sylls[i]); i+=1
    if cur: runs.append(tuple(cur))
    return unc,runs
def proxy(segkeys,H):
    maxlen=max(len(h) for h in H)
    tot=0;unc_tok=0;types=set();runs=set();segs0=0
    for k in segkeys:
        s=sylls(rows[k][0]); tot+=len(s)
        u,r=cover(s,H,maxlen); unc_tok+=len(u); types|=set(u); runs|=set(r)
        if not u: segs0+=1
    return dict(syl=tot,unc=unc_tok,frac=unc_tok/max(tot,1),types=len(types),runs=len(runs),segs0=segs0,n=len(segkeys))
C=['C01','C02','C03','C04','C05']
print('forward: course, proxy(on the banked segs of Ck vs bank<k), actual new headwords')
res=[]
for i in range(1,5):
    ref=set(C[:i]); tgt=C[i]
    H=set(tuple(t.split()) for k in segs if k[0] in ref for t in seg_h[k])
    tk=[k for k in landed if k[0]==tgt and fam_of.get(k) not in set(fam_of[x] for x in segs if x[0] in ref)]
    p=proxy(tk,H)
    actual=len(set(t for k in tk if k in seg_h for t in seg_h[k]) - set(' '.join(h) for h in H))
    res.append((tgt,p,actual))
    print(tgt,p,'actual',actual,'actual/runs=%.2f actual/types=%.2f actual/unc=%.3f'%(actual/p['runs'],actual/max(p['types'],1),actual/p['unc']))
H=set(tuple(t.split()) for k in segs for t in seg_h[k])
p=proxy(rem,H); print('REMAINDER vs full bank',p)
bycourse=collections.defaultdict(list)
for k in rem: bycourse[k[0]].append(k)
for c in sorted(bycourse):
    q=proxy(bycourse[c],H); print(c,q['n'],'frac=%.3f'%q['frac'],'runs',q['runs'],'types',q['types'],'segs0',q['segs0'])
