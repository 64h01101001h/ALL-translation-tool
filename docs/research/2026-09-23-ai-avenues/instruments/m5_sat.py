import sqlite3, json, collections, re, pickle, math, random
REPO='/Users/adamderickandrade/ALL-translation-tool/'
db=sqlite3.connect(REPO+'build/hgm_spine_v27_2.db')
attested=pickle.load(open('/private/tmp/claude-501/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2/scratchpad/lex2.pkl','rb'))['attested']
def syls(w):
    out=[]
    for tok in re.split(r"\s+", w or ''):
        tok=tok.strip(",./;:()[]!?\"*-_0123456789")
        if tok: out.append(tok)
    return out
def munch(ss, maxl=2):
    i=0; out=[]
    while i<len(ss):
        for l in range(min(maxl,len(ss)-i),0,-1):
            c=tuple(ss[i:i+l])
            if l==1 or c in attested: out.append(' '.join(c)); i+=l; break
    return out
COURSES=['C%02d'%i for i in range(1,19)]
rows=db.execute("select course,seq,wylie,english from corpus_segments where course in (%s) order by course,seq"%','.join('?'*18),COURSES).fetchall()
rows=[r for r in rows if r[0]!='C13']
print('mandate less C13:', len(rows))

def curve(items, keyfn, label):
    seen=set(); pts=[]; 
    for i,it in enumerate(items,1):
        seen.update(keyfn(it)); pts.append(len(seen))
    N=len(items)
    marks=[int(N*f) for f in (0.05,0.1,0.25,0.5,0.75,1.0)]
    print(f"{label}: final types={pts[-1]}")
    for m in marks:
        print(f"   after {m:5d} segments ({100*m/N:4.0f}%): {pts[m-1]:6d} types  ({100*pts[m-1]/pts[-1]:.1f}% of final)")
    # Heaps beta from first-half vs full
    n1,n2=N//4,N
    b=math.log(pts[n2-1]/pts[n1-1])/math.log(n2/n1)
    print(f"   Heaps beta (quarter->full) = {b:.3f}")
    # marginal in deciles
    dec=[]
    for d in range(10):
        a=int(N*d/10); z=int(N*(d+1)/10)
        new=pts[z-1]-(pts[a-1] if a>0 else 0)
        dec.append(round(new/(z-a),2))
    print("   new types per segment by decile:", dec)
    return pts

print("\n=== A. Tibetan-side saturation over C01-C18 (less C13), corpus order ===")
p_syl=curve(rows, lambda r: set(syls(r[2])), "syllable types")
p_wrd=curve(rows, lambda r: set(munch(syls(r[2]))), "munched word types (segmenter F1~55)")

print("\n=== A2. English-side saturation (free: GMR's English tokens) ===")
def etok(e): return set(re.findall(r"[a-z']+", (e or '').lower()))
p_eng=curve(rows, lambda r: etok(r[3]), "English word types")
