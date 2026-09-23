import json,sqlite3,collections,random
d=json.load(open('data/alignment/alignment_full_v1.json'))
L=d['links']
bank=set(); c05=collections.defaultdict(set)
for l in L:
    t=l.get('tib'); e=l.get('eng')
    if not (t and e): continue
    if l['course'] in ('C01','C02','C03','C04'): bank.add(t)
    elif l['course']=='C05': c05[l['seg']].add(t)
print('bank headwords', len(bank))
print('C05 landed segments', len(c05))
new={s:{t for t in ts if t not in bank} for s,ts in c05.items()}
allnew=set().union(*new.values())
print('C05 distinct NEW headwords', len(allnew))
# features from spine
con=sqlite3.connect('build/hgm_spine_v27_2.db')
rows={r[0]:(r[1] or '',r[2] or '') for r in con.execute("select seq,wylie,english from corpus_segments where course='C05'")}
# banked syllable set from bank headwords
banksyl=set()
for t in bank:
    for s in t.replace(',',' ').replace('/',' ').split():
        banksyl.add(s)
feat={}
for s in new:
    w,e=rows.get(s,('',''))
    syl=[x for x in w.replace('/',' ').replace(',',' ').split() if x]
    ds=set(syl)
    novel={x for x in ds if x not in banksyl}
    cost=len(w)+len(e)
    feat[s]=dict(cost=max(cost,1), dist=len(ds), novel=len(novel),
                 share=(len(novel)/len(ds) if ds else 0.0), length=cost)
json.dump({'new':{str(k):sorted(v) for k,v in new.items()},'feat':{str(k):v for k,v in feat.items()}},
          open('/private/tmp/claude-501/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2/scratchpad/h.json','w'))
