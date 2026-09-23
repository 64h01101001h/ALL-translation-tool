import json, sqlite3, collections, statistics, time
ROOT="/Users/adamderickandrade/ALL-translation-tool"
con=sqlite3.connect(ROOT+"/build/hgm_spine_v27_2.db")
C18=["C%02d"%i for i in range(1,19)]
rows={(c,s):(w or "",e or "") for c,s,w,e in con.execute(
  "select course,seq,wylie,english from corpus_segments where course in (%s)"%",".join("'%s'"%c for c in C18))}
bank=json.load(open(ROOT+"/data/alignment/alignment_full_v1.json"))["links"]
landed={(l["course"],l["seg"]) for l in bank if l.get("seg") is not None}
spec=collections.defaultdict(list)
for l in bank:
    if l.get("seg") is None: continue
    if l.get("d")==5 and l.get("tib") and l.get("eng"):
        spec[(l["course"],l["seg"])].append((l["tib"],l["eng"]))
read=[k for k in rows if k in landed and rows[k][0]]
rem=[k for k in rows if k not in landed and k[0]!="C13"]
seen={rows[k] for k in read}; local=set(); uniq=[]
for k in rem:
    v=rows[k]
    if v in seen or v in local: continue
    local.add(v); uniq.append(k)
def sh(w,k=3):
    t=w.split(); return {" ".join(t[i:i+k]) for i in range(max(1,len(t)-k+1))}
inv=collections.defaultdict(list); rsh=[]
for k in read:
    s=sh(rows[k][0]); rsh.append((k,s))
    for g in s: inv[g].append(len(rsh)-1)
t0=time.time(); surv=[]; nd=0; transferable=0
def cursor_ok(text, pieces):
    """forward-only cursor, the generator's rule 2, both sides"""
    pos=0; kept=[]
    for p in pieces:
        i=text.find(p,pos)
        if i<0: continue
        pos=i+len(p); kept.append(p)
    return kept
for k in uniq:
    w,e=rows[k]; s=sh(w)
    if not s: continue
    cnt=collections.Counter()
    for g in s:
        for i in inv.get(g,()): cnt[i]+=1
    bj=0; bk=None
    for i,ov in cnt.most_common(5):
        j=ov/(len(s)+len(rsh[i][1])-ov)
        if j>bj: bj,bk=j,rsh[i][0]
    if bj<0.90: continue
    nd+=1
    src=spec.get(bk,[])
    # keep pairs whose tib occurs in the new wylie and eng in the new english
    cand=[(t,en) for t,en in src if t in w and en in e]
    # forward cursor on the Tibetan side
    kt=set(cursor_ok(w,[t for t,_ in cand]))
    ok=[(t,en) for t,en in cand if t in kt]
    surv.append((len(ok), len(src)))
    transferable+=len(ok)
print("near-duplicate segments (J>=0.90) examined: %d  in %.1fs"%(nd,time.time()-t0))
if surv:
    r=[a/b for a,b in surv if b]
    print("source page d5 spans: mean %.1f"%statistics.mean([b for a,b in surv]))
    print("spans that TRANSFER verbatim+in-order: mean %.1f median %.1f"%(
        statistics.mean([a for a,b in surv]), statistics.median([a for a,b in surv])))
    print("share of the source page that transfers: mean %.1f%% median %.1f%%"%(
        100*statistics.mean(r),100*statistics.median(r)))
    print("total transferable d5 pairs across the %d near-duplicates: %d"%(nd,transferable))
