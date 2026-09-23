import json,re,sqlite3,collections,time
R='/Users/adamderickandrade/ALL-translation-tool/'
t0=time.time()
ev=json.load(open(R+'data/alignment/alignment_evidence_v1.json'))
pairs=ev['pairs']
con=sqlite3.connect(R+'build/hgm_spine_v27_2.db');cur=con.cursor()
cur.execute("select course,seq,wylie from corpus_segments")
segs=[(c,s,w or '') for c,s,w in cur.fetchall()]
c18=[r for r in segs if re.fullmatch(r'C(0[1-9]|1[0-8])',r[0])]
K=4
inv=collections.defaultdict(list)
for i,(c,s,w) in enumerate(c18):
    for j in range(0,max(0,len(w)-K+1)):
        inv[w[j:j+K]].append(i)
for k in inv: inv[k]=set(inv[k])
def freq(h):
    if len(h)<K:
        return sum(1 for c,s,w in c18 if h in w)
    sh=[h[j:j+K] for j in range(len(h)-K+1)]
    cand=min((inv.get(x,set()) for x in sh), key=len)
    return sum(1 for i in cand if h in c18[i][2])
f={h:freq(h) for h in pairs}
fv=sorted(f.values())
n=len(fv)
def q(p): return fv[int(p*(n-1))]
print('segments C01-C18',len(c18),'headwords',n,'elapsed %.1fs'%(time.time()-t0))
print('zero',sum(1 for v in fv if v==0),'==1',sum(1 for v in fv if v==1),'<=2',sum(1 for v in fv if v<=2),
      '>=10',sum(1 for v in fv if v>=10),'>=50',sum(1 for v in fv if v>=50),'>=200',sum(1 for v in fv if v>=200))
print('deciles',[q(p) for p in [0,.1,.2,.3,.4,.5,.6,.7,.8,.9,1.0]])
json.dump(f,open('/private/tmp/claude-501/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2/scratchpad/freq2.json','w'))
# hapax band and common band definition
print('band rare(<=2):',sum(1 for v in fv if v<=2),' mid(3-19):',sum(1 for v in fv if 3<=v<=19),' common(>=20):',sum(1 for v in fv if v>=20))
