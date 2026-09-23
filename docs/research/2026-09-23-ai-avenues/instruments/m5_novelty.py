import json, collections, re, sqlite3, pickle, random, math
REPO='/Users/adamderickandrade/ALL-translation-tool/'
db=sqlite3.connect(REPO+'build/hgm_spine_v27_2.db')
attested=pickle.load(open('/private/tmp/claude-501/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2/scratchpad/lex2.pkl','rb'))['attested']
bank=json.load(open(REPO+'data/alignment/alignment_full_v1.json'))
L=[x for x in bank['links'] if x['seg'] is not None and x['d'] in (5,6,7)]
ART=re.compile(r'^(the|a|an|of|to|in|for|that|this|his|her|their|its|our|your|my|and|is|are|be|been|will|would)\s+')
def nt(s): return (s or '').strip().strip(',.;:!?').lower()
def ne(s):
    if s is None: return None
    e=s.strip().lower().strip('".,;:!?()'); e=re.sub(r'\s+',' ',e)
    p=None
    while p!=e: p=e; e=ART.sub('',e)
    e=re.sub(r"(\w)'s$",r'\1',e)
    if len(e)>3 and e.endswith('s') and not e.endswith('ss'): e=e[:-1]
    return e
def syls(w):
    o=[]
    for t in re.split(r"\s+", w or ''):
        t=t.strip(",./;:()[]!?\"*-_0123456789")
        if t: o.append(t)
    return o
def munch(ss):
    i=0;o=[]
    while i<len(ss):
        for l in (2,1):
            if i+l<=len(ss) and (l==1 or tuple(ss[i:i+l]) in attested):
                o.append(' '.join(ss[i:i+l])); i+=l; break
    return o
def etok(e): return re.findall(r"[a-z']+",(e or '').lower())

rowmap={(c,s):(w,e) for c,s,w,e in db.execute("select course,seq,wylie,english from corpus_segments")}
byseg=collections.defaultdict(list)
for x in L: byseg[(x['course'],x['seg'])].append(x)
order=sorted(byseg,key=lambda k:(k[0],k[1]))
order=[k for k in order if k in rowmap]
N=len(order); cut=int(N*0.6)
known,cand=order[:cut],order[cut:]
print(f"known={len(known)} candidates={len(cand)}")

K_t=set(); K_p=set(); K_syl=set(); K_wrd=set(); K_eng=set(); K_we=set()
for k in known:
    for x in byseg[k]:
        t=nt(x['tib'])
        if t: K_t.add(t)
        if t and x['eng']: K_p.add((t,ne(x['eng'])))
    w,e=rowmap[k]
    ss=syls(w); K_syl.update(ss); K_wrd.update(munch(ss)); K_eng.update(etok(e))
    for a in set(munch(ss)):
        for b in set(etok(e)): K_we.add((a,b))
print('known: tib types %d pairs %d syl %d wrd %d eng %d we %d'%(len(K_t),len(K_p),len(K_syl),len(K_wrd),len(K_eng),len(K_we)))

# dictionary gloss coverage (free signal)
glossed=set()
for wy,g in db.execute("select wylie,hgm_gloss from entries where hgm_gloss is not null and hgm_gloss not in ('','[]')"):
    glossed.add(re.sub(r'^!+','',(wy or '').strip()))

feats={}; truth={}
for k in cand:
    w,e=rowmap[k]; ss=syls(w); ws=munch(ss); es=etok(e)
    uS=[s for s in ss if s not in K_syl]; uW=[x for x in ws if x not in K_wrd]; uE=[x for x in es if x not in K_eng]
    we_new=sum(1 for a in set(ws) for b in set(es) if (a,b) not in K_we)
    unglossed=sum(1 for x in set(ws) if x not in glossed)
    feats[k]=dict(nsyl=len(ss), unseen_syl=len(set(uS)), unseen_syl_share=len(uS)/max(1,len(ss)),
                  unseen_wrd=len(set(uW)), unseen_wrd_share=len(uW)/max(1,len(ws)),
                  unseen_eng=len(set(uE)), unseen_eng_share=len(uE)/max(1,len(es)),
                  we_new=we_new, we_new_share=we_new/max(1,len(set(ws))*len(set(es))),
                  unglossed=unglossed, neng=len(es))
    tset={nt(x['tib']) for x in byseg[k] if x['tib']}
    pset={(nt(x['tib']),ne(x['eng'])) for x in byseg[k] if x['tib'] and x['eng']}
    truth[k]=dict(new_t=len(tset-K_t), new_p=len(pset-K_p), tset=tset, pset=pset)

