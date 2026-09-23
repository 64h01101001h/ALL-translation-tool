import json,sqlite3,collections,time,statistics
ROOT="/Users/adamderickandrade/ALL-translation-tool"
t0=time.time()
bank=json.load(open(ROOT+"/data/alignment/alignment_full_v1.json"))["links"]
t_load=time.time()-t0
t0=time.time()
prec=collections.defaultdict(lambda: collections.defaultdict(set))
for l in bank:
    if l.get("d")==5 and l.get("eng") and l.get("tib"):
        prec[l["tib"]][l["eng"]].add((l["course"],l["seg"]))
t_idx=time.time()-t0
con=sqlite3.connect(ROOT+"/build/hgm_spine_v27_2.db")
t0=time.time()
gloss=collections.defaultdict(set)
for w,raw in con.execute("select wylie, raw from entries"):
    try:o=json.loads(raw)
    except:continue
    g=o.get("hgm_gloss")
    if not g: continue
    if isinstance(g,str): g=[g]
    for s in g:
        if isinstance(s,str) and s.strip(): gloss[w].add(s.strip())
t_gloss=time.time()-t0
rows=[(c,s,w or "",e or "") for c,s,w,e in con.execute(
  "select course,seq,wylie,english from corpus_segments where course in ('C05','C06','C07','C08')")]
def ngrams(w,MAXN=6):
    t=w.split(); out=set()
    for i in range(len(t)):
        for n in range(1,MAXN+1):
            if i+n>len(t): break
            g=" ".join(t[i:i+n]); out.add(g); g2=g.strip(",.;:!?")
            if g2: out.add(g2)
    return out
t0=time.time(); tot=0
for c,s,w,e in rows:
    if not w or not e: continue
    cands=set()
    for t in ngrams(w):
        for en,segs in prec.get(t,{}).items():
            if en in e: cands.add((t,en))
        for en in gloss.get(t,()):
            if en in e: cands.add((t,en))
    tot+=len(cands)
dt=time.time()-t0
print("index build ONCE: bank load %.2fs + precedent index %.2fs + gloss index %.2fs = %.2fs"%(t_load,t_idx,t_gloss,t_load+t_idx+t_gloss))
print("candidate generation over %d segments: %.2fs  => %.1f ms/segment, %.0f candidates/segment"%(len(rows),dt,1000*dt/len(rows),tot/len(rows)))
print("projected for 4,073 segments: %.1f s"%(dt/len(rows)*4073))
