import json, collections, sqlite3, re, statistics
R='/Users/adamderickandrade/ALL-translation-tool/'
L=json.load(open(R+'data/alignment/alignment_full_v1.json'))['links']
con=sqlite3.connect(R+'build/hgm_spine_v27_2.db')
seg={}
for c,s,w,e in con.execute("select course,seq,wylie,english from corpus_segments where course in ('C01','C02','C03','C04','C05')"):
    seg[(c,s)]=(w or '', e or '')
bank=collections.defaultdict(collections.Counter)   # tib -> Counter(eng)
for l in L:
    if l['course']!='C05' and l['d'] in (5,7) and l.get('eng'):
        bank[l['tib']][l['eng']]+=1
print('bank tib headwords (C01-C04 d5/7):', len(bank))

def occurs_tib(needle, hay):
    # syllable-boundary-ish: bounded by start/end or a non-letter
    i=hay.find(needle)
    while i>=0:
        a = i==0 or not hay[i-1].isalnum()
        b = i+len(needle)==len(hay) or not hay[i+len(needle)].isalnum()
        if a and b: return True
        i=hay.find(needle,i+1)
    return False
def occurs_eng(needle, hay):
    i=hay.find(needle)
    while i>=0:
        a = i==0 or not hay[i-1].isalpha()
        b = i+len(needle)==len(hay) or not hay[i+len(needle)].isalpha()
        if a and b: return True
        i=hay.find(needle,i+1)
    return False

c5=collections.defaultdict(set)
for l in L:
    if l['course']=='C05' and l['d'] in (5,7) and l.get('eng'):
        c5[l['seg']].add((l['tib'],l['eng']))
segs=sorted(c5)[:120]     # sample for speed
tp=fp=fn=0; cands=[]; per=[]
for s in segs:
    w,e = seg.get(('C05',s),('',''))
    if not w: continue
    cand=set()
    for tib, engs in bank.items():
        if len(tib)<2: continue
        if not occurs_tib(tib,w): continue
        for en in engs:
            if occurs_eng(en,e): cand.add((tib,en))
    truth=c5[s]
    t=len(cand&truth); tp+=t; fp+=len(cand-truth); fn+=len(truth-cand)
    cands.append(len(cand)); per.append((len(truth),t,len(cand)))
print(f'\nMECHANICAL PROPOSER (banked pair whose Tibetan and English BOTH occur in the segment)')
print(f'sample: {len(segs)} landed C05 segments')
print(f'  true landed links recovered (recall of the repeat class): {tp} of {tp+fn} = {100*tp/(tp+fn):.1f}%')
print(f'  candidates generated: {tp+fp}; precision {100*tp/(tp+fp):.1f}%')
print(f'  candidates per segment: median {statistics.median(cands):.0f}, mean {statistics.mean(cands):.0f}')
print(f'  true links per segment: mean {statistics.mean([a for a,b,c in per]):.1f}')
print(f'  false candidates per true one: {fp/max(tp,1):.1f}')