def pearson(a,b):
    n=len(a); ma=sum(a)/n; mb=sum(b)/n
    sa=math.sqrt(sum((x-ma)**2 for x in a)); sb=math.sqrt(sum((x-mb)**2 for x in b))
    if sa==0 or sb==0: return 0.0
    return sum((x-ma)*(y-mb) for x,y in zip(a,b))/(sa*sb)
def spearman(a,b):
    def rk(v):
        s=sorted(range(len(v)),key=lambda i:v[i]); r=[0]*len(v)
        for j,i in enumerate(s): r[i]=j
        return r
    return pearson(rk(a),rk(b))

FN=list(feats[cand[0]].keys())
print("\n=== correlation of FREE features with true novelty (candidates n=%d) ==="%len(cand))
print(f"{'feature':20} {'r(new_t)':>9} {'rho(new_t)':>11} {'r(new_p)':>9} {'rho(new_p)':>11}")
for f in FN:
    v=[feats[k][f] for k in cand]
    for tgt in ('new_t','new_p'): pass
    a=[truth[k]['new_t'] for k in cand]; b=[truth[k]['new_p'] for k in cand]
    print(f"{f:20} {pearson(v,a):9.3f} {spearman(v,a):11.3f} {pearson(v,b):9.3f} {spearman(v,b):11.3f}")

def capture(rank, key):
    """greedy-sequential cumulative distinct new items following a fixed ranking"""
    seen=set(K_t if key=='tset' else K_p); base=len(seen); out=[]
    for k in rank:
        seen |= truth[k][key]; out.append(len(seen)-base)
    return out
TOTAL={key: len(set().union(*[truth[k][key] for k in cand]) - (K_t if key=='tset' else K_p)) for key in ('tset','pset')}
print("\ntotal NEW items available in candidate pool: tib types %d, pairs %d"%(TOTAL['tset'],TOTAL['pset']))

def report(name, rank):
    line=[name]
    for key in ('tset','pset'):
        c=capture(rank,key)
        for q in (0.1,0.2,0.5):
            i=int(len(rank)*q)-1
            line.append(f"{100*c[i]/TOTAL[key]:.1f}")
    print(("%-28s "%name)+"  ".join(f"{x:>5}" for x in line[1:]))

def greedy(key):
    rem=list(cand); seen=set(K_t if key=='tset' else K_p); out=[]
    while rem:
        best=max(rem,key=lambda k: len(truth[k][key]-seen))
        out.append(best); seen|=truth[best][key]; rem.remove(best)
    return out

print("\n=== capture of new items at top-k%% of the candidate pool ===")
print("%-28s %5s %5s %5s  %5s %5s %5s"%("ranking","T@10","T@20","T@50","P@10","P@20","P@50"))
random.seed(7)
r=list(cand); random.shuffle(r); report("random",r)
report("corpus order (status quo)", list(cand))
for f in ('unseen_syl_share','unseen_syl','unseen_wrd','unseen_wrd_share','unseen_eng','we_new','unglossed','nsyl'):
    report("feature: "+f, sorted(cand,key=lambda k:-feats[k][f]))
# combined
report("combo unseen_wrd+we_new", sorted(cand,key=lambda k:-(feats[k]['unseen_wrd']*3+feats[k]['we_new']/50)))
report("ORACLE rank-by-truth (T)", sorted(cand,key=lambda k:-truth[k]['new_t']))
report("ORACLE rank-by-truth (P)", sorted(cand,key=lambda k:-truth[k]['new_p']))
report("ORACLE greedy cover (T)", greedy('tset'))
report("ORACLE greedy cover (P)", greedy('pset'))
pickle.dump({'feats':feats,'truth':{k:{'new_t':v['new_t'],'new_p':v['new_p']} for k,v in truth.items()},'cand':cand},open('/private/tmp/claude-501/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2/scratchpad/nov.pkl','wb'))
