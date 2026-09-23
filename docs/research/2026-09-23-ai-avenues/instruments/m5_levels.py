import sqlite3, json, collections, re, math
REPO='/Users/adamderickandrade/ALL-translation-tool/'
db=sqlite3.connect(REPO+'build/hgm_spine_v27_2.db')
COURSES=['C%02d'%i for i in range(1,19)]
rows=db.execute("select course,seq,wylie,english from corpus_segments where course in (%s) order by course,seq"%','.join('?'*18), COURSES).fetchall()
allrows=db.execute("select course,seq,wylie,english from corpus_segments order by course,seq").fetchall()

SYL=re.compile(r"[a-zA-Z'èé+~]+")
def syls(w):
    if not w: return []
    # wylie syllables are space separated; strip punctuation , / . ; ( ) digits
    out=[]
    for tok in re.split(r"[\s]+", w):
        tok=tok.strip(",./;:()[]!?\"*-_0123456789")
        if not tok: continue
        out.append(tok)
    return out

def stats(rs, label):
    occ=0; types=collections.Counter()
    for c,s,w,e in rs:
        t=syls(w); occ+=len(t); types.update(t)
    print(f"{label}: segments={len(rs)} syllable-occurrences={occ} syllable-types={len(types)} ratio={occ/max(1,len(types)):.1f}")
    return occ, types

print("=== LEVEL 1: syllables (exact, mechanical) ===")
o_all,t_all = stats(allrows, "whole spine (75 courses)")
o_m, t_m   = stats(rows, "C01-C18 mandate")
o13,t13 = stats([r for r in rows if r[0]=='C13'], "  C13 (excluded)")
# hapax
hap=sum(1 for k,v in t_m.items() if v==1)
print(f"  C01-C18 hapax syllable types: {hap} ({100*hap/len(t_m):.1f}% of types, {100*hap/o_m:.2f}% of occurrences)")
top=t_m.most_common(20)
cum=sum(v for k,v in top)
print(f"  top-20 syllable types cover {100*cum/o_m:.1f}% of occurrences:", [k for k,v in top])
# coverage curve
sv=sorted(t_m.values(), reverse=True)
c=0
for frac in (0.001,0.01,0.05,0.1,0.25,0.5):
    n=max(1,int(len(sv)*frac)); print(f"  top {frac*100:g}% of types ({n}) cover {100*sum(sv[:n])/o_m:.1f}% of occurrences")
