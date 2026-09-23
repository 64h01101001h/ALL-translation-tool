import json,sqlite3,collections
R='/Users/adamderickandrade/ALL-translation-tool/'
full=json.load(open(R+'data/alignment/alignment_full_v1.json'))
L=full['links']
con=sqlite3.connect(R+'build/hgm_spine_v27_2.db');cur=con.cursor()
cur.execute("select course,seq,wylie,english from corpus_segments")
W={};E={}
for c,s,w,e in cur.fetchall(): W[(c,s)]=w or ''; E[(c,s)]=e or ''
bys=collections.defaultdict(list)
for x in L:
    if x['d']==5: bys[(x['course'],x['seg'])].append(x)
tib_mono=0; tib_bad=0; scored=0; crossing=0; notfound=0
for k,v in bys.items():
    w=W.get(k,''); e=E.get(k,'')
    if not w or len(v)<2: continue
    # validate tibetan monotone under forward cursor
    ci=0; ok=True
    for x in v:
        i=w.find(x['tib'],ci)
        if i<0: ok=False;break
        ci=i+len(x['tib'])
    if ok: tib_mono+=1
    else: tib_bad+=1; continue
    vg=[x for x in v if x['eng']]
    if len(vg)<2 or not e: continue
    scored+=1
    ci=0; cross=False; nf=False
    for x in vg:
        i=e.find(x['eng'],ci)
        if i<0:
            if e.find(x['eng'])>=0: cross=True
            else: nf=True
            break
        ci=i+len(x['eng'])
    if cross: crossing+=1
    if nf: notfound+=1
print('segments with >=2 d5:',tib_mono+tib_bad,'tibetan-monotone:',tib_mono,'not:',tib_bad)
print('english scored:',scored,'non-monotone (needs eng_order):',crossing,'share',round(crossing/scored,4),'eng-not-found:',notfound)
