import json, re, sqlite3, collections, math, statistics as st
ROOT='/Users/adamderickandrade/ALL-translation-tool'
db=sqlite3.connect(f'file:{ROOT}/build/hgm_spine_v27_2.db?mode=ro',uri=True)
STOP=set('the a an of to and or in is that this it be for on as with by his her their our your we you i my are was not which there these those one who what when from at into its all any has have had will can may must do does also such so than then but if no'.split())
def cw(s): return [w for w in re.findall(r"[a-z]+", s.lower()) if w not in STOP and len(w)>=4]
lex=collections.defaultdict(set)
for l in json.load(open(f'{ROOT}/data/alignment/alignment_full_v1.json'))['links']:
    if l['d'] in (5,7) and l['eng'] and l['tib']:
        ws=cw(l['eng'])
        if ws: lex[' '.join(l['tib'].split())].add(tuple(ws))
ng=0
for w,g in []:
    try: gl=json.loads(g)
    except: continue
    for x in gl if isinstance(gl,list) else [gl]:
        if isinstance(x,str):
            ws=cw(x)
            if ws: lex[' '.join(w.split())].add(tuple(ws)); ng+=1
print('lexicon tib keys', len(lex), 'hgm_gloss renderings', ng)
rows=db.execute('select course, seq, wylie, english from corpus_segments order by course, seq').fetchall()
bycourse=collections.defaultdict(list)
for c,s,w,e in rows: bycourse[c].append((s,w or '',e or ''))
def syls(w): return re.sub(r"[,/|;:!?\[\]{}()\"@#*0-9]+",' ',w.lower()).split()
def anchors(w):
    sy=syls(w); out=[]
    for n in (4,3,2,1):
        for i in range(len(sy)-n+1):
            k=' '.join(sy[i:i+n])
            if k in lex: out.append(k)
    return out
def score(w, e):
    A=anchors(w)
    if not A: return None
    E=set(cw(e)); hit=0
    for k in A:
        if any(all(x in E for x in r) for r in lex[k]): hit+=1
    return hit/len(A)
res={}
for c,rs in bycourse.items():
    sc=[]
    for i,(s,w,e) in enumerate(rs):
        l0=score(w,e)
        lm=score(w,rs[i-1][2]) if i>0 else None
        lp=score(w,rs[i+1][2]) if i+1<len(rs) else None
        sc.append((s,l0,lm,lp))
    res[c]=sc
# window statistic: over W consecutive rows, mean(lag-1) - mean(lag0), and mean(lag+1)-mean(lag0)
W=8
flags=[]
allwins=0
for c,sc in res.items():
    for i in range(len(sc)-W+1):
        win=[x for x in sc[i:i+W] if None not in x[1:]]
        if len(win)<W*0.75: continue
        allwins+=1
        m0=st.mean(x[1] for x in win); mm=st.mean(x[2] for x in win); mp=st.mean(x[3] for x in win)
        d=max(mm,mp)-m0
        flags.append((d,c,sc[i][0],sc[i+W-1][0],round(m0,3),round(mm,3),round(mp,3)))
flags.sort(reverse=True)
print('windows scored', allwins)
for f in flags[:25]: print(f)
c13=[f for f in flags if f[1]=='C13' and f[2]>=58 and f[3]<=86]
rank={id(f):r for r,f in enumerate(flags)}
print('best C13:63-82 windows:')
for f in sorted(c13,reverse=True)[:5]: print('  rank',rank[id(f)]+1,f)
# row-level lag0 coverage
cov=sum(1 for c,sc in res.items() for x in sc if x[1] is not None); tot=sum(len(sc) for sc in res.values())
print('rows with >=1 lexical anchor', cov, 'of', tot)
print('median lag0 score', st.median(x[1] for sc in res.values() for x in sc if x[1] is not None))
print('median lag-1 score', st.median(x[2] for sc in res.values() for x in sc if x[2] is not None))
print('---- regions')
for thr in (0.3,0.2,0.15,0.1):
    sel=[f for f in flags if f[0]>thr]
    # merge overlapping windows into regions per course
    regs=[]
    for f in sorted(sel,key=lambda x:(x[1],x[2])):
        if regs and regs[-1][0]==f[1] and f[2]<=regs[-1][2]+1: regs[-1][2]=max(regs[-1][2],f[3]); regs[-1][3]=max(regs[-1][3],f[0])
        else: regs.append([f[1],f[2],f[3],f[0]])
    print(f'thr {thr}: windows {len(sel)}, regions {len(regs)}; mandate(C01-C18 ex C13) regions', sum(1 for r in regs if r[0] in {f"C{i:02d}" for i in range(1,19)} - {"C13"}))
    if thr==0.1:
        for r in regs: print('   ', r[0], r[1], '-', r[2], round(r[3],3))
