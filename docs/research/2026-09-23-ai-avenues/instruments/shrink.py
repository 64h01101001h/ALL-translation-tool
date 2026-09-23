import sys,json,sqlite3,statistics
ROOT="/Users/adamderickandrade/ALL-translation-tool"
sys.path.insert(0,ROOT+"/tools")
import build_precedent_index as b
from gen_alignment_page import find_word
idx=b.load_index(); bk=b.buckets_of(idx)
db=sqlite3.connect(b.SPINE)
for course in ("C06","C16"):
    seqs=[r[0] for r in db.execute("select seq from corpus_segments where course=? order by seq",(course,))][:200]
    now=[];filt=[];hw=[]
    for q in seqs:
        ex=b.extract(idx,bk,b.SPINE,course,q)
        if "error" in ex: continue
        E=db.execute("select english from corpus_segments where course=? and seq=?",(course,q)).fetchone()[0] or ""
        n=f=0
        for e in ex["entries"]:
            r=e.get("eng",[])
            n+=min(6,len(r)); f+=sum(1 for x,_ in r if find_word(E,x,0)>=0)
        now.append(n); filt.append(f); hw.append(ex["headwords"])
    print(course,"segments",len(now),"headwords/seg median",statistics.median(hw),
          "renderings shown now median",statistics.median(now),"mean",round(statistics.mean(now),1),
          "| present-in-English median",statistics.median(filt),"mean",round(statistics.mean(filt),1))
