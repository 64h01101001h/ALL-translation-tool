import json, glob, os, re
base=os.path.expanduser('~/.claude/projects/-Users-adamderickandrade-ALL-translation-tool/')
numpat=re.compile(r'\b(\d+)\s+(existing\s+)?(links?|occurrences?|instances?|precedents?|rows?|times)\b', re.I)
countcmd=re.compile(r'grep|wc -l|sqlite3|json\.load|count\(|rg ', re.I)
tot=0; claimed=0; noevidence=[]
for p in glob.glob(base+'*/subagents/*.meta.json'):
    try: m=json.load(open(p))
    except Exception: continue
    d=m.get('description','')
    if not d.startswith('Reconcile'): continue
    jp=p.replace('.meta.json','.jsonl')
    if not os.path.exists(jp): continue
    tot+=1; txt=[]; ran=False
    for line in open(jp, errors='replace'):
        try: d2=json.loads(line)
        except Exception: continue
        c=(d2.get('message') or {}).get('content')
        if isinstance(c,list):
            for b in c:
                if not isinstance(b,dict): continue
                if b.get('type')=='text': txt.append(b['text'])
                if b.get('type')=='tool_use':
                    s=json.dumps(b.get('input',{}))
                    if countcmd.search(s): ran=True
    t='\n'.join(txt)
    if numpat.search(t):
        claimed+=1
        if not ran:
            mm=numpat.search(t); noevidence.append((d, t[max(0,mm.start()-200):mm.end()+60].replace('\n',' ')))
print(f'reconcile agents {tot}; asserted a numeric precedent/occurrence count: {claimed}')
print(f'of those, ran NO counting command of any kind: {len(noevidence)}')
for d,e in noevidence[:8]: print('\n---',d,'\n  ',e[:340])
