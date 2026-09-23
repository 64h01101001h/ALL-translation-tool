import json, collections, re, sqlite3, pickle, random, math
REPO='/Users/adamderickandrade/ALL-translation-tool/'
db=sqlite3.connect(REPO+'build/hgm_spine_v27_2.db')
attested=pickle.load(open('/private/tmp/claude-501/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2/scratchpad/lex2.pkl','rb'))['attested']
bank=json.load(open(REPO+'data/alignment/alignment_full_v1.json'))
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
gl=collections.defaultdict(set)
for wy,g in db.execute("select wylie,hgm_gloss from entries where hgm_gloss is not null"):
    w=re.sub(r'^!+','',(wy or '').strip()).lower()
    try: arr=json.loads(g)
    except: arr=[]
    for a in arr or []:
        if isinstance(a,str): gl[w].add(ne(a))
glossed_hw=set(gl)
L=[x for x in bank['links'] if x['seg'] is not None and x['d'] in (5,7) and x['eng'] and x['tib']]
byseg=collections.defaultdict(set)
for x in L: byseg[(x['course'],x['seg'])].add((nt(x['tib']),ne(x['eng'])))
order=[k for k in sorted(byseg,key=lambda z:(z[0],z[1])) if k in rowmap]
N=len(order); cut=int(N*0.6); known,cand=order[:cut],order[cut:]
def COST(k):
    w,e=rowmap[k]; return 177210.0+575.2*(len(w.split())+len(e.split()))
K_p=set(); K_wrd=set(); K_eng=set(); K_we=set()
for k in known:
    K_p|=byseg[k]
    w,e=rowmap[k]; ws=set(munch(syls(w))); es=set(etok(e))
    K_wrd|=ws; K_eng|=es
    for a in ws:
        for b in es: K_we.add((a,b))
# TARGET = new AND not already in hgm_gloss  (the strict dictionary-relevance target)
truth={}; feats={}; cost={}
for k in cand:
    ps=byseg[k]
    truth[k]={p for p in ps-K_p if p[1] not in gl.get(p[0],())}
    w,e=rowmap[k]; ss=syls(w); ws=set(munch(ss)); es=set(etok(e))
    uW=len([x for x in ws if x not in K_wrd]); uE=len([x for x in es if x not in K_eng])
    wen=sum(1 for a in ws for b in es if (a,b) not in K_we)
    ung=sum(1 for x in ws if x not in glossed_hw)
    c=COST(k); cost[k]=c
    feats[k]=dict(nsyl=len(ss), unseen_wrd=uW, we_new=wen, unglossed=ung,
                  mix=2.0*uW+1.0*uE+wen/40.0+0.5*ung,
                  r_mix=(2.0*uW+1.0*uE+wen/40.0+0.5*ung)/c*1e6,
                  r_unglossed=ung/c*1e6, unseen_syl_share=len([s for s in ss if s not in set().union(*[set(syls(rowmap[q][0])) for q in []]) ])/max(1,len(ss)))
TOT=len(set().union(*truth.values()))
TC=sum(cost.values())
print('candidate pool %d segments, %d STRICTLY dictionary-relevant new pairs, total measured cost %.1fM billable tokens'%(len(cand),TOT,TC/1e6))
def budget_for(rank, frac):
    seen=set(); spent=0
    for k in rank:
        if len(seen)>=frac*TOT: break
        spent+=cost[k]; seen|=truth[k]
    return spent/TC, len(seen)/TOT
def greedy():
    rem=set(cand); seen=set(); out=[]
    while rem:
        best=max(rem,key=lambda k: len(truth[k]-seen)/cost[k])
        out.append(best); seen|=truth[best]; rem.discard(best)
    return out
random.seed(3); rnd=list(cand); random.shuffle(rnd)
rankings=[('random',rnd),('corpus order',list(cand))]
for f in ('unseen_wrd','we_new','unglossed','mix','r_mix','r_unglossed','nsyl'):
    rankings.append((f, sorted(cand,key=lambda k:-feats[k][f])))
rankings.append(('ORACLE greedy/cost', greedy()))
print('\nFraction of the measured budget needed to reach X%% of the strictly-new evidence:')
print('%-22s %8s %8s %8s %8s'%('ranking','50%','80%','90%','95%'))
for nm,rk in rankings:
    print('%-22s'%nm, ' '.join('%7.1f%%'%(100*budget_for(rk,f)[0]) for f in (.5,.8,.9,.95)))
