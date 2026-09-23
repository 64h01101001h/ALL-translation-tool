import json,sqlite3
from collections import defaultdict,Counter
R='/Users/adamderickandrade/ALL-translation-tool'
L=json.load(open(R+'/data/alignment/alignment_full_v1.json'))['links']
con=sqlite3.connect(R+'/build/hgm_spine_v27_2.db')
idx=defaultdict(set); src=defaultdict(set)
for l in L:
    if l['course']=='C05' or l.get('d')!=5: continue
    t,e=l.get('tib'),l.get('eng')
    if t and e: idx[t.strip()].add(e.strip()); src[(t.strip(),e.strip())].add('bank')
for w,g in con.execute("select wylie,hgm_gloss from entries where hgm_gloss is not null and hgm_gloss!=''"):
    try: gl=json.loads(g)
    except: continue
    for e in gl:
        if isinstance(e,str) and e.strip(): idx[w.strip()].add(e.strip()); src[(w.strip(),e.strip())].add('dict')
seg={r[0]:(r[1] or '',r[2] or '') for r in con.execute("select seq,wylie,english from corpus_segments where course='C05'")}
bank=defaultdict(set)
for l in L:
    if l['course']!='C05' or l.get('d')!=5: continue
    s,t,e=l.get('seg'),l.get('tib'),l.get('eng')
    if s is not None and t and e: bank[s].add((t.strip(),e.strip()))
def bh(h,n):
    i=h.find(n)
    while i>=0:
        pre=h[i-1] if i else ' '; post=h[i+len(n)] if i+len(n)<len(h) else ' '
        if pre in ' ,.\n\t/;(' and post in ' ,.\n\t/;)': return True
        i=h.find(n,i+1)
    return False
GATE={'the','a','an','and','or','his','our','your','i','you'}   # exactly what test_no_supplied_span_head catches
OTHER={'of','to','in','on','at','by','for','with','from','is','are','was','were','be','been','it','its','that','this','these','those','as','but','if','so','not','no','who','which','when','then','there','their','them','they','we','he','she','him','her','us','me','my','all','one','into','upon','through','out','up','do','does','did','have','has','had','will','would','can','could','may','might','shall','should','must','than','because','while','also','very','such','more','most','own','same','about','over','under','after','before','any','some','each','every','both','other','only','even','still','yet'}
tot=0; gate=0; other=0; dictonly=0; long_=0
ex=Counter()
for s in sorted(bank):
    w,en=seg.get(s,('',''))
    if not w: continue
    for t,es in idx.items():
        if len(t)<2 or not bh(w,t): continue
        for e in es:
            if not e or e not in en: continue
            tot+=1
            lw=e.strip().lower()
            first=lw.split()[0] if lw.split() else ''
            if lw in GATE: gate+=1; ex[(t,e)]+=1
            elif lw in OTHER: other+=1
            elif first in GATE or first in OTHER: long_+=1
            if src.get((t,e))=={'dict'}: dictonly+=1
print('total candidate pairs (343 C05 segs):',tot)
print('English is a bare word the span-head GATE would catch: %d (%.1f%%)'%(gate,100*gate/tot))
print('English is a bare function word the gate would NOT catch: %d (%.1f%%)'%(other,100*other/tot))
print('English is multiword but STARTS with a function word: %d (%.1f%%)'%(long_,100*long_/tot))
print('=> function-word-contaminated candidates: %.1f%%'%(100*(gate+other+long_)/tot))
print('dictionary-only provenance: %.1f%%'%(100*dictonly/tot))
print('\ntop gate-catchable candidate pairs offered (tib -> eng, times offered):')
for (t,e),n in ex.most_common(15): print('   %-18s -> %-6s  x%d'%(t,e,n))
