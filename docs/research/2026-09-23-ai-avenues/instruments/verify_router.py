import json,collections,re
S=json.load(open('specs.json'))
P=json.load(open('/Users/adamderickandrade/ALL-translation-tool/data/alignment/precedent_index_v1.json'))['index']
by=collections.defaultdict(dict)
for k,(label,spec) in S.items():
    parts=label.split(':'); phase=parts[0]; seq=parts[1]
    angle=parts[2] if len(parts)>2 else 'reconcile'
    by[(k.split('|')[0],seq)][angle if phase=='propose' else 'reconcile']=spec
HEAD=re.compile(r"^(the|a|an|and|or|his|our|your|I|you)\b",re.I)
C=collections.Counter(); tot=0
segs=set(); courses_seen=collections.Counter()
for (wf,seq),v in by.items():
    if not all(x in v for x in ('reconcile','tibetan-first','english-first')): continue
    segs.add(seq)
    k=lambda x:(x.get('tib'),x.get('eng'),x.get('d'))
    A=collections.Counter(k(x) for x in v['tibetan-first'].get('spans',[]))
    B=collections.Counter(k(x) for x in v['english-first'].get('spans',[]))
    for x in v['reconcile'].get('spans',[]):
        if x.get('d')!=5 or x.get('eng') is None: continue
        tot+=1
        kk=k(x)
        U=(A[kk] and B[kk])
        e=P.get(x['tib'],{}).get('eng',[])
        cites=[c for (eng,cs) in e if eng==x['eng'] for c in cs]
        other=set(c.split('/')[0] for c in cites if c.split('/')[0]!='C05:'+seq)
        nonc05=set(c for c in other if not c.startswith('C05:'))
        earlier05=set(c for c in other if c.startswith('C05:') and c.split(':')[1].isdigit() and int(c.split(':')[1])<int(seq))
        indep_earlier=nonc05|earlier05
        hd=bool(HEAD.match(x['eng']))
        if U and not hd and len(other)>=3:
            C['region_any']+=1
            if len(nonc05)>=3: C['region_nonC05_3+']+=1
            if len(indep_earlier)>=3: C['region_earlier_3+']+=1
            if len(nonc05)==0: C['region_only_C05_attest']+=1
print('segments',len(segs),'tot',tot)
for kx,vx in C.items(): print(kx,vx,'%.1f%%'%(100*vx/tot))
