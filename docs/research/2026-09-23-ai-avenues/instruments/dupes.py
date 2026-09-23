import json,sqlite3,collections,re,statistics
R='/Users/adamderickandrade/ALL-translation-tool/'
con=sqlite3.connect(R+'build/hgm_spine_v27_2.db');cur=con.cursor()
cur.execute("select course,seq,wylie,english from corpus_segments")
rows=[(c,s,w or '',e or '') for c,s,w,e in cur.fetchall()]
c18=[r for r in rows if re.fullmatch(r'C(0[1-9]|1[0-8])',r[0])]
full=json.load(open(R+'data/alignment/alignment_full_v1.json'))
L=full['links']
landed=collections.defaultdict(set)
for x in L:
    if x['seg'] is None: continue
    landed[(x['course'],x['seg'])].add((x['tib'],x['eng'],x['d']))
print('landed segments', len(landed))
# duplicate groups over C01-C18 by exact (wylie,english)
g=collections.defaultdict(list)
for c,s,w,e in c18:
    if w.strip() or e.strip(): g[(w,e)].append((c,s))
dups={k:v for k,v in g.items() if len(v)>1}
print('duplicate text groups in C01-C18:', len(dups), 'segments involved:', sum(len(v) for v in dups.values()))
# groups where >=2 members are landed -> free self-consistency test
both=[(k,[m for m in v if m in landed]) for k,v in dups.items()]
both=[(k,v) for k,v in both if len(v)>=2]
print('duplicate groups with >=2 LANDED members (free consistency pairs):', len(both))
js=[]
for k,v in both:
    a,b=v[0],v[1]
    A,B=landed[a],landed[b]
    j=len(A&B)/len(A|B) if (A|B) else 1.0
    js.append((j,a,b,len(A),len(B),len(A&B)))
if js:
    js.sort()
    vals=[x[0] for x in js]
    print('Jaccard of the two independently produced span sets on IDENTICAL text:')
    print('  n=%d mean %.3f median %.3f min %.3f max %.3f'%(len(vals),statistics.mean(vals),statistics.median(vals),min(vals),max(vals)))
    print('  worst 5:', [(round(x[0],3),x[1],x[2],x[3],x[4],x[5]) for x in js[:5]])
    print('  exact-identical pairs: %d/%d'%(sum(1 for v in vals if v==1.0),len(vals)))
# how many landed segments have a duplicate anywhere in C01-C18 (unlanded twin = free future check)
twin=sum(1 for k,v in dups.items() for m in v if m in landed)
print('landed segments that have an exact twin somewhere in C01-C18:', twin)
