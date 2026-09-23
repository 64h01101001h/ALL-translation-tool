import sqlite3, re, collections, json, math, random
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
def shingles(w,n=4):
    s=syls(w)
    return set(tuple(s[i:i+n]) for i in range(max(0,len(s)-n+1))) or {tuple(s)}
# processed pool = landed; candidate = remaining after exact dedupe
proc=[r for r in man if (r[0],r[1]) in landed]
rem=[r for r in man if (r[0],r[1]) not in landed]
seenk=set((r[2],r[3]) for r in proc); uniq=[]
exact=0
for r in rem:
    k=(r[2],r[3])
    if k in seenk: exact+=1; continue
    seenk.add(k); uniq.append(r)
print('landed %d ; remaining %d ; exact dupes %d ; unique %d'%(len(proc),len(rem),exact,len(uniq)))
# inverted index of shingles over processed + accepted-unique, streaming
idx=collections.defaultdict(list)
pool=[]
def add(r):
    sh=shingles(r[2]); pool.append((r,sh))
    for s in list(sh)[:200]: idx[s].append(len(pool)-1)
for r in proc: add(r)
buckets=collections.Counter(); costsave=0.0; tot=0.0
detail=[]
def COST(r): return 177210.0+575.2*(len((r[2] or '').split())+len((r[3] or '').split()))
for r in uniq:
    sh=shingles(r[2])
    cand=collections.Counter()
    for s in list(sh)[:200]:
        for i in idx.get(s,()): cand[i]+=1
    best=0.0
    for i,c in cand.most_common(30):
        o=pool[i][1]
        j=len(sh&o)/max(1,len(sh|o))
        if j>best: best=j
    tot+=COST(r)
    if best>=0.9: buckets['>=0.90']+=1; costsave+=0.8*COST(r)
    elif best>=0.7: buckets['0.70-0.90']+=1; costsave+=0.5*COST(r)
    elif best>=0.5: buckets['0.50-0.70']+=1; costsave+=0.25*COST(r)
    else: buckets['<0.50']+=1
    detail.append(best)
    add(r)
print('\nnear-duplicate structure of the 4,073 unique remaining (max Jaccard on 4-syllable shingles vs everything already accepted):')
for k in ('>=0.90','0.70-0.90','0.50-0.70','<0.50'):
    print('  %-10s %5d  (%.1f%%)'%(k,buckets[k],100*buckets[k]/len(uniq)))
print('\nmedian max-Jaccard %.3f ; mean %.3f'%(sorted(detail)[len(detail)//2], sum(detail)/len(detail)))
print('illustrative cost saving if near-duplicates are aligned by DIFF rather than from scratch')
print('  (discounts 80%%/50%%/25%% by band -- an ASSUMPTION, not a measurement): %.1f%% of the remaining budget'%(100*costsave/tot))
print('  exact-duplicate copy-forward alone already removes %d of %d remaining segments = %.1f%%'%(exact,len(rem),100*exact/len(rem)))
