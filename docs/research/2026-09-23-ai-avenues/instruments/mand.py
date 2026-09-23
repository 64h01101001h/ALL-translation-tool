exec(open('proxy.py').read().split("print('forward:")[0])
H=set(tuple(t.split()) for k in segs for t in seg_h[k])
seenw=set(rows[k][0] for k in landed); seen=set(); m=[]
for (c,s),(w,e) in sorted(rows.items()):
    if c not in MAND or (c,s) in landed or w in seenw or w in seen: continue
    seen.add(w); m.append((c,s))
p=proxy(m,H); print('mandate Tibetan-unique unread',p)
# of the 4,073 pair-unique, how many share Tibetan with a landed segment or another remaining one (different English)
wl=[rows[k][0] for k in rem]
import collections
print('4,073 pair-unique: Tibetan identical to landed:',sum(1 for w in wl if w in seenw),' distinct Tibetan among them:',len(set(wl)))
