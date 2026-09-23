import json,sqlite3,statistics,time
from collections import defaultdict,Counter
R='/Users/adamderickandrade/ALL-translation-tool'
d=json.load(open(R+'/data/alignment/alignment_full_v1.json'))
L=d['links']
# index from C01-C04 only  (leave-one-COURSE-out on C05)
idx=defaultdict(set)
for l in L:
    if l['course']=='C05': continue
    if l.get('d')!=5: continue
    t,e=l.get('tib'),l.get('eng')
    if not t or not e: continue
    idx[t.strip()].add(e.strip())
print('index headwords (C01-C04, d=5):',len(idx),'pairs:',sum(len(v) for v in idx.values()))

con=sqlite3.connect(R+'/build/hgm_spine_v27_2.db')
seg={r[0]:(r[1] or '',r[2] or '') for r in con.execute("select seq,wylie,english from corpus_segments where course='C05'")}

# banked C05 d=5 with eng, grouped by seq
bank=defaultdict(set); bankhw=defaultdict(set)
for l in L:
    if l['course']!='C05' or l.get('d')!=5: continue
    s=l.get('seg'); t=l.get('tib'); e=l.get('eng')
    if s is None or not t: continue
    if e:
        bank[s].add((t.strip(),e.strip()))
    bankhw[s].add(t.strip())
segs=sorted(bank)
print('landed C05 segments with d5+eng:',len(segs))

def boundary_hit(hay,needle):
    # syllable-boundary occurrence in wylie: delimiters are space and tsheg-ish punctuation
    i=hay.find(needle)
    while i>=0:
        pre = hay[i-1] if i>0 else ' '
        post = hay[i+len(needle)] if i+len(needle)<len(hay) else ' '
        if pre in ' ,.\n\t/;(' and post in ' ,.\n\t/;)':
            return True
        i=hay.find(needle,i+1)
    return False

t0=time.time()
rec_pair=0; tot_pair=0; rec_hw=0; tot_hw=0
cand_counts=[]; uniq_agree=0; uniq_tot=0
miss_share=[]
for s in segs:
    w,en=seg.get(s,('',''))
    if not w: continue
    cands=defaultdict(set)
    for t,es in idx.items():
        if len(t)<2: continue
        if not boundary_hit(w,t): continue
        for e in es:
            if e and e in en: cands[t].add(e)
    cands={k:v for k,v in cands.items() if v}
    n=sum(len(v) for v in cands.values()); cand_counts.append(n)
    bp=bank[s]
    tot_pair+=len(bp); rec_pair+=sum(1 for (t,e) in bp if e in cands.get(t,()))
    bh={t for (t,e) in bp}
    tot_hw+=len(bh); rec_hw+=sum(1 for t in bh if t in cands)
    bmap=defaultdict(set)
    for (t,e) in bp: bmap[t].add(e)
    for t,es in cands.items():
        if len(es)==1 and t in bmap:
            uniq_tot+=1
            if list(es)[0] in bmap[t]: uniq_agree+=1
print('elapsed %.2fs for %d segments (%.1f ms/seg)'%(time.time()-t0,len(segs),1000*(time.time()-t0)/len(segs)))
print('HEADWORD recall: %d/%d = %.1f%%'%(rec_hw,tot_hw,100*rec_hw/tot_hw))
print('PAIR recall: %d/%d = %.1f%%'%(rec_pair,tot_pair,100*rec_pair/tot_pair))
print('candidates/segment mean %.1f median %.0f p90 %.0f max %d total %d'%(statistics.mean(cand_counts),statistics.median(cand_counts),sorted(cand_counts)[int(.9*len(cand_counts))],max(cand_counts),sum(cand_counts)))
print('banked pairs/segment mean %.1f'%(tot_pair/len(segs)))
print('over-generation %.1fx'%(statistics.mean(cand_counts)/(tot_pair/len(segs))))
print('UNIQUE-candidate agreement: %d/%d = %.1f%%'%(uniq_agree,uniq_tot,100*uniq_agree/uniq_tot if uniq_tot else 0))
