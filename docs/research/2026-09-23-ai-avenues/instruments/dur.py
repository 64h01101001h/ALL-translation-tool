import glob, json, os, statistics
from datetime import datetime
base = os.path.expanduser('~/.claude/projects/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2/subagents/workflows')
durs=[]; aligned=[]
for f in glob.glob(base+'/*/agent-*.jsonl'):
    ts=[]; first=None
    with open(f, encoding='utf-8', errors='replace') as fh:
        for i,l in enumerate(fh):
            try: e=json.loads(l)
            except Exception: continue
            t=e.get('timestamp')
            if t: ts.append(t)
            if first is None and e.get('type')=='user':
                c=e.get('message',{}).get('content')
                first = c if isinstance(c,str) else json.dumps(c)[:3000]
    if len(ts)<2: continue
    p=lambda s: datetime.fromisoformat(s.replace('Z','+00:00'))
    d=(p(ts[-1])-p(ts[0])).total_seconds()/60
    durs.append(d)
    if first and ('YOUR TASK: propose' in first or 'YOUR TASK: reconcile' in first):
        aligned.append((d, 'reconcile' if 'YOUR TASK: reconcile' in first else 'propose'))
def s(x):
    x=sorted(x); n=len(x)
    return 'n=%d median=%.1f p90=%.1f mean=%.1f min' % (n, x[n//2], x[int(n*.9)], sum(x)/n) if n else 'n=0'
print('all agents', s(durs))
print('propose', s([d for d,k in aligned if k=='propose']))
print('reconcile', s([d for d,k in aligned if k=='reconcile']))
