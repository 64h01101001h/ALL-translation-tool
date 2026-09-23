import json, glob, os, collections, re
base=os.path.expanduser('~/.claude/projects/-Users-adamderickandrade-ALL-translation-tool/')
claim=0; total=0; looked=0; both=0; examples=[]
pat=re.compile(r'(precedent|\bcounted\b|\bcount\b|existing links|\bbank\b)', re.I)
numpat=re.compile(r'\b\d+\s+(existing\s+)?(links?|occurrences?|instances?|precedents?|times)\b', re.I)
for p in glob.glob(base+'*/subagents/*.meta.json'):
    try: m=json.load(open(p))
    except Exception: continue
    d=m.get('description','')
    if not d.startswith('Reconcile'): continue
    jp=p.replace('.meta.json','.jsonl')
    if not os.path.exists(jp): continue
    total+=1
    txt=[]; did_lookup=False
    for line in open(jp, errors='replace'):
        if 'alignment_full_v1' in line or 'alignment_evidence_v1' in line: did_lookup=True
        try: d2=json.loads(line)
        except Exception: continue
        c=(d2.get('message') or {}).get('content')
        if isinstance(c,list):
            for b in c:
                if isinstance(b,dict) and b.get('type')=='text': txt.append(b['text'])
    t='\n'.join(txt)
    has=bool(numpat.search(t))
    if did_lookup: looked+=1
    if has:
        claim+=1
        if did_lookup: both+=1
        elif len(examples)<6:
            mm=numpat.search(t); examples.append((d, t[max(0,mm.start()-160):mm.end()+80].replace('\n',' ')))
print(f'reconcile agents: {total}')
print(f'  touched the bank file at all: {looked} ({100*looked/total:.1f}%)')
print(f'  asserted a numeric precedent count in prose: {claim} ({100*claim/total:.1f}%)')
print(f'  did BOTH (counted and claimed): {both}')
print(f'  claimed WITHOUT touching the bank: {claim-both}')
for d,e in examples: print('\n---',d,'\n  ',e[:320])
