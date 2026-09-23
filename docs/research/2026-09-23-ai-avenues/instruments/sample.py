import json, collections, random, sqlite3
R='/Users/adamderickandrade/ALL-translation-tool/'
L=json.load(open(R+'data/alignment/alignment_full_v1.json'))['links']
d57=[l for l in L if l['d'] in (5,7) and l.get('eng')]
bytib=collections.defaultdict(collections.Counter)
loc=collections.defaultdict(list)
for l in d57:
    bytib[l['tib']][l['eng']]+=1
    loc[(l['tib'],l['eng'])].append((l['course'],l['seg']))
cand=[]
for t,c in bytib.items():
    top,topn=c.most_common(1)[0]
    for e,k in c.items():
        if e!=top and k==1 and topn>=5: cand.append((t,e,top,topn))
random.seed(20260922)
con=sqlite3.connect(R+'build/hgm_spine_v27_2.db')
for t,e,top,topn in random.sample(cand,10):
    c,s=loc[(t,e)][0]
    row=con.execute("select wylie,english from corpus_segments where course=? and seq=?",(c,s)).fetchone()
    print(f'=== {c}:{s}   {t!r} -> {e!r}   (dominant {top!r} x{topn})')
    print('   WYL:', (row[0] or '')[:180].replace('\n',' '))
    print('   ENG:', (row[1] or '')[:220].replace('\n',' '))
