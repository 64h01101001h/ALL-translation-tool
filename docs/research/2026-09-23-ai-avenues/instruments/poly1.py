import json, collections, statistics
B='/Users/adamderickandrade/ALL-translation-tool/data/alignment/alignment_full_v1.json'
d=json.load(open(B))
L=d['links']
print('total links', len(L))
# depth distribution
dd=collections.Counter(l['d'] for l in L)
print('depth dist', sorted(dd.items()))
# non-null english
nn=[l for l in L if l.get('eng') is not None and l.get('tib') is not None]
print('links with non-null eng and tib', len(nn))
# restrict to lexical depths 5,6,7 (word/particle/morpheme) vs all
for depths in [(5,),(5,7),(5,6,7),(1,2,3,4,5,6,7)]:
    sub=[l for l in nn if l['d'] in depths]
    hw=collections.defaultdict(set)
    for l in sub: hw[l['tib']].add(l['eng'])
    multi=sum(1 for k,v in hw.items() if len(v)>1)
    print('depths',depths,'links',len(sub),'headwords',len(hw),'multi-rendering',multi, '%.1f%%'%(100*multi/len(hw)))
