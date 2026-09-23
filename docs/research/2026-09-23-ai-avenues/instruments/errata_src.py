import gzip,json,re,os,collections,difflib
segs=json.load(gzip.open('/Users/adamderickandrade/ALL-translation-tool/data/full_parallel_corpus_v32.json.gz'))
bycourse=collections.defaultdict(list)
for i,s in enumerate(segs): bycourse[s['course']].append(i)
er=json.load(open('/Users/adamderickandrade/ALL-translation-tool/docs/errata_register.json'))
sm=json.load(open('src_match.json'))
def syl(s):
    s=s.upper(); s=re.sub(r'\[[^\]]*\]','',s); s=re.sub(r'@\S*','',s)
    s=re.sub(r"[^A-Z'%\-\+]", ' ', s); return [t for t in s.split() if t]
# index files by tag
root='/Users/adamderickandrade/ALL-translation-tool/library'
tagpath={}
for coll in ['sungbum','kangyur','tengyur']:
    for dp,dn,fn in os.walk(os.path.join(root,coll)):
        for f in fn:
            if f.upper().endswith('.TXT') and 'META' not in f.upper():
                tagpath.setdefault(coll+'/'+f[:8],[]).append(os.path.join(dp,f))
res=collections.Counter(); rows=[]
for e in er:
    if e['kind']!='TIBETAN_SPELLING': continue
    c,n=e['segment'].split(':'); n=int(n)
    try: gi=bycourse[c][n-1]
    except: res['badref']+=1; continue
    r=sm.get(str(gi))
    if not r or not r['best'] or r['best'][1]/r['nsh']<0.3:
        res['no source']+=1; rows.append((e['item_id'],e['segment'],e['confidence'],'NO SOURCE','')); continue
    t=syl(segs[gi]['acip'])
    best=None
    for p in tagpath[r['best'][0]]:
        ft=syl(open(p,encoding='latin-1').read())
        # locate by first shingle hit
        K=6; idx={}
        for j in range(len(ft)-K+1): idx.setdefault(tuple(ft[j:j+K]),j)
        pos=[idx[tuple(t[j:j+K])]-j for j in range(len(t)-K+1) if tuple(t[j:j+K]) in idx]
        if pos:
            st=collections.Counter(pos).most_common(1)[0][0]
            best=ft[max(0,st-5):st+len(t)+5]; break
    if best is None: res['locate fail']+=1; continue
    sm_=difflib.SequenceMatcher(a=t,b=best,autojunk=False)
    diffs=[]
    for op,a1,a2,b1,b2 in sm_.get_opcodes():
        if op!='equal' and a2>a1:  # ignore trailing context insertions
            diffs.append((' '.join(t[a1:a2]),' '.join(best[b1:b2])))
    rows.append((e['item_id'],e['segment'],e['confidence'],r['best'][0],diffs[:6]))
    res['diffs' if diffs else 'identical to source']+=1
print(res)
for r in rows: print(r)
