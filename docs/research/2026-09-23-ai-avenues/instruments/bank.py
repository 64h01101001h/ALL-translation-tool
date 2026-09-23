import json, collections, re
R='/Users/adamderickandrade/ALL-translation-tool/'
d=json.load(open(R+'data/alignment/alignment_full_v1.json'))
L=d['links']
print('links',len(L))
by=collections.Counter(l['course'] for l in L)
print('by course',dict(sorted(by.items())))
segs=set((l['course'],l['seg']) for l in L)
print('distinct segments',len(segs))
dd=collections.Counter(l['d'] for l in L)
print('by depth',dict(sorted(dd.items())))
nulls=sum(1 for l in L if l.get('eng') is None)
print('null-eng links',nulls, round(100*nulls/len(L),1),'%')
pairs=set((l['tib'],l['eng']) for l in L if l.get('eng'))
print('distinct (tib,eng) pairs',len(pairs))
heads=set(l['tib'] for l in L)
print('distinct tib strings',len(heads))
# dictionary layer only
D5=[l for l in L if l['d'] in (5,7) and l.get('eng')]
print('d=5/7 with eng:',len(D5))
print('distinct d5/7 pairs:',len(set((l['tib'],l['eng']) for l in D5)))
print('distinct d5/7 tib:',len(set(l['tib'] for l in D5)))
