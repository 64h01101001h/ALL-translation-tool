import json, collections, re, sqlite3, pickle, math
REPO='/Users/adamderickandrade/ALL-translation-tool/'
db=sqlite3.connect(REPO+'build/hgm_spine_v27_2.db')
attested=pickle.load(open('/private/tmp/claude-501/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2/scratchpad/lex2.pkl','rb'))['attested']
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
rowmap={(c,s):((w or ''),(e or '')) for c,s,w,e in db.execute("select course,seq,wylie,english from corpus_segments")}
bank=json.load(open(REPO+'data/alignment/alignment_full_v1.json'))
W=[x for x in bank['links'] if x['seg'] is not None and x['d'] in (5,7)]
byseg=collections.defaultdict(set)
for x in W:
    if x['tib']: byseg[(x['course'],x['seg'])].add(x['tib'].strip().lower())
order=[k for k in sorted(byseg,key=lambda z:(z[0],z[1])) if k in rowmap]
def beta(cum):
    N=len(cum); return math.log(cum[N-1]/cum[N//4-1])/math.log(N/(N//4))
cb=[];s=set()
for k in order: s|=byseg[k]; cb.append(len(s))
cm=[];s=set()
for k in order: s|=set(munch(syls(rowmap[k][0]))); cm.append(len(s))
print('SAME 1,724 landed segments:')
print('  bank      headword types %5d  beta %.3f'%(cb[-1],beta(cb)))
print('  mechanical munch   types %5d  beta %.3f'%(cm[-1],beta(cm)))
print('  ratio bank/mechanical: %.2f'%(cb[-1]/cm[-1]))
COURSES=['C%02d'%i for i in range(1,19)]
rows=[r for r in db.execute("select course,seq,wylie from corpus_segments where course in (%s) order by course,seq"%','.join('?'*18),COURSES) if r[0]!='C13']
cf=[];s=set()
for c,q,w in rows: s|=set(munch(syls(w))); cf.append(len(s))
print('\nFULL mandate (7,920 segs) mechanical curve: types %d  beta(quarter->full) %.3f'%(cf[-1],beta(cf)))
print('  at n=1724 the mechanical curve had %d types (%.0f%% of final); the remaining 6,196 segments add %d (%.1f%%)'%(cf[1723],100*cf[1723]/cf[-1],cf[-1]-cf[1723],100*(cf[-1]-cf[1723])/cf[-1]))
# What the bank-extrapolation would predict vs what mechanical actually shows
Kb=cb[-1]/1724**beta(cb)
print('\nSANITY: bank-beta extrapolation to 7,920 predicts %d headword types;'%(Kb*7920**beta(cb)))
print('        the mechanical curve, measured over all 7,920, grows only %.2fx from its n=1724 value (%d -> %d),'%(cf[-1]/cf[1723],cf[1723],cf[-1]))
print('        i.e. Heaps extrapolation from C01-C05 OVERSTATES the mandate tail by roughly %.1fx.'%((Kb*7920**beta(cb)/cb[-1])/(cf[-1]/cf[1723])))
