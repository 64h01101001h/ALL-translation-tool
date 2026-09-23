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
for (wf,seq),v in by.items():
    if not all(x in v for x in ('reconcile','tibetan-first','english-first')): continue
    k=lambda x:(x.get('tib'),x.get('eng'),x.get('d'))
    A=collections.Counter(k(x) for x in v['tibetan-first'].get('spans',[]))
    B=collections.Counter(k(x) for x in v['english-first'].get('spans',[]))
    for x in v['reconcile'].get('spans',[]):
        if x.get('d')!=5 or x.get('eng') is None: continue
        tot+=1
        kk=k(x)
        ag='U' if (A[kk] and B[kk]) else ('one' if (A[kk] or B[kk]) else 'none')
        e=P.get(x['tib'],{}).get('eng',[])
        cites=[c for (eng,cs) in e if eng==x['eng'] for c in cs]
        other=set(c.split('/')[0] for c in cites if c.split('/')[0]!='C05:'+seq)
        pr='p0' if not other else ('p1-2' if len(other)<=2 else 'p3+')
        hd='head' if HEAD.match(x['eng']) else '-'
        C[(ag,pr,hd)]+=1; C[(ag,)]+=1; C[(pr,)]+=1; C[(ag,pr)]+=1
print('reconciled d5 non-null spans',tot)
for key in sorted([c for c in C if len(c)==1]): print(key,C[key],'%.1f%%'%(100*C[key]/tot))
for key in sorted([c for c in C if len(c)==2]): print(key,C[key],'%.1f%%'%(100*C[key]/tot))
print('U & p3+ & no head:',C[('U','p3+','-')], '%.1f%%'%(100*C[('U','p3+','-')]/tot))
