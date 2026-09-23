import json, sqlite3, collections, math, re, random
REPO='/Users/adamderickandrade/ALL-translation-tool'
d=json.load(open(REPO+'/data/alignment/alignment_full_v1.json'))
seg_heads=collections.defaultdict(set)
for l in d['links']:
    t,e=l.get('tib'),l.get('eng')
    if t and e and l.get('seg') is not None:
        seg_heads[(l['course'],l['seg'])].add(t)
con=sqlite3.connect(REPO+'/build/hgm_spine_v27_2.db')
wy={(c,s):(w or '') for c,s,w in con.execute("select course,seq,wylie from corpus_segments")}
SPLIT=re.compile(r"[\s/|,;:()\[\]!?.*@#$%^&_+=\"'<>{}-]+")
syls=lambda w:[x for x in SPLIT.split(w) if x]
bank=set()
for k in seg_heads:
    if k[0]!='C05': bank|=seg_heads[k]
test=sorted([k for k in seg_heads if k[0]=='C05'], key=lambda k:k[1])
allnew=set()
for k in test: allnew|=(seg_heads[k]-bank)
N=len(allnew)
feat_len=[len(syls(wy.get(k,''))) for k in test]
def curve(order_idx):
    cov=set(); out=[]
    for j,i in enumerate(order_idx,1):
        cov|=(seg_heads[test[i]]-bank); out.append(len(cov)/N)
    return out
by_len=curve(sorted(range(len(test)), key=lambda i:-feat_len[i]))
random.seed(11)
rnd=[0.0]*len(test)
for _ in range(50):
    idx=list(range(len(test))); random.shuffle(idx); c=curve(idx)
    rnd=[a+b/50 for a,b in zip(rnd,c)]
# greedy oracle
rem={k:(seg_heads[k]-bank) for k in test}; cov=set(); orc=[]
while rem:
    b=max(rem,key=lambda k:len(rem[k]-cov)); cov|=rem.pop(b); orc.append(len(cov)/N)
def budget_for(c,target):
    for i,v in enumerate(c,1):
        if v>=target: return i
    return None
print('C05: %d segments, %d distinct new headwords'%(len(test),N))
print('%-10s %8s %8s %8s'%('target','length','random','oracle'))
for t in (0.25,0.5,0.75,0.9,1.0):
    print('%-10.0f%% %8s %8s %8s'%(t*100,budget_for(by_len,t),budget_for(rnd,t),budget_for(orc,t)))
for b in (0.1,0.2,0.3,0.5):
    i=int(len(test)*b)-1
    print('budget %.0f%% (%d segs): length %.1f%%  random %.1f%%  oracle %.1f%%'%(b*100,i+1,100*by_len[i],100*rnd[i],100*orc[i]))

print('--- matched-yield budgets (segments needed to reach target coverage) ---')
for t in (0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,0.95,1.0):
    bl=budget_for(by_len,t); bo=budget_for(orc,t); br=budget_for(rnd,t)
    print('t=%3.0f%%  random %3s  length %3s  oracle %3s   oracle saves %s segs vs length (%.1f%% of course)'%(
        t*100,br,bl,bo,(bl-bo) if (bl and bo) else '?', 100*(bl-bo)/len(test) if (bl and bo) else 0))
