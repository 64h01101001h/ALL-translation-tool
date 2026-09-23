# READ-ONLY experiment: how much of a new course's landed d=5 work was already
# attested (same tib, same eng) in earlier courses' bank; and how often a naive
# unique-string anchor rule reproduces a landed span. Agreement with landed
# (machine) output is NOT accuracy.
import json, sqlite3, re, collections, sys
R='/Users/adamderickandrade/ALL-translation-tool/'
d=json.load(open(R+'data/alignment/alignment_full_v1.json'))
L=d['links']
db=sqlite3.connect('file:'+R+'build/hgm_spine_v27_2.db?mode=ro',uri=True)
seg={}
for cid,course,seq,wy,en in db.execute("select id,course,seq,wylie,english from corpus_segments where course in ('C01','C02','C03','C04','C05')"):
    seg[(course,seq)]=(wy or '',en or '')
test=sys.argv[1] if len(sys.argv)>1 else 'C05'
order=['C01','C02','C03','C04','C05']
prior=order[:order.index(test)]
prior_texts=set(seg[k] for k in seg if k[0] in prior)
bank_pairs=collections.Counter(); bank_tib=collections.Counter(); bank_tib_null=collections.Counter()
for l in L:
    if l['course'] in prior and l['d'] in (5,6,7):
        t=l['tib'].strip()
        if l['eng'] is None: bank_tib_null[t]+=1
        else: bank_pairs[(t,l['eng'].strip())]+=1
        bank_tib[t]+=1
tl=[l for l in L if l['course']==test and l['d']==5 and l['seg'] is not None]
dup=set(k for k in seg if k[0]==test and seg[k] in prior_texts)
tl_nd=[l for l in tl if (test,l['seg']) not in dup]
print('test',test,'prior',prior,'d5 spans',len(tl),'excluding exact-dup segments',len(tl_nd),'dup segs',len(dup))
c=collections.Counter()
for l in tl_nd:
    t=l['tib'].strip(); e=l['eng'].strip() if l['eng'] else None
    if e is None: c['null-eng']+=1; c['null-eng tib seen null before' if bank_tib_null[t] else 'null-eng tib not seen null']+=1
    elif (t,e) in bank_pairs: c['pair attested before']+=1
    elif t in bank_tib: c['known tib, new rendering']+=1
    else: c['new tib']+=1
for k,v in sorted(c.items()): print(f'  {k:40s} {v:6d} {v/len(tl_nd):.1%}')
# naive anchor rule
def occ(hay,needle,word=True):
    if not needle: return 0
    if word:
        return len(re.findall(r'(?<![A-Za-z\'])'+re.escape(needle)+r'(?![A-Za-z])',hay))
    return hay.count(needle)
def tocc(wy,t):
    # syllable-bounded match in wylie: bounded by start/space/shad
    return len(re.findall(r'(?:(?<=^)|(?<=[ /,|]))'+re.escape(t)+r'(?=$|[ /,|])',wy))
landed=collections.defaultdict(set)
for l in tl_nd:
    if l['eng']: landed[l['seg']].add((l['tib'].strip(),l['eng'].strip()))
segs=sorted(set(l['seg'] for l in tl_nd))
cand=0; hit=0; covered=set()
pairs_by_tib=collections.defaultdict(list)
for (t,e),n in bank_pairs.items(): pairs_by_tib[t].append(e)
for s in segs:
    wy,en=seg[(test,s)]
    for t,es in pairs_by_tib.items():
        if len(t)<2 or tocc(wy,t)!=1: continue
        good=[e for e in es if occ(en,e)==1]
        if len(good)!=1: continue   # ambiguous or absent: refuse
        cand+=1
        if (t,good[0]) in landed[s]: hit+=1; covered.add((s,t,good[0]))
tot=sum(len(v) for v in landed.values())
print(f'naive anchors proposed {cand}, reproduce a landed span {hit} ({hit/max(cand,1):.1%}); landed non-null d5 spans covered {len(covered)}/{tot} ({len(covered)/tot:.1%})')
# refine: classify each anchor against landed spans for that segment
landed_tib=collections.defaultdict(list)
for l in L:
    if l['course']==test and l['seg'] is not None and l['d'] in (1,2,3,4,5,6,7):
        landed_tib[l['seg']].append((l['d'],l['tib'].strip(),(l['eng'] or '').strip() or None))
cls=collections.Counter(); ex=collections.defaultdict(list)
for s in segs:
    wy,en=seg[(test,s)]
    for t,es in pairs_by_tib.items():
        if len(t)<2 or tocc(wy,t)!=1: continue
        good=[e for e in es if occ(en,e)==1]
        if len(good)!=1: continue
        e=good[0]
        same=[x for x in landed_tib[s] if x[1]==t and x[0]>=5]
        if any(x[2]==e for x in same): k='exact'
        elif any(x[2] is None for x in same): k='landed same tib as NULL'
        elif same: k='landed same tib, other eng'
        elif any(x[0]==5 and t in x[1] and x[1]!=t for x in landed_tib[s]): k='inside a longer landed d5'
        elif any(x[0]==5 and x[1] in t and x[1]!=t for x in landed_tib[s]): k='landed split it finer'
        else: k='other'
        nsyl=len(t.split())
        cls[(k)]+=1; cls[(k, 'multi' if nsyl>1 else 'mono')]+=1
        if len(ex[k])<4: ex[k].append((s,t,e,[x for x in same][:2]))
for k,v in sorted(cls.items(), key=lambda kv: str(kv[0])): print(' ',k,v)
for k,v in ex.items(): print(k, v)
