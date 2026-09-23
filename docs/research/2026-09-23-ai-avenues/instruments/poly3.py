import json, collections
B='/Users/adamderickandrade/ALL-translation-tool/data/alignment/alignment_full_v1.json'
d=json.load(open(B)); L=d['links']
sub=[l for l in L if l['d']==5 and l.get('eng') and l.get('tib')]
rend=collections.defaultdict(collections.Counter)
for l in sub: rend[l['tib']][l['eng']]+=1
byK=sorted(rend.items(), key=lambda x:-len(x[1]))
for t,c in byK[:8]:
    print('=== %r  K=%d  occ=%d'%(t,len(c),sum(c.values())))
    print('   ', ' | '.join('%s(%d)'%(e,n) for e,n in c.most_common()))
    print()
