import json,sqlite3,re,time
from collections import defaultdict
R='/Users/adamderickandrade/ALL-translation-tool'
con=sqlite3.connect(R+'/build/hgm_spine_v27_2.db')
L=json.load(open(R+'/data/alignment/alignment_full_v1.json'))['links']
bank=defaultdict(list)
for l in L:
    s=l.get('seg')
    if s is not None and l.get('tib'): bank[(l['course'],s)].append(l)
rows={}
for c,s,w,e in con.execute("select course,seq,wylie,english from corpus_segments where course in ('C01','C02','C03','C04','C05')"):
    rows[(c,s)]=(w or '',e or '')
keys=[k for k in bank if k in rows]
print('landed segments with bank + spine text:',len(keys))
def toks(s): return frozenset(re.findall(r"[a-zA-Z']+",s.lower()))
T={k:toks(rows[k][0]+' '+rows[k][1]) for k in keys}
# rare-token blocking
inv=defaultdict(list)
for k in keys:
    for t in T[k]: inv[t].append(k)
cand=set()
for t,ks in inv.items():
    if len(ks)>60: continue
    for i in range(len(ks)):
        for j in range(i+1,len(ks)): cand.add((ks[i],ks[j]))
print('blocked candidate pairs:',len(cand))
pairs=[]
for a,b in cand:
    ta,tb=T[a],T[b]
    if not ta or not tb: continue
    u=len(ta|tb)
    if u==0: continue
    j=len(ta&tb)/u
    if j>=0.80: pairs.append((a,b,j))
print('near-duplicate pairs Jaccard>=0.80 among LANDED:',len(pairs))
idp=sum(1 for a,b,j in pairs if rows[a]==rows[b])
print('  byte-identical:',idp,'  non-identical:',len(pairs)-idp)
ok=bad=0; ex=[]
for a,b,j in pairs:
    if rows[a]==rows[b]: continue
    wb,eb=rows[b]
    for l in bank[a]:
        t=l.get('tib') or ''; e=l.get('eng')
        tv=t in wb; ev=(e is None) or (e in eb)
        if tv and ev: ok+=1
        else:
            bad+=1
            if len(ex)<10: ex.append((a,b,round(j,3),t[:35],(e or '')[:35],tv,ev))
tot=ok+bad
print('SPAN TRANSFER TEST (copy A\'s alignment onto its >=0.80 twin B):')
print('  spans tested %d | still verbatim in twin %d (%.1f%%) | NOT VERBATIM %d (%.1f%%)'%(tot,ok,100*ok/tot if tot else 0,bad,100*bad/tot if tot else 0))
for x in ex: print('   %s -> %s j=%s tib=%r eng=%r tib_ok=%s eng_ok=%s'%x)
