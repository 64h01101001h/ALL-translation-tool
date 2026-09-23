import json,collections,re,sqlite3,sys
ROOT="/Users/adamderickandrade/ALL-translation-tool"
sys.path.insert(0,ROOT+"/tools")
from gen_alignment_page import find_word
L=json.load(open(ROOT+"/data/alignment/alignment_full_v1.json"))["links"]
db=sqlite3.connect(ROOT+"/build/hgm_spine_v27_2.db")
en={(c,s):(e or "") for c,s,e in db.execute("select course,seq,english from corpus_segments")}
ORDER=["C01","C02","C03","C04","C05"]
for T,prior in (("C05",set(ORDER[:4])),("C04",set(ORDER[:3])),("C03",set(ORDER[:2]))):
    pool=[l for l in L if l["course"] in prior]
    eng=collections.defaultdict(collections.Counter)
    for l in pool:
        if l["eng"] is not None: eng[l["tib"]][l["eng"]]+=1
    tg=[l for l in L if l["course"]==T and l["d"]==5 and l["seg"] and l["eng"] is not None and l["eng"] in eng.get(l["tib"],{})]
    hits={"base":collections.Counter(),"inseg":collections.Counter()}
    nren={"base":[], "inseg":[]}
    for l in tg:
        E=en[(l["course"],l["seg"])]
        base=[x for x,_ in sorted(eng[l["tib"]].items(),key=lambda kv:(-kv[1],kv[0]))]
        present=[x for x in base if find_word(E,x,0)>=0]
        inseg=present   # renderings absent from this segment's English cannot be banked here
        for name,lst in (("base",base),("inseg",inseg)):
            nren[name].append(len(lst))
            if l["eng"] in lst:
                r=lst.index(l["eng"])+1
                for k in (1,3,6):
                    if r<=k: hits[name][k]+=1
    n=len(tg)
    import statistics
    for name in ("base","inseg"):
        print(T,name,"n=%d"%n," ".join("R@%d %.1f%%"%(k,100*hits[name][k]/n) for k in (1,3,6)),
              "mean renderings shown/headword %.2f"%statistics.mean(nren[name]))
