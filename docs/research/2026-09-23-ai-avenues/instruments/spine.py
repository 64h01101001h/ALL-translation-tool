exec(open('proxy.py').read().split("print('forward:")[0])
import re
H=set(tuple(t.split()) for k in segs for t in seg_h[k])
seenw=set(rows[k][0] for k in landed)
grp=collections.defaultdict(list); seen=set()
for (c,s),(w,e) in sorted(rows.items()):
    if (c,s) in landed or w in seenw or w in seen or not norm_tib(w): continue
    seen.add(w)
    g=re.match(r'[A-Z]+',c).group(0) if re.match(r'[A-Z]+',c) else c
    if g=='C': g='C-mandate' if c in MAND else ('C13' if c=='C13' else 'C19+')
    grp[g].append((c,s))
tot=[]
for g in sorted(grp, key=lambda g:-len(grp[g])):
    p=proxy(grp[g],H); tot+=grp[g]
    print('%-10s uniqueTib=%5d syl=%7d uncovered=%.3f types=%5d runs=%6d'%(g,p['n'],p['syl'],p['frac'],p['types'],p['runs']))
p=proxy(tot,H); print('ALL unread (Tibetan-unique)',p)
