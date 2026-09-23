import json, collections, sqlite3, re, statistics, math
R='/Users/adamderickandrade/ALL-translation-tool/'
ev=json.load(open(R+'data/alignment/alignment_evidence_v1.json'))
full=json.load(open(R+'data/alignment/alignment_full_v1.json'))
L=full['links']
out={}

# ---- population sizes at three units
pairs=ev['pairs']
hw=len(pairs)
recs=sum(len(v) for v in pairs.values())
occ=sum(r['n'] for v in pairs.values() for r in v)
out['evidence']={'headwords':hw,'records':recs,'occurrences':occ}

# full bank
out['bank']={'links':len(L),
 'nonnull_eng':sum(1 for x in L if x['eng'] is not None),
 'null_eng':sum(1 for x in L if x['eng'] is None),
 'distinct_pairs_nonnull':len({(x['tib'],x['eng']) for x in L if x['eng'] is not None}),
 'distinct_tib_nonnull':len({x['tib'] for x in L if x['eng'] is not None}),
 'depth':dict(collections.Counter(x['d'] for x in L)),
 'case_true':sum(1 for x in L if x['case']),
 'segments':len({(x['course'],x['seg']) for x in L}),
}
# depth x null
dn=collections.Counter((x['d'], x['eng'] is None) for x in L)
out['bank']['depth_x_null']={f'd{d}_{"null" if n else "gloss"}':c for (d,n),c in sorted(dn.items())}

# ---- polysemy at evidence layer
rend=collections.Counter()
for k,v in pairs.items(): rend[k]=len(v)
out['polysemy']={'headwords':hw,
  'hist':dict(collections.Counter(min(v,6) for v in rend.values())),
  'ge2':sum(1 for v in rend.values() if v>=2),
  'ge3':sum(1 for v in rend.values() if v>=3),
  'ge5':sum(1 for v in rend.values() if v>=5),
  'max':max(rend.values()),
  'top':[ (k,rend[k]) for k in sorted(rend,key=lambda k:-rend[k])[:12]]}

# reverse: english -> distinct tibetan (evidence layer)
eng2tib=collections.defaultdict(set)
for k,v in pairs.items():
    for r in v: eng2tib[r['eng'].strip().lower()].add(k)
out['reverse_polysemy']={'english_strings':len(eng2tib),
  'ge2':sum(1 for s in eng2tib.values() if len(s)>=2),
  'ge3':sum(1 for s in eng2tib.values() if len(s)>=3),
  'ge5':sum(1 for s in eng2tib.values() if len(s)>=5),
  'top':[(e,len(s)) for e,s in sorted(eng2tib.items(),key=lambda t:-len(t[1]))[:12]]}

# ---- compounds: d=5 immediately followed by d=7 members (bank order)
comp=0; members=collections.Counter(); i=0
parents=set()
while i < len(L):
    x=L[i]
    if x['d']==5:
        j=i+1; m=0
        while j<len(L) and L[j]['d']==7 and L[j]['page']==x['page'] and L[j]['seg']==x['seg']:
            m+=1; j+=1
        if m:
            comp+=1; members[m]+=1; parents.add((x['tib'],x['eng']))
        i=j if m else i+1
    else:
        i+=1
out['compounds']={'d5_with_members':comp,'member_count_hist':dict(sorted(members.items())),
  'distinct_compound_pairs':len(parents)}

# ---- syllable counts / length ratios at d=5 glossed
def syls(t):
    t=t.strip()
    return len([s for s in re.split(r'\s+',t) if s]) if t else 0
d5=[x for x in L if x['d']==5 and x['eng']]
rat=[]
for x in d5:
    s=syls(x['tib']); w=len([t for t in re.split(r'\s+',x['eng'].strip()) if t])
    if s: rat.append((w/s,s,w,x))
rr=[r[0] for r in rat]
rr.sort()
def q(p): return rr[int(p*(len(rr)-1))]
out['expansion']={'n_d5_glossed':len(d5),'ratio_eng_words_per_tib_syllable':{
   'p10':round(q(.10),3),'p25':round(q(.25),3),'median':round(q(.5),3),'p75':round(q(.75),3),'p90':round(q(.90),3),'p99':round(q(.99),3),'max':round(rr[-1],3)},
  'ratio_ge3':sum(1 for r in rr if r>=3),'ratio_ge2':sum(1 for r in rr if r>=2),'ratio_le_0.34':sum(1 for r in rr if r<=0.34)}

# ---- dictionary membership (technical / unusual spelling proxies)
con=sqlite3.connect(R+'build/hgm_spine_v27_2.db')
cur=con.cursor()
cur.execute("select wylie, hgm_gloss, tier, corpus_n_segments from entries")
dic={}
for w,g,t,n in cur.fetchall():
    dic.setdefault(w,(g,t,n))
out['dictionary']={'entries':len(dic)}
hit=0; glossed=0; miss=[]
for k in pairs:
    if k in dic:
        hit+=1
        g=dic[k][0]
        if g and g not in ('[]','null'): glossed+=1
    else: miss.append(k)
out['dictionary'].update({'evidence_headwords_in_dict':hit,'evidence_headwords_with_hgm_gloss':glossed,
   'evidence_headwords_not_in_dict':len(miss),'miss_examples':miss[:15]})

# ---- corpus frequency of evidence headwords over C01-C18, via syllable inverted index
cur.execute("select course, seq, wylie from corpus_segments where course like 'C%' and length(course)=3")
rows=[r for r in cur.fetchall() if re.fullmatch(r'C(0[1-9]|1[0-8])', r[0])]
inv=collections.defaultdict(set)
segtext={}
for c,s,w in rows:
    key=(c,s); segtext[key]=' '+(w or '')+' '
    for sy in set(re.split(r'\s+', (w or '').strip())):
        if sy: inv[sy].add(key)
out['corpus']={'c01_c18_segments':len(rows)}
freq={}
for k in pairs:
    ss=[s for s in re.split(r'\s+',k.strip()) if s]
    if not ss: freq[k]=0; continue
    cand=min((inv.get(s,set()) for s in ss), key=len)
    pat=' '+k.strip()+' '
    freq[k]=sum(1 for key in cand if pat in segtext[key])
fv=sorted(freq.values())
out['corpus']['headword_segment_frequency']={
  'n':len(fv),'zero':sum(1 for v in fv if v==0),'eq1':sum(1 for v in fv if v==1),
  'le2':sum(1 for v in fv if v<=2),'ge10':sum(1 for v in fv if v>=10),'ge50':sum(1 for v in fv if v>=50),
  'median':fv[len(fv)//2],'p90':fv[int(.9*len(fv))],'max':fv[-1]}
# decile boundaries
out['corpus']['freq_deciles']=[fv[int(p*(len(fv)-1))] for p in [0,.1,.2,.3,.4,.5,.6,.7,.8,.9,1.0]]
json.dump({'freq':freq}, open('/private/tmp/claude-501/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2/scratchpad/freq.json','w'))
print(json.dumps(out,indent=1))
