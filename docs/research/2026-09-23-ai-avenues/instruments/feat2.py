import json,re,sqlite3,collections,math
R='/Users/adamderickandrade/ALL-translation-tool/'
V=json.load(open(R+'data/alignment/audit_verdicts_20260904.json'))
items=V['items']
print('final values', collections.Counter(x['final'] for x in items))
print('stands values', collections.Counter(str(x.get('stands')) for x in items))
freq=json.load(open('/private/tmp/claude-501/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2/scratchpad/freq2.json'))
ev=json.load(open(R+'data/alignment/alignment_evidence_v1.json'))
con=sqlite3.connect(R+'build/hgm_spine_v27_2.db');cur=con.cursor()
cur.execute("select wylie,hgm_gloss from entries where hgm_gloss is not null and hgm_gloss not in ('[]','null')")
g=collections.defaultdict(set)
for w,gg in cur.fetchall():
    try: arr=json.loads(gg)
    except Exception: arr=[]
    for a in arr:
        if isinstance(a,str): g[w].add(a.strip().lower())
FN=re.compile(r'^(the|a|an|and|or|his|our|your|I|you)\b', re.I)
def pos(it):
    f=it['final']
    return 1 if ('DEFECT' in f or 'CONFIRM' in f) and 'REFUT' not in f else 0
lab=[(it, pos(it)) for it in items]
print('positives', sum(p for _,p in lab))
feats={}
def add(name, fn):
    tp=fp=fn_=tn=0
    for it,p in lab:
        f=1 if fn(it) else 0
        if f and p: tp+=1
        elif f and not p: fp+=1
        elif (not f) and p: fn_+=1
        else: tn+=1
    prec=tp/(tp+fp) if tp+fp else float('nan')
    rec=tp/(tp+fn_) if tp+fn_ else float('nan')
    feats[name]=(tp,fp,fn_,tn,prec,rec)
add('eng begins with supplied function word', lambda it: bool(FN.match(it['eng'])))
add('eng word count >= 3', lambda it: len(it['eng'].split())>=3)
add('eng words / tib syllables >= 2', lambda it: len(it['eng'].split())/max(1,len(it['headword'].split()))>=2)
add('headword corpus freq <= 2 (rare)', lambda it: freq.get(it['headword'],99)<=2)
add('headword corpus freq >= 20 (common)', lambda it: freq.get(it['headword'],0)>=20)
add('headword has >= 3 renderings in layer', lambda it: len(ev['pairs'].get(it['headword'],[]))>=3)
add('eng NOT among hgm_gloss for headword', lambda it: it['headword'] in g and it['eng'].strip().lower() not in g[it['headword']])
add('headword absent from dictionary', lambda it: it['headword'] not in g)
add('headword is single syllable', lambda it: len(it['headword'].split())==1)
print('%-45s %4s %4s %4s %4s %6s %6s'%('feature','TP','FP','FN','TN','prec','rec'))
for k,(tp,fp,fn_,tn,pr,rc) in feats.items():
    print('%-45s %4d %4d %4d %4d %6.2f %6.2f'%(k,tp,fp,fn_,tn,pr,rc))
