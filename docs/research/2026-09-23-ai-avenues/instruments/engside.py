import json,collections,re,sqlite3
ROOT="/Users/adamderickandrade/ALL-translation-tool"
L=json.load(open(ROOT+"/data/alignment/alignment_full_v1.json"))["links"]
db=sqlite3.connect(ROOT+"/build/hgm_spine_v27_2.db")
en={(c,s):(e or "") for c,s,e in db.execute("select course,seq,english from corpus_segments")}
ORDER=["C01","C02","C03","C04","C05"]
ART={"the","a","an"}
def nE(e):
    w=re.findall(r"[a-z0-9']+",e.lower())
    while w and w[0] in ART: w=w[1:]
    return " ".join(w)
for T,prior in (("C05",set(ORDER[:4])),("C04",set(ORDER[:3]))):
    pool=[l for l in L if l["course"] in prior]
    tg=[l for l in L if l["course"]==T and l["d"]==5 and l["seg"] and l["eng"] is not None]
    rend=collections.defaultdict(set)   # eng -> tibs
    byt=collections.defaultdict(set)    # tib -> normalized engs
    for l in pool:
        if l["eng"] is not None:
            rend[l["eng"]].add(l["tib"]); byt[l["tib"]].add(nE(l["eng"]))
    heads={l["tib"] for l in pool}
    exact_eng=sum(1 for l in tg if l["eng"] in rend)
    exact_eng_right=sum(1 for l in tg if l["tib"] in rend.get(l["eng"],()))
    novel=[l for l in tg if l["tib"] in heads and l["tib"] not in rend.get(l["eng"],set())]
    novel_norm=sum(1 for l in novel if nE(l["eng"]) in byt[l["tib"]])
    print(T,"rendered d5 links",len(tg))
    print("  eng string attested anywhere in pool: %d (%.1f%%)"%(exact_eng,100*exact_eng/len(tg)))
    print("  (tib,eng) pair attested: %d (%.1f%%)"%(exact_eng_right,100*exact_eng_right/len(tg)))
    print("  novel rendering for known head: %d; of these equal after case/article/punct fold: %d (%.1f%%)"%(len(novel),novel_norm,100*novel_norm/max(1,len(novel))))
    # English-side extract noise: for 50 target segments, how many pool renderings (len>=4 chars) occur in the segment english
    segs=sorted({(l["course"],l["seg"]) for l in tg})[:200]
    multi=[e for e in rend if len(e)>=4]
    import statistics
    sizes=[]
    for k in segs:
        E=en.get(k,"")
        sizes.append(sum(1 for e in multi if e in E))
    print("  english-side candidates per segment (renderings >=4 chars found verbatim in segment english): median",statistics.median(sizes),"p90",sorted(sizes)[int(.9*len(sizes))])
