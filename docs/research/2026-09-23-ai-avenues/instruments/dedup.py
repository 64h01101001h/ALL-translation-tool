import json,sqlite3,re
from collections import defaultdict
R='/Users/adamderickandrade/ALL-translation-tool'
con=sqlite3.connect(R+'/build/hgm_spine_v27_2.db')
L=json.load(open(R+'/data/alignment/alignment_full_v1.json'))['links']
bank=defaultdict(list)
for l in L:
    if l['course']!='C05': continue
    s=l.get('seg')
    if s is not None and l.get('tib'): bank[s].append(l)
rows={r[0]:(r[1] or '',r[2] or '') for r in con.execute("select seq,wylie,english from corpus_segments where course='C05'")}
def toks(s): return set(re.findall(r"[a-zA-Z']+",s.lower()))
segs=sorted(bank)
pairs=[]
for i,a in enumerate(segs):
    ta=toks(rows[a][0]+' '+rows[a][1])
    if not ta: continue
    for b in segs[i+1:]:
        tb=toks(rows[b][0]+' '+rows[b][1])
        if not tb: continue
        j=len(ta&tb)/len(ta|tb)
        if j>=0.80: pairs.append((a,b,j))
print('C05 landed near-duplicate pairs at Jaccard>=0.80:',len(pairs))
exact=sum(1 for a,b,j in pairs if rows[a]==rows[b])
print('  of which byte-identical (wylie,english):',exact)
ok=bad=0; badex=[]
for a,b,j in pairs:
    if rows[a]==rows[b]: continue
    wb,eb=rows[b]
    for l in bank[a]:
        t=l.get('tib') or ''; e=l.get('eng')
        tv = t in wb
        ev = (e is None) or (e in eb)
        if tv and ev: ok+=1
        else:
            bad+=1
            if len(badex)<8: badex.append((a,b,round(j,3),t[:40],(e or '')[:40],tv,ev))
tot=ok+bad
print('transferring spans from one near-duplicate to its twin (non-identical pairs only):')
print('  spans tested %d | verbatim in twin %d (%.1f%%) | WOULD BE REFUSED/FALSE %d (%.1f%%)'%(tot,ok,100*ok/tot if tot else 0,bad,100*bad/tot if tot else 0))
for x in badex: print('   C05:%s->C05:%s j=%s tib=%r eng=%r tib_ok=%s eng_ok=%s'%x)
