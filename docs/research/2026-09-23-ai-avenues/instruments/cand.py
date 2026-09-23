import json, sqlite3, re, sys, time, collections, statistics as st
ROOT="/Users/adamderickandrade/ALL-translation-tool"
t0=time.time()
bank=json.load(open(ROOT+"/data/alignment/alignment_full_v1.json"))["links"]
db=sqlite3.connect(ROOT+"/build/hgm_spine_v27_2.db")
segtext={}
for c,s,w,e in db.execute("select course,seq,wylie,english from corpus_segments"):
    segtext[(c,s)]=(w or "", e or "")
# dictionary headwords + glosses
gloss=collections.defaultdict(set)
heads=set()
import json as J
n=0
for (wy,raw) in db.execute("select wylie,raw from entries"):
    if not wy: continue
    heads.add(wy)
    try: o=J.loads(raw)
    except Exception: continue
    g=o.get("hgm_gloss")
    if isinstance(g,list):
        for x in g:
            if isinstance(x,str) and x.strip(): gloss[wy].add(x.strip())
    elif isinstance(g,str) and g.strip(): gloss[wy].add(g.strip())
print("dict headwords %d, glossed %d  (%.1fs)"%(len(heads),len(gloss),time.time()-t0))

# bank d=5 index: tib -> set(eng), and per-(course,seq) truth
bank_eng=collections.defaultdict(collections.Counter)
truth=collections.defaultdict(set)      # (course,seq) -> set((tib,eng)) at d=5
truth_tib=collections.defaultdict(set)
for L in bank:
    if L.get("d")!=5: continue
    tib=L.get("tib"); eng=L.get("eng"); c=L.get("course"); s=L.get("seg")
    if not tib: continue
    if s is not None:
        truth_tib[(c,s)].add(tib)
        if eng: truth[(c,s)].add((tib,eng))
    if eng: bank_eng[tib][(c,s)]=1  # placeholder, rebuilt below
# rebuild with (tib->eng counter) but we will subtract per-segment later
pairs=collections.defaultdict(collections.Counter)   # tib -> Counter(eng)
pair_src=collections.defaultdict(lambda: collections.defaultdict(set))  # tib->eng->set(course,seq)
for L in bank:
    if L.get("d")!=5: continue
    tib=L.get("tib"); eng=L.get("eng")
    if not tib or not eng: continue
    pairs[tib][eng]+=1
    pair_src[tib][eng].add((L.get("course"),L.get("seg")))
print("bank d5 headwords %d  (%.1fs)"%(len(pairs),time.time()-t0))

# lexicon for segmentation: bank headwords + dictionary headwords
LEX = set(pairs.keys()) | set(truth_tib_k for ks in [] for truth_tib_k in [])
LEX |= heads
LEX = {w for w in LEX if w and not w.startswith(" ") and not w.endswith(" ")}
maxsyl = 0
LEXBY=collections.defaultdict(set)
for w in LEX:
    n=w.count(" ")+1
    if n<=8: LEXBY[n].add(w); maxsyl=max(maxsyl,n)
print("lexicon %d entries, max %d syllables (%.1fs)"%(len(LEX),maxsyl,time.time()-t0))
json.dump({"ok":1},open("/dev/null","w"))

SYL=re.compile(r"[^\s]+")
def segment(wy):
    """maximal-munch over syllable tokens using LEX; returns list of (start,end,string)"""
    toks=[(m.start(),m.end(),m.group()) for m in SYL.finditer(wy)]
    out=[]; i=0
    while i<len(toks):
        hit=None
        for n in range(min(maxsyl,len(toks)-i),0,-1):
            cand=" ".join(t[2] for t in toks[i:i+n])
            c2=cand.strip(",.;:")
            if cand in LEXBY.get(n,()): hit=(n,cand); break
            if c2!=cand and c2 in LEXBY.get(n,()): hit=(n,c2); break
        if hit:
            n,cand=hit; out.append(cand); i+=n
        else:
            out.append(toks[i][2]); i+=1
    return out

def candidates(course,seq,wy,en,exclude_self=True):
    """deterministic (tib,eng) candidate pairs"""
    segs=segment(wy)
    enl=en
    cands=set(); tibs=[]
    for t in segs:
        tibs.append(t)
        opts=collections.Counter()
        for e,c in pairs.get(t,{}).items():
            if exclude_self:
                srcs=pair_src[t][e]
                c2=c-sum(1 for s in srcs if s==(course,seq))
                # subtract count contributed by this very segment
                cnt=c - sum(1 for L in [] )
                if (course,seq) in srcs and len(srcs)==1: continue
            opts[e]+=c
        for g in gloss.get(t,()):
            opts[g]+=0
        for e in opts:
            if e and e in enl: cands.add((t,e))
    return segs,cands

# evaluate on landed segments
courses=collections.Counter()
rows=[]
for (c,s) in sorted(truth.keys()):
    if s is None: continue
    if (c,s) not in segtext: continue
    wy,en=segtext[(c,s)]
    if not wy or not en: continue
    T=truth[(c,s)]
    segs,C=candidates(c,s,wy,en)
    hit=len(T & C)
    rows.append((c,s,len(T),len(C),hit,len(segs)))
    courses[c]+=1
tot_t=sum(r[2] for r in rows); tot_c=sum(r[3] for r in rows); tot_h=sum(r[4] for r in rows)
print("\n=== DETERMINISTIC CANDIDATE GENERATOR vs the reconciled d=5 bank ===")
print("segments evaluated: %d  %s"%(len(rows),dict(courses)))
print("true d5 pairs (with English): %d"%tot_t)
print("candidate pairs offered:      %d"%tot_c)
print("candidates that ARE true:     %d"%tot_h)
print("RECALL  (true pairs offered): %.1f%%"%(100*tot_h/tot_t))
print("PRECISION(offered that true): %.1f%%"%(100*tot_h/max(tot_c,1)))
print("candidates per segment: mean %.1f median %.0f"%(st.mean([r[3] for r in rows]),st.median([r[3] for r in rows])))
print("true pairs per segment: mean %.1f median %.0f"%(st.mean([r[2] for r in rows]),st.median([r[2] for r in rows])))
print("segmenter tokens/segment: mean %.1f"%st.mean([r[5] for r in rows]))
per=collections.defaultdict(lambda:[0,0,0])
for c,s,t,cc,h,_ in rows:
    per[c][0]+=t; per[c][1]+=cc; per[c][2]+=h
print("\nper course: course  true  cands  hit   recall  prec")
for c in sorted(per):
    t,cc,h=per[c]
    print("  %-5s %6d %6d %5d  %5.1f%%  %5.1f%%"%(c,t,cc,h,100*h/max(t,1),100*h/max(cc,1)))
print("(%.1fs)"%(time.time()-t0))
