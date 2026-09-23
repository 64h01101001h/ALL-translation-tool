import sqlite3, json, collections, re, sys
REPO='/Users/adamderickandrade/ALL-translation-tool/'
db=sqlite3.connect(REPO+'build/hgm_spine_v27_2.db')
COURSES=['C%02d'%i for i in range(1,19)]
rows=db.execute("select course,seq,wylie,english,acip from corpus_segments where course in (%s) order by course,seq"%','.join('?'*18), COURSES).fetchall()
print('C01-C18 segments:', len(rows))

bank=json.load(open(REPO+'data/alignment/alignment_full_v1.json'))
L=bank['links']
landed=set((x['course'],x['seg']) for x in L)
print('landed segments (distinct course,seg in bank):', len(landed))
print('landed by course:', collections.Counter(c for c,s in landed))

# scope: mandate less C13 less landed
scope=[r for r in rows if r[0]!='C13']
print('less C13:', len(scope))
rem=[r for r in scope if (r[0],r[1]) not in landed]
print('less landed:', len(rem))
# exact duplicate (wylie, english) against read text or each other
read_keys=set((r[2],r[3]) for r in rows if (r[0],r[1]) in landed)
seen=set(read_keys)
uniq=[]
dup=0
for r in rem:
    k=(r[2],r[3])
    if k in seen: dup+=1; continue
    seen.add(k); uniq.append(r)
print('exact (wylie,english) dupes removed:', dup, '-> unique remaining:', len(uniq))
print('dupe share by course:', {c: round(100*sum(1 for r in rem if r[0]==c and (r[2],r[3]) not in [ ] )/1,1) for c in []})
byc=collections.Counter(r[0] for r in rem); byu=collections.Counter(r[0] for r in uniq)
for c in COURSES:
    if byc[c]: print(c, 'remaining', byc[c], 'unique', byu[c], 'dup%%', round(100*(1-byu[c]/byc[c]),1))
json.dump({'scope':[list(r) for r in uniq]}, open('/private/tmp/claude-501/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2/scratchpad/unique_remaining.json','w'))
