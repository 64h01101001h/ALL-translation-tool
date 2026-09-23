import json, sqlite3, collections, statistics, time, re
ROOT="/Users/adamderickandrade/ALL-translation-tool"
con=sqlite3.connect(ROOT+"/build/hgm_spine_v27_2.db")
C18=["C%02d"%i for i in range(1,19)]
rows=[(c,s,w or "",e or "") for c,s,w,e in con.execute(
  "select course,seq,wylie,english from corpus_segments where course in (%s)"%",".join("'%s'"%c for c in C18))]
print("C01-C18 segments:",len(rows))
bank=json.load(open(ROOT+"/data/alignment/alignment_full_v1.json"))["links"]
landed={(l["course"],l["seg"]) for l in bank if l.get("seg") is not None}
print("landed (course,seq) pairs:",len(landed))
read=[r for r in rows if (r[0],r[1]) in landed]
rem=[r for r in rows if (r[0],r[1]) not in landed and r[0]!="C13"]
print("C13 excluded:",sum(1 for r in rows if r[0]=="C13"))
print("remaining after landed+C13:",len(rem))
# exact dedup
seen={(w,e) for _,_,w,e in read}
uniq=[];  local=set()
for c,s,w,e in rem:
    if (w,e) in seen: continue
    if (w,e) in local: continue
    local.add((w,e)); uniq.append((c,s,w,e))
print("UNIQUE REMAINING (exact dedup vs read and each other):",len(uniq))

# ---- near-duplicate: wylie shingle Jaccard against already-read text ----
def shingles(w,k=3):
    t=w.split()
    return {" ".join(t[i:i+k]) for i in range(max(1,len(t)-k+1))}
t0=time.time()
read_sh=[(c,s,shingles(w)) for c,s,w,e in read if w]
# inverted index on shingles for speed
inv=collections.defaultdict(list)
for i,(c,s,sh) in enumerate(read_sh):
    for g in sh: inv[g].append(i)
bands=collections.Counter(); best=[]
for c,s,w,e in uniq:
    sh=shingles(w)
    if not sh: bands['no-wylie']+=1; continue
    cnt=collections.Counter()
    for g in sh:
        for i in inv.get(g,()): cnt[i]+=1
    b=0.0
    for i,ov in cnt.most_common(5):
        j=ov/ (len(sh)+len(read_sh[i][2])-ov)
        if j>b: b=j
    best.append(b)
    if b>=0.9: bands['>=0.90']+=1
    elif b>=0.7: bands['0.70-0.90']+=1
    elif b>=0.5: bands['0.50-0.70']+=1
    elif b>=0.3: bands['0.30-0.50']+=1
    else: bands['<0.30']+=1
print("near-duplicate scan of %d unique-remaining vs %d already-read, %.1fs"%(len(uniq),len(read_sh),time.time()-t0))
for k in ['>=0.90','0.70-0.90','0.50-0.70','0.30-0.50','<0.30','no-wylie']:
    print("   best wylie 3-shingle Jaccard %-12s %5d  %5.1f%%"%(k,bands[k],100*bands[k]/len(uniq)))
print("   median best-J %.3f"%statistics.median(best))
