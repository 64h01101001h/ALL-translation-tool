import json,sqlite3,collections,statistics,time
ROOT="/Users/adamderickandrade/ALL-translation-tool"
con=sqlite3.connect(ROOT+"/build/hgm_spine_v27_2.db")
C18=["C%02d"%i for i in range(1,19)]
q="select course,seq,wylie,english from corpus_segments"
allrows=[(c,s,w or "",e or "") for c,s,w,e in con.execute(q)]
bank=json.load(open(ROOT+"/data/alignment/alignment_full_v1.json"))["links"]
landed={(l["course"],l["seg"]) for l in bank if l.get("seg") is not None}
sub=[r for r in allrows if r[0] in C18]
rem=[r for r in sub if (r[0],r[1]) not in landed and r[0]!="C13"]
seen={(w,e) for c,s,w,e in sub if (c,s) in landed}; loc=set(); uniq=[]
for c,s,w,e in rem:
    if (w,e) in seen or (w,e) in loc: continue
    loc.add((w,e)); uniq.append((c,s,w,e))
def stats(rows,name):
    cw=sum(len(w) for _,_,w,_ in rows); ce=sum(len(e) for _,_,_,e in rows)
    syl=sum(len(w.split()) for _,_,w,_ in rows); ew=sum(len(e.split()) for _,_,_,e in rows)
    print("%-22s n=%6d  wylie %10d ch (%8d syl)  english %10d ch (%8d words)"%(name,len(rows),cw,syl,ce,ew))
    print("%-22s  per segment: wylie %6.0f ch / %5.1f syl ; english %6.0f ch / %5.1f words"%("",cw/len(rows),syl/len(rows),ce/len(rows),ew/len(rows)))
    # token estimate: 1 tok ~ 4 chars for English, ~3 chars for wylie (transliteration fragments badly)
    tk=cw/3.0+ce/4.0
    print("%-22s  ESTIMATED tokens (wylie/3 + eng/4): %,d total, %.0f per segment"%("",int(tk),tk/len(rows)) if False else
          "%-22s  ESTIMATED tokens (wylie/3 + eng/4): %d total, %.0f per segment"%("",int(tk),tk/len(rows)))
    return tk
t_all=stats(allrows,"WHOLE SPINE")
t_18=stats(sub,"C01-C18")
t_uniq=stats(uniq,"4,073 UNIQUE MANDATE")
t_landed=stats([r for r in sub if (r[0],r[1]) in landed],"already landed")
print()
print("ratio unique-mandate tokens / whole spine tokens: %.3f"%(t_uniq/t_all))
