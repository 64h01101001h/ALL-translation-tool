import json, collections, re, sqlite3
REPO='/Users/adamderickandrade/ALL-translation-tool/'
bank=json.load(open(REPO+'data/alignment/alignment_full_v1.json'))
L=[x for x in bank['links'] if x['seg'] is not None]
ART=re.compile(r'^(the|a|an|of|to|in|for|that|this|his|her|their|its|our|your|my|and|is|are|be|been|will|would)\s+')
def nt(s): return (s or '').strip().strip(',.;:!?').lower()
def ne(s):
    if s is None: return None
    e=s.strip().lower().strip('".,;:!?()'); e=re.sub(r'\s+',' ',e)
    p=None
    while p!=e: p=e; e=ART.sub('',e)
    e=re.sub(r"(\w)'s$",r'\1',e)
    if len(e)>3 and e.endswith('s') and not e.endswith('ss'): e=e[:-1]
    return e
byseg=collections.defaultdict(list)
for x in L:
    if x['d'] in (5,6,7): byseg[(x['course'],x['seg'])].append(x)
order=sorted(byseg, key=lambda k:(k[0],k[1]))
seen_t=set(); seen_p=set()
recs=[]
for k in order:
    tset={nt(x['tib']) for x in byseg[k] if x['tib']}
    pset={(nt(x['tib']),ne(x['eng'])) for x in byseg[k] if x['tib'] and x['eng']}
    nt_new=len(tset-seen_t); np_new=len(pset-seen_p)
    recs.append((k,nt_new,np_new,len(tset),len(pset)))
    seen_t|=tset; seen_p|=pset
print('landed segments:',len(recs),'final tib types',len(seen_t),'final pairs',len(seen_p))
zt=[r for r in recs if r[1]==0]
print(f"segments contributing ZERO new Tibetan types: {len(zt)} ({100*len(zt)/len(recs):.1f}%)")
print(f"  ...but they contribute {sum(r[2] for r in zt)} new (tib,normEng) pairs = {100*sum(r[2] for r in zt)/sum(r[2] for r in recs):.1f}% of all new pairs")
# decile marginal
N=len(recs)
print("\nmarginal per decile of reading order (bank, real data):")
for d in range(10):
    a,b=int(N*d/10),int(N*(d+1)/10)
    s=recs[a:b]
    print(f"  d{d+1}: new tib types/seg {sum(r[1] for r in s)/len(s):5.2f}   new pairs/seg {sum(r[2] for r in s)/len(s):5.2f}")
print("\nfirst decile vs last decile ratio: tib %.1fx  pairs %.1fx"%(
   (sum(r[1] for r in recs[:N//10])/ (N//10))/max(1e-9,(sum(r[1] for r in recs[-(N//10):])/(N//10))),
   (sum(r[2] for r in recs[:N//10])/ (N//10))/max(1e-9,(sum(r[2] for r in recs[-(N//10):])/(N//10)))))
json.dump([[list(k),a,b,c,d] for k,a,b,c,d in recs], open('/private/tmp/claude-501/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2/scratchpad/bank_recs.json','w'))
