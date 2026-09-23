import json,collections,re,sqlite3,sys
ROOT="/Users/adamderickandrade/ALL-translation-tool"
sys.path.insert(0,ROOT+"/tools")
from gen_alignment_page import find_word
L=json.load(open(ROOT+"/data/alignment/alignment_full_v1.json"))["links"]
db=sqlite3.connect(ROOT+"/build/hgm_spine_v27_2.db")
en={(c,s):(e or "") for c,s,e in db.execute("select course,seq,english from corpus_segments")}
ORDER=["C01","C02","C03","C04","C05"]
for T,prior in (("C05",set(ORDER[:4])),("C04",set(ORDER[:3]))):
    pool=[l for l in L if l["course"] in prior]
    eng=collections.defaultdict(collections.Counter); nul=collections.Counter()
    for l in pool:
        if l["eng"] is None: nul[l["tib"]]+=1
        else: eng[l["tib"]][l["eng"]]+=1
    heads=set(eng)|set(nul)
    tg=[l for l in L if l["course"]==T and l["d"]==5 and l["seg"] and l["tib"] in heads]
    c=collections.Counter()
    for l in tg:
        E=en[(l["course"],l["seg"])]
        present=[x for x in eng.get(l["tib"],{}) if find_word(E,x,0)>=0]
        if l["eng"] is None:
            c["null_answer"]+=1
            if present: c["null_but_attested_rendering_present"]+=1
        elif l["eng"] in eng.get(l["tib"],{}):
            c["attested"]+=1
            if present and present!=[l["eng"]] and sorted(present,key=lambda x:-eng[l["tib"]][x])[0]!=l["eng"]:
                c["attested_but_top_present_wrong"]+=1
        else:
            c["novel"]+=1
            if present: c["novel_but_other_attested_present"]+=1
    print(T,dict(c))
