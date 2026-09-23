exec(open('neardup.py').read().split("for thr in")[0])
# union-find over all near-dup edges >= thr among reps (landed + remaining)
import itertools
def clusters(thr):
    par={}
    def f(x):
        par.setdefault(x,x)
        while par[x]!=x:
            par[x]=par[par[x]]; x=par[x]
        return x
    for k in remaining:
        cands=collections.Counter()
        for g in T[k]:
            if df[g]<=40:
                for o in inv[g]:
                    if o!=k: cands[o]+=1
        for o,_ in cands.most_common(50):
            if min(jac(T[k],T[o]),jac(E[k],E[o]))>=thr:
                par[f(k)]=f(o)
    groups=collections.defaultdict(list)
    for k in remaining: groups[f(k)].append(k)
    # also mark groups containing a landed segment
    landedroots={f(k) for k in par if k in landed}
    saved=0
    for r,ms in groups.items():
        if r in landedroots: saved+=len(ms)
        else: saved+=len(ms)-1
    return saved
for thr in (0.95,0.9,0.8):
    print("thr",thr,"remaining segments reducible to a diff-transfer:",clusters(thr))
