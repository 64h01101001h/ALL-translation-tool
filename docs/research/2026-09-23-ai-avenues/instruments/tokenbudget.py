import json, sqlite3, collections, math, re, random
REPO='/Users/adamderickandrade/ALL-translation-tool'
d=json.load(open(REPO+'/data/alignment/alignment_full_v1.json'))
seg_heads=collections.defaultdict(set)
for l in d['links']:
    t,e=l.get('tib'),l.get('eng')
    if t and e and l.get('seg') is not None: seg_heads[(l['course'],l['seg'])].add(t)
con=sqlite3.connect(REPO+'/build/hgm_spine_v27_2.db')
rows={(c,s):(w or '',e or '') for c,s,w,e in con.execute("select course,seq,wylie,english from corpus_segments")}
SPLIT=re.compile(r"[\s/|,;:()\[\]!?.*@#$%^&_+=\"'<>{}-]+")
syls=lambda w:[x for x in SPLIT.split(w) if x]
bank=set()
for k in seg_heads:
    if k[0]!='C05': bank|=seg_heads[k]
prior_syl=set()
for k in seg_heads:
    if k[0]!='C05' and k in rows: prior_syl|=set(syls(rows[k][0]))
test=sorted([k for k in seg_heads if k[0]=='C05'], key=lambda k:k[1])
allnew=set()
for k in test: allnew|=(seg_heads[k]-bank); 
N=len(allnew)
# cost model: token cost proportional to segment text length (the agents read the segment)
# calibrate so mean cost = 322727
raw=[len(rows[k][0])+len(rows[k][1]) for k in test]
mean=sum(raw)/len(raw)
cost=[322727*r/mean for r in raw]
print('C05 cost model: mean 322,727 tok/seg; range {:,.0f}-{:,.0f}'.format(min(cost),max(cost)))
TOT=sum(cost)
feat={'length':[len(syls(rows[k][0])) for k in test],
      'novel-syl COUNT':[len(set(syls(rows[k][0]))-prior_syl) for k in test],
      'novel-syl SHARE':[ (lambda u: len(u-prior_syl)/len(u) if u else 0)(set(syls(rows[k][0]))) for k in test],
      'novel-syl per token':[ (lambda u,c: (len(u-prior_syl)/c) if u else 0)(set(syls(rows[k][0])), cost[i]) for i,k in enumerate(test)]}
def curve_tokens(order_idx, budget_frac):
    B=TOT*budget_frac; cov=set(); spent=0
    for i in order_idx:
        if spent+cost[i]>B: continue
        spent+=cost[i]; cov|=(seg_heads[test[i]]-bank)
    return len(cov)/N
print()
print('%-22s %8s %8s %8s'%('ranker','10% tok','20% tok','30% tok'))
for name,f in feat.items():
    idx=sorted(range(len(test)),key=lambda i:-f[i])
    print('%-22s %7.1f%% %7.1f%% %7.1f%%'%(name,100*curve_tokens(idx,.1),100*curve_tokens(idx,.2),100*curve_tokens(idx,.3)))
random.seed(5); r1=r2=r3=0
for _ in range(60):
    idx=list(range(len(test))); random.shuffle(idx)
    r1+=curve_tokens(idx,.1); r2+=curve_tokens(idx,.2); r3+=curve_tokens(idx,.3)
print('%-22s %7.1f%% %7.1f%% %7.1f%%'%('random',100*r1/60,100*r2/60,100*r3/60))
# greedy oracle under token budget (max coverage per token)
def greedy(budget_frac):
    B=TOT*budget_frac; cov=set(); spent=0; rem=set(range(len(test)))
    while True:
        best=None;bv=0
        for i in rem:
            if spent+cost[i]>B: continue
            g=len(seg_heads[test[i]]-bank-cov)/cost[i]
            if g>bv: bv=g;best=i
        if best is None: break
        cov|=(seg_heads[test[best]]-bank); spent+=cost[best]; rem.discard(best)
    return len(cov)/N
print('%-22s %7.1f%% %7.1f%% %7.1f%%'%('ORACLE (greedy/tok)',100*greedy(.1),100*greedy(.2),100*greedy(.3)))
