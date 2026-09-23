import json, collections, sqlite3
R='/Users/adamderickandrade/ALL-translation-tool/'
L=json.load(open(R+'data/alignment/alignment_full_v1.json'))['links']
want=[('dang po','Stopping desire for this life is the subject of'),('gsungs','Our Lama went on to give us'),
      ('skye ba','you will then be able to reach'),("'ba' zhig",'all they can do in the end'),
      ('tshad','the point at which we can say one has succeeded')]
con=sqlite3.connect(R+'build/hgm_spine_v27_2.db')
for l in L:
    if l['d'] in (5,7) and (l.get('tib'),l.get('eng')) in want:
        c,s=l['course'],l['seg']
        r=con.execute("select wylie,english from corpus_segments where course=? and seq=?",(c,s)).fetchone()
        print(f"=== {c}:{s}  {l['tib']!r} -> {l['eng']!r}")
        print('  WYL:', (r[0] or '')[:200].replace('\n',' '))
        print('  ENG:', (r[1] or '')[:300].replace('\n',' '))
        print()
