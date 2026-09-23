import json, collections, statistics
R='/Users/adamderickandrade/ALL-translation-tool/'
L=json.load(open(R+'data/alignment/alignment_full_v1.json'))['links']
d57=[l for l in L if l['d'] in (5,7) and l.get('eng')]
bytib=collections.defaultdict(collections.Counter)
for l in d57: bytib[l['tib']][l['eng']]+=1
n=len(bytib)
multi={t:c for t,c in bytib.items() if len(c)>1}
print(f'd=5/7 Tibetan headwords in the bank: {n}')
print(f'  with MORE THAN ONE distinct English: {len(multi)} ({100*len(multi)/n:.1f}%)')
dist=collections.Counter(len(c) for c in bytib.values())
print('  distinct-English count histogram:', dict(sorted(dist.items())[:10]), '... max', max(dist))
# the sharp class: a rendering attested ONCE against a dominant one (>=5) -> singleton dissent
sing=[]
for t,c in multi.items():
    tot=sum(c.values()); top,topn=c.most_common(1)[0]
    for e,k in c.items():
        if e==top: continue
        if k==1 and topn>=5: sing.append((t,e,top,topn,tot))
print(f'\nSINGLETON DISSENTS (a rendering attested once where another is attested >=5 times): {len(sing)}')
print(f'  distinct Tibetan headwords involved: {len(set(s[0] for s in sing))}')
for t,e,top,topn,tot in sorted(sing,key=lambda x:-x[3])[:12]:
    print(f'   {t!r:28} -> {e!r:34} (1x)   vs {top!r} ({topn}x of {tot})')
