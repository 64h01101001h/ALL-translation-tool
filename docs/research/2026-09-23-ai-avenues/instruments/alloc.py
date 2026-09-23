import json,sqlite3,collections,re,math,statistics,random
R='/Users/adamderickandrade/ALL-translation-tool/'
full=json.load(open(R+'data/alignment/alignment_full_v1.json')); L=full['links']
freq=json.load(open('/private/tmp/claude-501/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2/scratchpad/freq2.json'))
ev=json.load(open(R+'data/alignment/alignment_evidence_v1.json'))
rend={h:len(v) for h,v in ev['pairs'].items()}
con=sqlite3.connect(R+'build/hgm_spine_v27_2.db');cur=con.cursor()
cur.execute("select wylie,hgm_gloss from entries where hgm_gloss is not null and hgm_gloss not in ('[]','null')")
g=collections.defaultdict(set)
for w,gg in cur.fetchall():
    try: arr=json.loads(gg)
    except Exception: arr=[]
    for a in arr:
        if isinstance(a,str): g[w].add(a.strip().lower())
FN=re.compile(r'^(the|a|an|and|or|his|our|your|I|you)\b', re.I)
def strata(x):
    s=[]
    d=x['d']; tib=x['tib'] or ''; eng=x['eng']
    ns=len(tib.split())
    if d==6: s.append('particle (d=6)')
    if d==7: s.append('compound member (d=7)')
    if eng is None: s.append('omission / no exponent (eng=null)')
    if d==5 and eng:
        if ns>=2: s.append('compound or multi-syllable word (d=5)')
        else: s.append('single-syllable word (d=5)')
        w=len(eng.split())
        if w/max(1,ns)>=2: s.append('expansion (>=2 English words per syllable)')
        if w/max(1,ns)<=0.34: s.append('compression (<=0.34)')
        if FN.match(eng): s.append('function-word-initial span (gate-flagged)')
        f=freq.get(tib)
        if f is not None:
            if f<=2: s.append('rare headword (corpus freq <=2)')
            elif f>=20: s.append('common headword (freq >=20)')
        if rend.get(tib,0)>=3: s.append('polysemous headword (>=3 renderings)')
        if tib in g:
            if eng.strip().lower() in g[tib]: s.append('literal: English is an attested hgm_gloss')
            else: s.append('novel English for a glossed headword')
        else: s.append('headword absent from the 105k dictionary')
    if d in (1,2,3,4): s.append('clause/phrase span (d=1-4)')
    return s
per=collections.defaultdict(lambda: collections.Counter())
for x in L:
    if x['seg'] is None: continue
    for s in strata(x): per[(x['course'],x['seg'])][s]+=1
segs=list(per)
allst=collections.Counter()
for k in segs: allst.update(per[k])
print('landed segments:',len(segs))
print('%-52s %8s %9s %10s'%('stratum','spans','per seg','in 100 segs'))
for s,c in allst.most_common():
    print('%-52s %8d %9.2f %10.0f'%(s,c,c/len(segs),100*c/len(segs)))
# coverage: how many of 100 random segments contain >=1 span of each stratum
random.seed(20260922)
cov=collections.Counter()
for _ in range(200):
    samp=random.sample(segs,100)
    seen=collections.Counter()
    for k in samp:
        for s in per[k]: seen[s]+=1
    for s in allst: 
        if seen[s]>0: cov[s]+=1
print()
print('share of 200 simulated 100-segment draws containing >=1 span of the stratum: all 1.00' if all(cov[s]==200 for s in allst) else {s:cov[s]/200 for s in allst})
