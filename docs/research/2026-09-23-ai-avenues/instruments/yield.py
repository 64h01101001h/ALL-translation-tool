import json, sqlite3, re, collections, statistics as st
ROOT="/Users/adamderickandrade/ALL-translation-tool"
bank=json.load(open(ROOT+"/data/alignment/alignment_full_v1.json"))["links"]
db=sqlite3.connect(ROOT+"/build/hgm_spine_v27_2.db")
segtext={(c,s):(w or "",e or "") for c,s,w,e in db.execute("select course,seq,wylie,english from corpus_segments")}
banked={(L.get("course"),L.get("seg")) for L in bank if L.get("seg") is not None}
MAND=["C%02d"%i for i in range(1,19)]
rows=list(db.execute("select course,seq,wylie,english from corpus_segments where course in (%s) order by course,seq"%",".join("'%s'"%c for c in MAND)))
seen=set()
for c,s,w,e in rows:
    if (c,s) in banked: seen.add(((w or "").strip(),(e or "").strip()))
uniq=[]
for c,s,w,e in rows:
    if (c,s) in banked or c=="C13": continue
    k=((w or "").strip(),(e or "").strip())
    if k in seen: continue
    seen.add(k); uniq.append((c,s,w or "",e or ""))
cw=sum(len(w) for _,_,w,_ in uniq); ce=sum(len(e) for _,_,_,e in uniq)
print("=== SOURCE VOLUME, the 4,073 UNIQUE REMAINING C01-C18 SEGMENTS ===")
print("segments %d   wylie chars %d   english chars %d   TOTAL %d"%(len(uniq),cw,ce,cw+ce))
print("per segment: %.0f wylie + %.0f english = %.0f chars"%(cw/len(uniq),ce/len(uniq),(cw+ce)/len(uniq)))
for cpt in (3.6,4.0):
    print("  at %.1f chars/token (ESTIMATE): %.0f source tokens total, %.0f per segment"%(
        cpt,(cw+ce)/cpt,(cw+ce)/cpt/len(uniq)))
per=collections.Counter()
for c,_,_,_ in uniq: per[c]+=1
print("  per course:", dict(sorted(per.items())))

print("\n=== MARGINAL YIELD, measured over the bank in landing order ===")
# landing order approximated by course-campaign order then seq
ORDER={"C01":0,"C02":1,"C03":2,"C04":3,"C05":4}
d5=[L for L in bank if L.get("d")==5 and L.get("tib") and L.get("eng") and L.get("seg") is not None]
bysegs=collections.defaultdict(set)
for L in d5: bysegs[(L["course"],L["seg"])].add((L["tib"],L["eng"]))
segs=sorted(bysegs, key=lambda k:(ORDER.get(k[0],9),k[1]))
heads=set(); prs=set(); nh=[]; np_=[]
for k in segs:
    a=b=0
    for (t,e) in bysegs[k]:
        if t not in heads: heads.add(t); a+=1
        if (t,e) not in prs: prs.add((t,e)); b+=1
    nh.append(a); np_.append(b)
N=len(segs); D=N//10
print("segments in landing order: %d   final: %d headwords, %d pairs"%(N,len(heads),len(prs)))
print("decile | new headwords/seg | new (tib,eng) pairs/seg")
for i in range(10):
    lo,hi=i*D,(i+1)*D if i<9 else N
    print("   %2d   | %8.2f          | %8.2f"%(i+1,st.mean(nh[lo:hi]),st.mean(np_[lo:hi])))
print("first decile / last decile headword ratio: %.1fx"%(st.mean(nh[:D])/max(st.mean(nh[-D:]),1e-9)))
print("LAST DECILE (the forward rate): %.2f new headwords, %.2f new pairs, %.2f occurrences per segment"%(
    st.mean(nh[-D:]), st.mean(np_[-D:]), st.mean([len(bysegs[k]) for k in segs[-D:]])))
print("whole-bank averages: %.2f occurrences/segment"%(len(d5)/N))
