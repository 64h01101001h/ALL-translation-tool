import json, collections
R='/Users/adamderickandrade/ALL-translation-tool/'
L=json.load(open(R+'data/alignment/alignment_full_v1.json'))['links']
d5=[l for l in L if l['d']==5 and l['eng'] is not None and l['seg'] is not None]
print('d5 non-null links',len(d5))
hw=collections.Counter(l['tib'].strip() for l in d5)
pair=collections.Counter((l['tib'].strip(),l['eng'].strip()) for l in d5)
print('headwords',len(hw),'distinct pairs',len(pair))
tot=sum(hw.values()); acc=0
for i,(h,n) in enumerate(hw.most_common()):
    acc+=n
    if i+1 in (50,100,200,500,1000,2000,5000): print(f'top {i+1} headwords cover {acc/tot:.1%} of d5 links')
sing=sum(1 for h,n in hw.items() if n==1); print('hapax headwords',sing, f'{sing/len(hw):.1%}')
ps=sum(1 for p,n in pair.items() if n==1); print('pairs attested once',ps,f'{ps/len(pair):.1%}', 'links in once-pairs', ps, f'{ps/tot:.1%} of links')
# review load if each distinct pair reviewed once + cap per pair
for cap in (1,3,5):
    print('cap',cap,'judgments', sum(min(n,cap) for n in pair.values()))
multi=[h for h in hw if len({e for (t,e) in pair if t==h})>1]
top=hw.most_common()
for N in (100,500,1000):
    for cap in (10,30):
        print(f'top{N} cap{cap}: judgments', sum(min(n,cap) for h,n in top[:N]), 'covering links', sum(n for h,n in top[:N]))
print('headwords with >=30 d5 citations', sum(1 for h,n in top if n>=30), ' >=10', sum(1 for h,n in top if n>=10))
# multi-rendering headwords
rend=collections.defaultdict(set)
for (t,e) in pair: rend[t].add(e)
print('headwords with >=2 d5 renderings', sum(1 for t in rend if len(rend[t])>=2))
print('links per landed segment', len(d5)/1748, 'all links per seg', len(L)/1748)
