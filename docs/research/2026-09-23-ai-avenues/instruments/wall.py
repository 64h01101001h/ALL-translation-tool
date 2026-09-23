import json, glob, os, collections
P='/Users/adamderickandrade/.claude/projects/-Users-adamderickandrade-ALL-translation-tool'
wf=collections.defaultdict(list)
for f in glob.glob(P+'/*/subagents/workflows/*/*.jsonl'):
    w=f.split('/')[-2]
    tsl=[]
    for line in open(f):
        try: e=json.loads(line)
        except: continue
        t=e.get('timestamp')
        if t: tsl.append(t)
    if tsl: wf[w].append((min(tsl),max(tsl)))
from datetime import datetime
rows=[]
for w,v in wf.items():
    a=min(x[0] for x in v); b=max(x[1] for x in v)
    d=(datetime.fromisoformat(b.replace('Z','+00:00'))-datetime.fromisoformat(a.replace('Z','+00:00'))).total_seconds()/60
    rows.append((a[:16],w,len(v),round(d,1)))
rows.sort()
for r in rows[-25:]: print(r)
print('workflows',len(rows))
