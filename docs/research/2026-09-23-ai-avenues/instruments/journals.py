import glob, json, os, collections
base = os.path.expanduser('~/.claude/projects/-Users-adamderickandrade-ALL-translation-tool')
runs = []
types = collections.Counter()
for j in glob.glob(base + '/*/subagents/workflows/*/journal.jsonl'):
    ev = []
    for line in open(j, encoding='utf-8', errors='replace'):
        try: ev.append(json.loads(line))
        except Exception: pass
    labels = [e.get('label','') for e in ev if e.get('type')=='started']
    if not any(l.startswith(('reconcile:','propose:')) for l in labels): continue
    for e in ev: types[e.get('type')] += 1
    started = {e['agentId']: e for e in ev if e.get('type')=='started'}
    res = [e for e in ev if e.get('type') in ('result','completed')]
    err = [e for e in ev if e.get('type') in ('error','failed')]
    runs.append((j, len(started), len(res), len(err), sum(1 for e in ev if e.get('type')=='replayed' or e.get('cached'))))
print('runs with propose/reconcile agents:', len(runs))
print('event types:', dict(types))
tot = [sum(r[i] for r in runs) for i in (1,2,3,4)]
print('started, result, error, cached-flag:', tot)
# show one sample of each distinct type
seen=set()
for j,*_ in runs[:50]:
    for line in open(j, encoding='utf-8', errors='replace'):
        e=json.loads(line); t=e.get('type')
        if t not in seen:
            seen.add(t); s=json.dumps(e)[:300]; print(t, '::', s)
