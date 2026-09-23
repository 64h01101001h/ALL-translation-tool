import sqlite3, json, collections, re, pickle, random
REPO='/Users/adamderickandrade/ALL-translation-tool/'
db=sqlite3.connect(REPO+'build/hgm_spine_v27_2.db')
attested=pickle.load(open('/private/tmp/claude-501/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2/scratchpad/lex2.pkl','rb'))['attested']
def syls(w):
    out=[]
    for t in re.split(r"\s+", w or ''):
        t=t.strip(",./;:()[]!?\"*-_0123456789")
        if t: out.append(t)
    return out
def munch(ss):
    i=0;out=[]
    while i<len(ss):
        for l in (2,1):
            if i+l<=len(ss) and (l==1 or tuple(ss[i:i+l]) in attested):
                out.append(' '.join(ss[i:i+l])); i+=l; break
    return out
COURSES=['C%02d'%i for i in range(1,19)]
rows=db.execute("select course,seq,wylie,english from corpus_segments where course in (%s) order by course,seq"%','.join('?'*18),COURSES).fetchall()
bycourse=collections.defaultdict(list)
for r in rows: bycourse[r[0]].append(r)

print("=== marginal NEW word types per course, in course order (C13 excluded but its text NOT counted as read) ===")
seen=set(); prev=0
print(f"{'course':6} {'segs':>5} {'newtypes':>9} {'per seg':>8} {'cumulative':>10}")
for c in COURSES:
    if c=='C13': continue
    for r in bycourse[c]: seen.update(munch(syls(r[2])))
    new=len(seen)-prev; prev=len(seen)
    print(f"{c:6} {len(bycourse[c]):5d} {new:9d} {new/len(bycourse[c]):8.2f} {len(seen):10d}")

print("\n=== same, but ordered by descending marginal yield (greedy course selection) ===")
remaining=[c for c in COURSES if c!='C13']
seen=set(); order=[]
while remaining:
    best=None;bn=-1
    for c in remaining:
        s=set()
        for r in bycourse[c]: s.update(munch(syls(r[2])))
        n=len(s-seen)
        if n>bn: bn=n;best=c
    for r in bycourse[best]: seen.update(munch(syls(r[2])))
    order.append((best,len(bycourse[best]),bn,len(seen)))
    remaining.remove(best)
tot=len(seen); cs=0
for c,n,new,cum in order:
    cs+=n
    print(f"{c:6} segs={n:5d} new={new:6d} perseg={new/n:5.2f} cum_types={cum:6d} ({100*cum/tot:5.1f}%) cum_segs={cs}")
