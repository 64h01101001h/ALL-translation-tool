import json, collections, statistics as st, re
d=json.load(open('/Users/adamderickandrade/ALL-translation-tool/data/alignment/alignment_full_v1.json'))
L=d['links']
bydepth=collections.Counter(l['d'] for l in L)
print('by depth', sorted(bydepth.items()))
d5=[l for l in L if l['d']==5 and l['eng']]
print('d5 with eng', len(d5))
segs=collections.defaultdict(list)
for l in d5: segs[(l['course'],l['seg'])].append(l)
sizes=[len(v) for v in segs.values()]
print('segments with d5', len(segs), 'mean m', st.mean(sizes), 'median', st.median(sizes), 'max', max(sizes))
bycourse=collections.Counter(k[0] for k in segs)
print(bycourse)
# proxy defect: english span begins with supplied function word (the gate's pattern)
pat=re.compile(r"^(the|a|an|and|or|his|our|your|I|you)\b", re.I)
flags={k:[1 if pat.match(l['eng'].strip()) else 0 for l in v] for k,v in segs.items()}
N=sum(len(v) for v in flags.values()); p=sum(sum(v) for v in flags.values())/N
print('flag rate', p, 'flags', sum(sum(v) for v in flags.values()))
# ANOVA ICC estimator for binary outcome
k=len(flags); groups=list(flags.values())
n0=(N - sum(len(g)**2 for g in groups)/N)/(k-1)
grand=p
msb=sum(len(g)*(st.mean(g)-grand)**2 for g in groups)/(k-1)
msw=sum(sum((x-st.mean(g))**2 for x in g) for g in groups)/(N-k)
icc=(msb-msw)/(msb+(n0-1)*msw)
print('n0',n0,'MSB',msb,'MSW',msw,'ICC',icc)
for c in sorted(bycourse):
    gs=[v for kk,v in flags.items() if kk[0]==c]
    NN=sum(len(g) for g in gs); pp=sum(sum(g) for g in gs)/NN
    kk=len(gs)
    if kk<3: continue
    n00=(NN - sum(len(g)**2 for g in gs)/NN)/(kk-1)
    msb=sum(len(g)*(st.mean(g)-pp)**2 for g in gs)/(kk-1)
    msw=sum(sum((x-st.mean(g))**2 for x in g) for g in gs)/(NN-kk)
    print(c, 'segs',kk,'spans',NN,'flag',round(pp,4),'ICC',round((msb-msw)/(msb+(n00-1)*msw),4))
