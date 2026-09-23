import json, collections, re
R='/Users/adamderickandrade/ALL-translation-tool/'
L=json.load(open(R+'data/alignment/alignment_full_v1.json'))['links']
d57=[l for l in L if l['d'] in (5,7) and l.get('eng')]
bytib=collections.defaultdict(collections.Counter)
for l in d57: bytib[l['tib']][l['eng']]+=1
def canon(s): return re.sub(r'[^a-z]','',s.lower())
cand=[]
for t,c in bytib.items():
    top,topn=c.most_common(1)[0]
    for e,k in c.items():
        if e!=top and k==1 and topn>=5: cand.append((t,e,top,topn))
print('singleton dissents:', len(cand))
casevar=[x for x in cand if canon(x[1])==canon(x[2])]
print(f'  case/punctuation-only variants of the dominant (pure noise): {len(casevar)} ({100*len(casevar)/len(cand):.1f}%)')
rest=[x for x in cand if canon(x[1])!=canon(x[2])]
print(f'  substantive dissents after that free filter: {len(rest)}')
# existing gate word list
GATE=set('the a an and or his our your i you'.split())
# wider supplied-word list
WIDE=set('the a an and or his our your i you of to in on at by for from as with these those this that its their there it we they he she all such each any some one when then so is are was were be been being do does did'.split())
def head(e): 
    m=re.match(r"[A-Za-z']+", e.strip())
    return m.group(0).lower() if m else ''
g1=[x for x in rest if head(x[1]) in GATE]
g2=[x for x in rest if head(x[1]) in WIDE]
print(f'  head word already caught by no_supplied_span_head gate: {len(g1)}')
print(f'  head word in a WIDER supplied-word list (free extension): {len(g2)}  (+{len(g2)-len(g1)} new)')
# multi-word dissent against single-word dominant = over-capture signature
oc=[x for x in rest if len(x[1].split())>len(x[2].split())+1]
print(f'  dissent at least 2 words LONGER than the dominant (over-capture signature): {len(oc)}')
print('  examples:')
for t,e,top,topn in sorted(oc,key=lambda x:-(len(x[1].split())-len(x[2].split())))[:10]:
    print(f'    {t!r:22} -> {e!r:44} vs {top!r} (x{topn})')
