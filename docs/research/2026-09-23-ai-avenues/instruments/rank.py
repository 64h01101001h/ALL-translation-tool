import json, sqlite3, re, collections, statistics as st, time
ROOT="/Users/adamderickandrade/ALL-translation-tool"
bank=json.load(open(ROOT+"/data/alignment/alignment_full_v1.json"))["links"]
db=sqlite3.connect(ROOT+"/build/hgm_spine_v27_2.db")
segtext={(c,s):(w or "",e or "") for c,s,w,e in db.execute("select course,seq,wylie,english from corpus_segments")}
gloss=collections.defaultdict(set); heads=set()
for (wy,raw) in db.execute("select wylie,raw from entries"):
    if not wy: continue
    heads.add(wy)
    try: o=json.loads(raw)
    except Exception: continue
    g=o.get("hgm_gloss")
    if isinstance(g,list):
        for x in g:
            if isinstance(x,str) and x.strip(): gloss[wy].add(x.strip())
pair_src=collections.defaultdict(lambda: collections.defaultdict(set)); truth=collections.defaultdict(set)
for L in bank:
    if L.get("d")!=5: continue
    tib,eng,c,s=L.get("tib"),L.get("eng"),L.get("course"),L.get("seg")
    if tib and eng:
        pair_src[tib][eng].add((c,s))
        if s is not None: truth[(c,s)].add((tib,eng))
LEX=set(pair_src)|heads; LEX={w for w in LEX if w and w==w.strip()}
LEXBY=collections.defaultdict(set); maxsyl=0
for w in LEX:
    n=w.count(" ")+1
    if n<=8: LEXBY[n].add(w); maxsyl=max(maxsyl,n)
SYL=re.compile(r"[^\s]+")
def lattice(wy):
    toks=[m.group() for m in SYL.finditer(wy)]; out=set()
    for i in range(len(toks)):
        for n in range(1,min(maxsyl,len(toks)-i)+1):
            cand=" ".join(toks[i:i+n]); c2=cand.strip(",.;:")
            if cand in LEXBY.get(n,()): out.add(cand)
            if c2!=cand and c2 in LEXBY.get(n,()): out.add(c2)
    return out

print("=== RANKED CANDIDATES: keep only top-K attested English per lattice Tibetan ===")
for K in (1,2,3,99):
    S=collections.Counter(); cp=[]
    for (c,s),T in truth.items():
        if s is None or (c,s) not in segtext: continue
        wy,en=segtext[(c,s)]
        if not wy or not en: continue
        cands=set()
        for tib in lattice(wy):
            scored=[]
            for e,srcs in pair_src.get(tib,{}).items():
                rest={x for x in srcs if x[0]!=c}       # leave-one-COURSE-out
                if rest and e in en: scored.append((len(rest),e))
            for g in gloss.get(tib,()):
                if g in en and not any(e==g for _,e in scored): scored.append((0,g))
            scored.sort(key=lambda x:(-x[0],len(x[1])))
            for _,e in scored[:K]: cands.add((tib,e))
        cp.append(len(cands)); S["tot"]+=len(T); S["cand"]+=len(cands); S["hit"]+=len(T&cands)
    print("  K=%-3s recall %5.1f%%  precision %5.1f%%  candidates/segment %6.1f  cand:true %.1f"%(
        "all" if K==99 else K, 100*S['hit']/S['tot'], 100*S['hit']/max(S['cand'],1),
        st.mean(cp), S['cand']/max(S['hit'],1)))

print("\n=== FREE TRIAGE OF THE 4,073 UNIQUE REMAINING C01-C18 SEGMENTS ===")
banked_segs={(L.get("course"),L.get("seg")) for L in bank if L.get("seg") is not None}
MAND=["C%02d"%i for i in range(1,19)]
rows=list(db.execute("select course,seq,wylie,english from corpus_segments where course in (%s)"%
        ",".join("'%s'"%c for c in MAND)))
print("C01-C18 spine segments:",len(rows))
rem=[r for r in rows if (r[0],r[1]) not in banked_segs and r[0]!="C13"]
print("less landed, less C13:",len(rem))
seen=set(); uniq=[]
for c,s,w,e in rows:
    if (c,s) in banked_segs: seen.add(((w or "").strip(),(e or "").strip()))
for c,s,w,e in rem:
    k=((w or "").strip(),(e or "").strip())
    if k in seen: continue
    seen.add(k); uniq.append((c,s,w or "",e or ""))
print("UNIQUE remaining:",len(uniq))
lookupable=0; novelshare=[]; ntok=[]; zero_novel=0
for c,s,w,e in uniq:
    lat=lattice(w)
    toks=[m.group() for m in SYL.finditer(w)]
    ntok.append(len(toks))
    known=sum(1 for t in toks if t in LEX or any(t in l.split() for l in lat))
    novel=[t for t in toks if t not in LEX]
    novelshare.append(len(novel)/max(len(toks),1))
    if not novel: zero_novel+=1
print("  syllable tokens/segment: mean %.1f median %.0f"%(st.mean(ntok),st.median(ntok)))
print("  share of syllables NOT a known lexicon headword: mean %.1f%% median %.1f%%"%(
    100*st.mean(novelshare),100*st.median(novelshare)))
print("  segments with ZERO unknown syllables: %d (%.1f%%)"%(zero_novel,100*zero_novel/len(uniq)))
q=sorted(novelshare)
print("  novelty deciles: "+" ".join("%.0f%%"%(100*q[int(len(q)*i/10)]) for i in range(10)))
