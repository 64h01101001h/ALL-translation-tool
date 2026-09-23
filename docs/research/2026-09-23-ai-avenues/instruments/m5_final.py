import json, collections, re, sqlite3, pickle, random, math, statistics
REPO='/Users/adamderickandrade-ALL'  # placeholder
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
    for t in re.split(r"\s+",w or ''):
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
rowmap={(c,s):((w or ''),(e or '')) for c,s,w,e in db.execute("select course,seq,wylie,english from corpus_segments")}
byseg=collections.defaultdict(list)
for x in L: byseg[(x['course'],x['seg'])].append(x)
order=[k for k in sorted(byseg,key=lambda k:(k[0],k[1])) if k in rowmap]

# MEASURED cost model from 168 C05 segments: billable = 177210 + 575.2*length
def COST(k):
    w,e=rowmap[k]; return 177210.0 + 575.2*(len(w.split())+len(e.split()))

glossed=set()
for wy,g in db.execute("select wylie,hgm_gloss from entries where hgm_gloss is not null and hgm_gloss not in ('','[]')"):
    glossed.add(re.sub(r'^!+','',(wy or '').strip()))

def run(cut_frac, seed_label):
    N=len(order); cut=int(N*cut_frac); known,cand=order[:cut],order[cut:]
    K_t=set();K_p=set();K_syl=set();K_wrd=set();K_eng=set();K_we=set()
    for k in known:
        for x in byseg[k]:
            t=nt(x['tib'])
            if t: K_t.add(t)
            if t and x['eng']: K_p.add((t,ne(x['eng'])))
        w,e=rowmap[k]; ss=syls(w); ws=set(munch(ss)); es=set(etok(e))
        K_syl|=set(ss); K_wrd|=ws; K_eng|=es
        for a in ws:
            for b in es: K_we.add((a,b))
    feats={};truth={};cost={}
    for k in cand:
        w,e=rowmap[k]; ss=syls(w); ws=munch(ss); es=etok(e)
        sw=set(ws); se=set(es)
        uW=set(x for x in sw if x not in K_wrd); uE=set(x for x in se if x not in K_eng); uS=set(s for s in ss if s not in K_syl)
        we_new=sum(1 for a in sw for b in se if (a,b) not in K_we)
        ung=sum(1 for x in sw if x not in glossed)
        c=COST(k)
        feats[k]=dict(unseen_syl_share=len(uS)/max(1,len(ss)),
            unseen_wrd=len(uW), unseen_eng=len(uE), we_new=we_new, unglossed=ung,
            nsyl=len(ss),
            # per-COST rate versions (the efficiency-correct form)
            r_unseen_wrd=len(uW)/c*1e6, r_unseen_eng=len(uE)/c*1e6, r_we_new=we_new/c*1e6,
            r_unglossed=ung/c*1e6,
            r_mix=(2.0*len(uW)+1.0*len(uE)+we_new/40.0+0.5*ung)/c*1e6)
        truth[k]=dict(tset={nt(x['tib']) for x in byseg[k] if x['tib']},
                      pset={(nt(x['tib']),ne(x['eng'])) for x in byseg[k] if x['tib'] and x['eng']})
        cost[k]=c
    TOT={key:len(set().union(*[truth[k][key] for k in cand])-(K_t if key=='tset' else K_p)) for key in ('tset','pset')}
    TC=sum(cost.values())
    def cap(rank,key,bf):
        seen=set(K_t if key=='tset' else K_p); base=len(seen); spent=0; capb=bf*TC
        for k in rank:
            if spent+cost[k]>capb: break
            spent+=cost[k]; seen|=truth[k][key]
        return 100*(len(seen)-base)/TOT[key]
    def greedy(key):
        rem=set(cand); seen=set(K_t if key=='tset' else K_p); out=[]
        while rem:
            best=max(rem,key=lambda k: len(truth[k][key]-seen)/cost[k])
            out.append(best); seen|=truth[best][key]; rem.discard(best)
        return out
    print(f"\n#### split at {int(cut_frac*100)}%  known={len(known)} cand={len(cand)}  new tib={TOT['tset']} new pairs={TOT['pset']}  budget=MEASURED cost (70% fixed)")
    print("%-24s %6s %6s %6s  %6s %6s %6s"%("ranking","T@10","T@20","T@50","P@10","P@20","P@50"))
    def rep(nm,rk):
        print("%-24s %6.1f %6.1f %6.1f  %6.1f %6.1f %6.1f"%(nm,*[cap(rk,'tset',q) for q in (.1,.2,.5)],*[cap(rk,'pset',q) for q in (.1,.2,.5)]))
    random.seed(11); r=list(cand); random.shuffle(r); rep("random",r)
    rep("corpus order",list(cand))
    for f in ('unseen_syl_share','unseen_wrd','we_new','unglossed','nsyl','r_unseen_wrd','r_we_new','r_unglossed','r_mix'):
        rep(f, sorted(cand,key=lambda k:-feats[k][f]))
    rep("ORACLE greedy/cost (T)", greedy('tset'))
    rep("ORACLE greedy/cost (P)", greedy('pset'))
    return feats,truth,cost,TOT,K_t,K_p,cand
run(0.6,'a')
run(0.4,'b')
run(0.8,'c')
