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
    e=s.strip().lower().strip('".,;:!?()'); e=re.sub(r'\s+',' ',e); p=None
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
order=[k for k in sorted(byseg,key=lambda k:(k[0],k[1])) if k in rowmap]
N=len(order); cut=int(N*0.6); known,cand=order[:cut],order[cut:]
K_t=set();K_p=set();K_syl=set();K_wrd=set();K_eng=set();K_we=set()
for k in known:
    for x in byseg[k]:
        t=nt(x['tib'])
        if t: K_t.add(t)
        if t and x['eng']: K_p.add((t,ne(x['eng'])))
    w,e=rowmap[k]; ss=syls(w); K_syl|=set(ss); K_wrd|=set(munch(ss)); K_eng|=set(etok(e))
    for a in set(munch(ss)):
        for b in set(etok(e)): K_we.add((a,b))
glossed=set()
for wy,g in db.execute("select wylie,hgm_gloss from entries where hgm_gloss is not null and hgm_gloss not in ('','[]')"):
    glossed.add(re.sub(r'^!+','',(wy or '').strip()))
feats={};truth={};cost={}
for k in cand:
    w,e=rowmap[k]; ss=syls(w); ws=munch(ss); es=etok(e)
    uW=set(x for x in ws if x not in K_wrd); uE=set(x for x in es if x not in K_eng); uS=set(s for s in ss if s not in K_syl)
    we_new=sum(1 for a in set(ws) for b in set(es) if (a,b) not in K_we)
    ung=sum(1 for x in set(ws) if x not in glossed)
    feats[k]=dict(nsyl=len(ss),unseen_syl=len(uS),unseen_syl_share=len(uS)/max(1,len(ss)),
        unseen_wrd=len(uW),unseen_wrd_share=len(uW)/max(1,len(ws)),unseen_eng=len(uE),
        we_new=we_new,we_new_rate=we_new/max(1,len(ss)+len(es)),unglossed=ung,
        unglossed_rate=ung/max(1,len(ss)),
        mix=(len(uW)+len(uE)+we_new/60.0)/max(1,len(ss)+len(es)))
    truth[k]=dict(tset={nt(x['tib']) for x in byseg[k] if x['tib']},
                  pset={(nt(x['tib']),ne(x['eng'])) for x in byseg[k] if x['tib'] and x['eng']})
    cost[k]=len(ss)+len(es)          # variable cost model (tokens read)
TOT={key:len(set().union(*[truth[k][key] for k in cand])-(K_t if key=='tset' else K_p)) for key in ('tset','pset')}
TOTCOST=sum(cost.values()); NSEG=len(cand)
print('candidates %d, total cost units %d, new tib %d new pairs %d'%(NSEG,TOTCOST,TOT['tset'],TOT['pset']))
FIX=0.0
def capture_at(rank,key,budget_frac,mode):
    seen=set(K_t if key=='tset' else K_p); base=len(seen); spent=0.0
    cap = budget_frac*(TOTCOST if mode=='var' else NSEG)
    for k in rank:
        c = cost[k] if mode=='var' else 1
        if spent+c>cap: break
        spent+=c; seen|=truth[k][key]
    return 100*(len(seen)-base)/TOT[key]
def greedy(key,mode):
    rem=set(cand); seen=set(K_t if key=='tset' else K_p); out=[]
    while rem:
        best=max(rem,key=lambda k: len(truth[k][key]-seen)/(cost[k] if mode=='var' else 1))
        out.append(best); seen|=truth[best][key]; rem.discard(best)
    return out
for mode,label in (('seg','BUDGET = number of segments (fixed cost per segment)'),
                   ('var','BUDGET = corpus tokens read (cost proportional to segment length)')):
    print("\n=== %s ==="%label)
    print("%-26s %6s %6s %6s  %6s %6s %6s"%("ranking","T@10","T@20","T@50","P@10","P@20","P@50"))
    def rep(nm,rk):
        print("%-26s %6.1f %6.1f %6.1f  %6.1f %6.1f %6.1f"%(nm,
            *[capture_at(rk,'tset',q,mode) for q in (.1,.2,.5)],
            *[capture_at(rk,'pset',q,mode) for q in (.1,.2,.5)]))
    random.seed(7); r=list(cand); random.shuffle(r); rep("random",r)
    rep("corpus order",list(cand))
    for f in ('unseen_syl_share','unseen_wrd','unseen_wrd_share','nsyl','we_new','we_new_rate','unglossed','unglossed_rate','mix'):
        rep(f, sorted(cand,key=lambda k:-feats[k][f]))
    rep("ORACLE greedy (T)", greedy('tset',mode))
    rep("ORACLE greedy (P)", greedy('pset',mode))
