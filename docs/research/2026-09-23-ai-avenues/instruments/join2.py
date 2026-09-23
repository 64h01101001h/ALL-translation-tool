import json,sqlite3,statistics,time
from collections import defaultdict,Counter
R='/Users/adamderickandrade/ALL-translation-tool'
L=json.load(open(R+'/data/alignment/alignment_full_v1.json'))['links']
con=sqlite3.connect(R+'/build/hgm_spine_v27_2.db')

idx=defaultdict(set); src=defaultdict(set); tier_of=defaultdict(set)
for l in L:
    if l['course']=='C05' or l.get('d')!=5: continue
    t,e=l.get('tib'),l.get('eng')
    if t and e: idx[t.strip()].add(e.strip()); src[(t.strip(),e.strip())].add('bank')
ndict=0
for w,g,tier in con.execute("select wylie,hgm_gloss,tier from entries where hgm_gloss is not null and hgm_gloss!=''"):
    try: gl=json.loads(g)
    except: continue
    for e in gl:
        if not isinstance(e,str) or not e.strip(): continue
        idx[w.strip()].add(e.strip()); src[(w.strip(),e.strip())].add('dict'); tier_of[(w.strip(),e.strip())].add(tier); ndict+=1
print('index headwords:',len(idx),'pairs:',sum(len(v) for v in idx.values()),'(dict contributions %d)'%ndict)

seg={r[0]:(r[1] or '',r[2] or '') for r in con.execute("select seq,wylie,english from corpus_segments where course='C05'")}
bank=defaultdict(set)
for l in L:
    if l['course']!='C05' or l.get('d')!=5: continue
    s,t,e=l.get('seg'),l.get('tib'),l.get('eng')
    if s is None or not t or not e: continue
    bank[s].add((t.strip(),e.strip()))
segs=sorted(bank)

def bh(hay,n):
    i=hay.find(n)
    while i>=0:
        pre=hay[i-1] if i else ' '; post=hay[i+len(n)] if i+len(n)<len(hay) else ' '
        if pre in ' ,.\n\t/;(' and post in ' ,.\n\t/;)': return True
        i=hay.find(n,i+1)
    return False

FN={'the','a','an','and','or','his','our','your','i','you','of','to','in','is','that','it','this','be','as','are','for','not','but','on','with','they','we','he','she','all','so','if','was','by','from','at','have','has','one','no','do','there','their','them','其'}
t0=time.time()
rp=tp=rh=th=0; cc=[]; ua=ut=0; fnc=0; totc=0; dictonly=0; tiers=Counter()
for s in segs:
    w,en=seg.get(s,('',''))
    if not w: continue
    cands=defaultdict(set)
    for t,es in idx.items():
        if len(t)<2 or not bh(w,t): continue
        for e in es:
            if e and e in en: cands[t].add(e)
    cands={k:v for k,v in cands.items() if v}
    n=sum(len(v) for v in cands.values()); cc.append(n); totc+=n
    for t,es in cands.items():
        for e in es:
            if e.strip().lower() in FN: fnc+=1
            if src.get((t,e))=={'dict'}:
                dictonly+=1
                for tt in tier_of.get((t,e),()): tiers[tt]+=1
    bp=bank[s]; tp+=len(bp); rp+=sum(1 for (t,e) in bp if e in cands.get(t,()))
    bh_=set(t for t,_ in bp); th+=len(bh_); rh+=sum(1 for t in bh_ if t in cands)
    bm=defaultdict(set)
    for (t,e) in bp: bm[t].add(e)
    for t,es in cands.items():
        if len(es)==1 and t in bm:
            ut+=1; ua+= (list(es)[0] in bm[t])
el=time.time()-t0
print('%.2fs / %d segs = %.1f ms/seg'%(el,len(segs),1000*el/len(segs)))
print('HEADWORD recall %.1f%%  PAIR recall %.1f%%'%(100*rh/th,100*rp/tp))
print('cands/seg mean %.1f median %.0f p90 %.0f max %d TOTAL %d'%(statistics.mean(cc),statistics.median(cc),sorted(cc)[int(.9*len(cc))],max(cc),totc))
print('over-generation %.1fx (banked %.1f/seg)'%(statistics.mean(cc)/(tp/len(segs)), tp/len(segs)))
print('UNIQUE-candidate agreement %d/%d = %.1f%%'%(ua,ut,100*ua/ut))
print('candidates whose ENGLISH is a bare function word: %d/%d = %.1f%%'%(fnc,totc,100*fnc/totc))
print('candidates sourced ONLY from the dictionary (not bank): %d = %.1f%%'%(dictonly,100*dictonly/totc))
print('their tiers:',tiers.most_common())
