exec(open('neardup.py').read().split("for thr in")[0])
import difflib
cat=collections.Counter(); spanfrac=[]
# how many of a landed partner's banked d5 spans would still be verbatim in the new segment?
bylk=collections.defaultdict(list)
for l in L:
    if l["seg"]: bylk[(l["course"],l["seg"])].append(l)
import sys; sys.path.insert(0,ROOT+"/tools")
from gen_alignment_page import find_word
keep_tot=0; span_tot=0; nseg=0
for k,(jt,je,o) in best.items():
    if not o or min(jt,je)<0.8: continue
    cat[("T=" if jt==1 else "T~")+("E=" if je==1 else "E~")]+=1
    if o in landed:
        w,e=seg[k]; nseg+=1
        for l in bylk[o]:
            if l["d"]!=5: continue
            span_tot+=1
            if not l["tib"] or (l["eng"] is not None and not l["eng"]): continue
            ok = find_word(w,l["tib"],0)>=0 and (l["eng"] is None or find_word(e,l["eng"],0)>=0)
            keep_tot+=ok
print(dict(cat))
print("landed-partner near-dups:",nseg,"partner d5 spans:",span_tot,"still verbatim on both sides in the new segment: %d (%.1f%%)"%(keep_tot,100*keep_tot/max(1,span_tot)))
