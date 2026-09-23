exec(open('cov.py').read().split("def incidence_stats")[0])
import collections, random, statistics
# per headword: list of families, each family -> set of renderings (trimmed eng)
hw_fam=collections.defaultdict(dict)
for f,m in fam_members.items():
    for k in m:
        for (t,e) in seg_p[k]:
            hw_fam[t].setdefault(f,set()).add(e)
nf=collections.Counter(len(v) for v in hw_fam.values())
print('headwords by #families (with english):', [(k,nf[k]) for k in sorted(nf)[:12]], '>=10:',sum(v for k,v in nf.items() if k>=10), '>=20:',sum(v for k,v in nf.items() if k>=20),'>=5:',sum(v for k,v in nf.items() if k>=5))
multi=[t for t,v in hw_fam.items() if len(v)>=10]
nrend=[len(set().union(*v.values())) for t,v in hw_fam.items() if len(v)>=10]
print('headwords >=10 fams:',len(multi),'mean distinct renderings',statistics.mean(nrend),'single-rendering share',sum(1 for x in nrend if x==1)/len(nrend))
def top(fams):
    c=collections.Counter()
    for s in fams:
        for e in s: c[e]+=1
    mx=max(c.values()); tops=sorted(e for e,v in c.items() if v==mx)
    return tops
random.seed(7)
K=[1,2,3,4,5,6,8,10,15,20]
agree={k:[] for k in K}; unique_top={k:[] for k in K}
for t in multi:
    fams=list(hw_fam[t].values())
    final=top(fams)
    if len(final)>1: continue  # final tie: skip
    for rep in range(50):
        random.shuffle(fams)
        for k in K:
            if k>len(fams): continue
            tp=top(fams[:k])
            agree[k].append(1 if tp==final else 0)
            unique_top[k].append(1 if len(tp)==1 else 0)
for k in K:
    if agree[k]: print('k=%2d  P(top-1 at k == top-1 at all, no tie)=%.3f  n=%d  P(no tie at k)=%.3f'%(k,sum(agree[k])/len(agree[k]),len(agree[k])//50,sum(unique_top[k])/len(unique_top[k])))
# restrict to headwords with >=2 renderings (contested)
print('--- contested headwords only (>=2 distinct renderings, >=10 fams)')
agree={k:[] for k in K}
cont=0
for t in multi:
    fams=list(hw_fam[t].values())
    if len(set().union(*fams))<2: continue
    final=top(fams)
    if len(final)>1: continue
    cont+=1
    for rep in range(50):
        random.shuffle(fams)
        for k in K:
            if k>len(fams): continue
            agree[k].append(1 if top(fams[:k])==final else 0)
print('contested n',cont)
for k in K:
    if agree[k]: print('k=%2d  P(top-1 stable)=%.3f'%(k,sum(agree[k])/len(agree[k])))
# syllable-length of headword singletons vs repeated (families)
inc=collections.Counter()
for f,s in fam_h.items():
    for t in s: inc[t]+=1
def slen(t): return len(t.split())
for lab,sel in (('singleton',[t for t,v in inc.items() if v==1]),('repeated',[t for t,v in inc.items() if v>=2])):
    c=collections.Counter(min(slen(t),6) for t in sel)
    print(lab,len(sel),{k:round(c[k]/len(sel),3) for k in sorted(c)})
