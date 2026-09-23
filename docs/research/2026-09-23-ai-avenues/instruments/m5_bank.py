import json, collections, re
REPO='/Users/adamderickandrade/ALL-translation-tool/'
bank=json.load(open(REPO+'data/alignment/alignment_full_v1.json'))
L=[x for x in bank['links'] if x['seg'] is not None]
segs=set((x['course'],x['seg']) for x in L)
print('landed segments:', len(segs), 'links:', len(L))

W=[x for x in L if x['d'] in (5,6,7)]
print('word-layer links (d=5/6/7):', len(W), 'per segment %.1f'%(len(W)/len(segs)))
d5=[x for x in L if x['d']==5]
print('d=5 (the dictionary layer):', len(d5), 'per segment %.1f'%(len(d5)/len(segs)))

def norm_t(s): return (s or '').strip().strip(',.;:!?').lower()
ART=re.compile(r'^(the|a|an|of|to|in|for|that|this|his|her|their|its|our|your|my|and|is|are|be|been|will|would)\s+')
def norm_e(s):
    if s is None: return None
    e=s.strip().lower().strip('".,;:!?()')
    e=re.sub(r'\s+',' ',e)
    prev=None
    while prev!=e:
        prev=e; e=ART.sub('',e)
    e=re.sub(r"(\w)'s$", r'\1', e)
    if len(e)>3 and e.endswith('s') and not e.endswith('ss'): e=e[:-1]
    return e

for label, S in (('all d=5/6/7', W), ('d=5 only', d5)):
    occ=len(S)
    tib=collections.Counter(norm_t(x['tib']) for x in S)
    pairs=collections.Counter((norm_t(x['tib']), x['eng']) for x in S)
    npairs=collections.Counter((norm_t(x['tib']), norm_e(x['eng'])) for x in S)
    nonnull=[x for x in S if x['eng']]
    npairs_nn=collections.Counter((norm_t(x['tib']), norm_e(x['eng'])) for x in nonnull)
    print(f"\n[{label}]")
    print(f"  occurrences           {occ}")
    print(f"  distinct Tibetan types {len(tib)}   (occ/type {occ/len(tib):.1f})")
    print(f"  distinct (tib,eng) raw {len(pairs)}   (occ/pair {occ/len(pairs):.1f})")
    print(f"  distinct (tib,norm eng) {len(npairs)}  (occ/pair {occ/len(npairs):.1f})")
    print(f"  non-null eng            {len(nonnull)} ({100*len(nonnull)/occ:.1f}%)  distinct norm pairs {len(npairs_nn)}")
    # senses per headword
    sp=collections.defaultdict(set)
    for x in nonnull: sp[norm_t(x['tib'])].add(norm_e(x['eng']))
    dist=collections.Counter(len(v) for v in sp.values())
    print('  senses/headword distribution (1,2,3,4,5+):', dist[1], dist[2], dist[3], dist[4], sum(v for k,v in dist.items() if k>=5))
    print('  headwords with >=2 distinct normalised renderings: %d/%d (%.1f%%)'%(sum(v for k,v in dist.items() if k>=2), len(sp), 100*sum(v for k,v in dist.items() if k>=2)/len(sp)))

# hapax pairs
S=W; nonnull=[x for x in S if x['eng']]
pc=collections.Counter((norm_t(x['tib']), norm_e(x['eng'])) for x in nonnull)
h=sum(1 for v in pc.values() if v==1)
print('\nhapax (tib,normEng) pairs: %d/%d types (%.1f%%), = %.1f%% of occurrences'%(h,len(pc),100*h/len(pc),100*h/len(nonnull)))
