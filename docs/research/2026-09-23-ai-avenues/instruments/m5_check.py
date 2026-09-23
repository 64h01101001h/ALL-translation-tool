import sqlite3, re, collections, json
REPO='/Users/adamderickandrade/ALL-translation-tool/'
db=sqlite3.connect(REPO+'build/hgm_spine_v27_2.db')
COURSES=['C%02d'%i for i in range(1,19)]
rows=db.execute("select course,seq,wylie,english from corpus_segments where course in (%s) order by course,seq"%','.join('?'*18),COURSES).fetchall()
bank=json.load(open(REPO+'data/alignment/alignment_full_v1.json'))
landed=set((x['course'],x['seg']) for x in bank['links'] if x['seg'] is not None)
man=[r for r in rows if r[0]!='C13']
def syls(w):
    o=[]
    for t in re.split(r"\s+",w or ''):
        t=t.strip(",./;:()[]!?\"*-_0123456789")
        if t: o.append(t)
    return o
def sh(w,n=4):
    s=syls(w); return set(tuple(s[i:i+n]) for i in range(max(0,len(s)-n+1))) or {tuple(s)}
proc=[r for r in man if (r[0],r[1]) in landed]
rem=[r for r in man if (r[0],r[1]) not in landed]
seen=set((r[2],r[3]) for r in proc); uniq=[]
for r in rem:
    k=(r[2],r[3])
    if k in seen: continue
    seen.add(k); uniq.append(r)
idx=collections.defaultdict(list); pool=[]
def add(r):
    s=sh(r[2]); pool.append((r,s))
    for x in list(s)[:200]: idx[x].append(len(pool)-1)
for r in proc: add(r)
hits=[]
for r in uniq:
    s=sh(r[2]); c=collections.Counter()
    for x in list(s)[:200]:
        for i in idx.get(x,()): c[i]+=1
    best=0; bi=None
    for i,_ in c.most_common(30):
        o=pool[i][1]; j=len(s&o)/max(1,len(s|o))
        if j>best: best=j; bi=i
    if best>=0.9: hits.append((best,r,pool[bi][0]))
    add(r)
print('near-dup >=0.90 count', len(hits))
L=[len(syls(h[1][2])) for h in hits]
print('their Tibetan length in syllables: median %d, p10 %d, p90 %d ; share with <8 syllables: %.1f%%'%(
   sorted(L)[len(L)//2], sorted(L)[len(L)//10], sorted(L)[9*len(L)//10], 100*sum(1 for x in L if x<8)/len(L)))
print('\nthree examples:')
for j,(b,a,c) in enumerate(hits[:3]):
    print('\n--- J=%.3f  candidate %s:%d   vs already-read %s:%d'%(b,a[0],a[1],c[0],c[1]))
    print('  CAND tib : %s'%(a[2] or '')[:170])
    print('  PRIOR tib: %s'%(c[2] or '')[:170])
    print('  CAND eng : %s'%(a[3] or '')[:170])
    print('  PRIOR eng: %s'%(c[3] or '')[:170])
# how many of the >=0.90 differ ONLY in english?
same_t=sum(1 for b,a,c in hits if a[2]==c[2])
print('\nof the %d near-dups, %d (%.1f%%) have BYTE-IDENTICAL Tibetan and differ only in the English'%(len(hits),same_t,100*same_t/len(hits)))
