import json, collections, re, sqlite3, pickle, math, statistics
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
COURSES=['C%02d'%i for i in range(1,19)]
rows=db.execute("select course,seq,wylie,english from corpus_segments where course in (%s) order by course,seq"%','.join('?'*18),COURSES).fetchall()
man=[r for r in rows if r[0]!='C13']
occ=sum(len(munch(syls(r[2]))) for r in man)
sy=sum(len(syls(r[2])) for r in man)
print('MANDATE C01-C18 less C13: %d segments'%len(man))
print('  syllable occurrences      %d'%sy)
print('  word occurrences (munch)  %d'%occ)
print('  word occurrences per segment %.1f (bank measures %.1f d=5/7 links per segment)'%(occ/len(man), 34790/1724))

bank=json.load(open(REPO+'data/alignment/alignment_full_v1.json'))
L=[x for x in bank['links'] if x['seg'] is not None and x['d'] in (5,7) and x['eng']]
ART=re.compile(r'^(the|a|an|of|to|in|for|that|this|his|her|their|its|our|your|my|and|is|are|be|been|will|would)\s+')
def nt(s): return (s or '').strip().strip(',.;:!?').lower()
def ne(s):
    e=s.strip().lower().strip('".,;:!?()'); e=re.sub(r'\s+',' ',e); p=None
    while p!=e: p=e; e=ART.sub('',e)
    e=re.sub(r"(\w)'s$",r'\1',e)
    if len(e)>3 and e.endswith('s') and not e.endswith('ss'): e=e[:-1]
    return e
byseg=collections.defaultdict(set)
for x in L: byseg[(x['course'],x['seg'])].add((nt(x['tib']),ne(x['eng'])))
order=sorted(byseg,key=lambda k:(k[0],k[1]))
cum=[];seen=set()
for k in order:
    seen|=byseg[k]; cum.append(len(seen))
N=len(order)
print('\nHEAPS fit on (tib,normEng) pairs over %d landed segments'%N)
for f in (0.125,0.25,0.5,1.0): print('   n=%4d -> V=%5d'%(int(N*f),cum[int(N*f)-1]))
n1,n2=N//4,N
beta=math.log(cum[n2-1]/cum[n1-1])/math.log(n2/n1); K=cum[-1]/N**beta
print('   beta=%.3f  K=%.2f'%(beta,K))
for target,lab in ((5821,'mandate less C13, EXACT-DEDUPED (1748 landed + 4073 unique remaining)'),
                   (7920,'mandate less C13, all segments'),
                   (42199,'whole 75-course spine (out of mandate; illustrative only)')):
    print('   projected distinct pairs at n=%5d : %7d   [%s]'%(target, K*target**beta, lab))
proj=K*5821**beta
print('\nPROJECTION for the deduped mandate (n=5821):')
print('  distinct (tib,normEng) pairs  ~%d'%proj)
print('  word occurrences              ~%d'%occ)
print('  occurrences per distinct pair ~%.1f'%(occ/proj))
# headword projection
byseg2=collections.defaultdict(set)
for x in [y for y in bank['links'] if y['seg'] is not None and y['d'] in (5,7)]:
    if x['tib']: byseg2[(x['course'],x['seg'])].add(nt(x['tib']))
cum2=[];s2=set()
for k in sorted(byseg2,key=lambda z:(z[0],z[1])):
    s2|=byseg2[k]; cum2.append(len(s2))
M=len(cum2); b2=math.log(cum2[M-1]/cum2[M//4-1])/math.log(M/(M//4)); K2=cum2[-1]/M**b2
print('  headwords: beta=%.3f -> projected distinct headwords at n=5821: %d (mechanical whole-mandate munch gives %d word types)'%(b2,K2*5821**b2, 0))
