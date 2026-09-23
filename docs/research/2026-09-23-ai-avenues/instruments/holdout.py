import json, sqlite3, collections, math, re, random
REPO='/Users/adamderickandrade/ALL-translation-tool'
d=json.load(open(REPO+'/data/alignment/alignment_full_v1.json'))
L=d['links']
seg_heads=collections.defaultdict(set)
for l in L:
    t,e=l.get('tib'),l.get('eng')
    if t and e and l.get('seg') is not None:
        seg_heads[(l['course'],l['seg'])].add(t)
con=sqlite3.connect(REPO+'/build/hgm_spine_v27_2.db')
wy={}; 
for co,sq,w in con.execute("select course,seq,wylie from corpus_segments"):
    wy[(co,sq)]=w or ''
SPLIT=re.compile(r"[\s/|,;:()\[\]!?.*@#$%^&_+=\"'<>{}-]+")
def syls(w): return [s for s in SPLIT.split(w) if s]

prior=[k for k in seg_heads if k[0] in ('C01','C02','C03','C04')]
test=sorted([k for k in seg_heads if k[0]=='C05'], key=lambda k:k[1])
bank=set(); 
for k in prior: bank|=seg_heads[k]
prior_syl=set()
for k in prior:
    if k in wy: prior_syl|=set(syls(wy[k]))
print('prior bank headwords',len(bank),'prior syllables',len(prior_syl),'test segments',len(test))

truth=[]; f_cnt=[]; f_share=[]; f_len=[]
for k in test:
    u=set(syls(wy.get(k,'')))
    novel=u-prior_syl
    f_cnt.append(len(novel)); f_share.append(len(novel)/len(u) if u else 0); f_len.append(len(u))
    truth.append(len(seg_heads[k]-bank))
def pearson(a,b):
    n=len(a); ma=sum(a)/n; mb=sum(b)/n
    num=sum((x-ma)*(y-mb) for x,y in zip(a,b))
    da=math.sqrt(sum((x-ma)**2 for x in a)); db=math.sqrt(sum((y-mb)**2 for y in b))
    return num/(da*db) if da and db else float('nan')
tot=sum(truth)
def q(feat,frac=0.2):
    idx=sorted(range(len(feat)), key=lambda i:-feat[i])[:int(len(feat)*frac)]
    # union-based: distinct NEW headwords captured
    u=set()
    for i in idx: u|=(seg_heads[test[i]]-bank)
    return sum(truth[i] for i in idx)/tot, len(u)
allnew=set()
for k in test: allnew|=(seg_heads[k]-bank)
print('C05 new headwords (occurrence-summed)',tot,'distinct',len(allnew))
for name,f in (('novel-syllable COUNT',f_cnt),('novel-syllable SHARE',f_share),('segment length',f_len)):
    s,u=q(f)
    print('%-24s r=%.3f  top-20%%: %.1f%% of occ-new, %d/%d distinct new (%.1f%%)'%(name,pearson(f,truth),100*s,u,len(allnew),100*u/len(allnew)))
random.seed(3); rs=[]; ru=[]
for _ in range(200):
    f=[random.random() for _ in test]; s,u=q(f); rs.append(s); ru.append(u/len(allnew))
print('random top-20%%: %.1f%% occ-new, %.1f%% distinct new'%(100*sum(rs)/len(rs),100*sum(ru)/len(ru)))
# greedy oracle for distinct coverage with 20% budget
rem=dict((k,seg_heads[k]-bank) for k in test); cov=set(); picked=0
budget=int(len(test)*0.2)
for _ in range(budget):
    best=max(rem, key=lambda k: len(rem[k]-cov))
    cov|=rem[best]; del rem[best]
print('greedy oracle top-20%%: %.1f%% distinct new'%(100*len(cov)/len(allnew)))
# zero-yield segments
z=sum(1 for t in truth if t==0)
print('C05 segments with zero new headwords: %d/%d (%.1f%%)'%(z,len(test),100*z/len(test)))
print('C05 mean new headwords/segment %.2f'%(tot/len(test)))
