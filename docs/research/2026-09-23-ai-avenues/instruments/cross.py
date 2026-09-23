import json,re,sqlite3,collections
R='/Users/adamderickandrade/ALL-translation-tool/'
full=json.load(open(R+'data/alignment/alignment_full_v1.json'))
L=full['links']
con=sqlite3.connect(R+'build/hgm_spine_v27_2.db');cur=con.cursor()
cur.execute("select course,seq,english from corpus_segments")
eng={(c,s):(e or '') for c,s,e in cur.fetchall()}
bys=collections.defaultdict(list)
for x in L:
    if x['d'] in (5,6) and x['eng']: bys[(x['course'],x['seg'])].append(x)
scored=0; crossing=0; ex=[]
for k,v in bys.items():
    E=eng.get(k,'')
    if not E or len(v)<2: continue
    scored+=1
    cur_i=0; cross=False
    for x in v:
        i=E.find(x['eng'], cur_i)
        if i<0:
            # exists earlier? -> English order differs from Tibetan order
            if E.find(x['eng'])>=0: cross=True; break
            else: cross=None; break   # not found at all (shouldn't happen post-gate)
        cur_i=i+len(x['eng'])
    if cross is True:
        crossing+=1
        if len(ex)<6: ex.append((k,len(v)))
print('segments scored', scored, 'segments needing eng_order (non-monotone):', crossing,
      'share', round(crossing/scored,4))
print('examples', ex)
# how many spans per segment on average
print('mean d5/d6 glossed spans per segment', round(sum(len(v) for v in bys.values())/len(bys),1))
