import json, collections
R='/Users/adamderickandrade/ALL-translation-tool/'
L=json.load(open(R+'data/alignment/alignment_full_v1.json'))['links']
d5=[l for l in L if l['d']==5 and l['eng'] and l['seg'] is not None]
pc=collections.Counter((l['tib'].strip(),l['eng'].strip()) for l in d5)
by=collections.defaultdict(list)
for (t,e),n in pc.items(): by[t].append((n,e))
for k in (2,3,5):
    c=[t for t,v in by.items() if sum(1 for n,e in v if n>=k)>=2]
    print(f'headwords with >=2 renderings each attested >={k}x:',len(c))
# course spread per rendering for a few known splits
for t in ["skyabs su 'gro","gus pa","bsod nams","chos"]:
    v=sorted(by.get(t,[]),reverse=True)[:6]; print(t, v)
