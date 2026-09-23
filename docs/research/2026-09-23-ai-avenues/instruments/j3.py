import glob, json, os, collections, re
base = os.path.expanduser('~/.claude/projects/-Users-adamderickandrade-ALL-translation-tool')
reasons=collections.Counter(); S=R=F=0; per_session=collections.Counter()
for j in glob.glob(base + '/*/subagents/workflows/*/journal.jsonl'):
    ev=[json.loads(l) for l in open(j, encoding='utf-8', errors='replace') if l.strip()]
    if any(e.get('label','').startswith('research') for e in ev): continue  # skip the in-flight research run
    sess=j.split('/')[-5][:8]
    for e in ev:
        t=e.get('type')
        if t=='started': S+=1; per_session[sess]+=1
        elif t=='result': R+=1
        elif t=='failed':
            F+=1
            msg=json.dumps(e)[:4000].lower()
            m = ('weekly limit' if 'weekly' in msg else 'usage/spend limit' if ('limit' in msg and ('usage' in msg or 'spend' in msg)) else
                 'rate limit' if 'rate' in msg and 'limit' in msg else 'safeguard/refusal' if ('safeguard' in msg or 'refus' in msg) else
                 'stopped/aborted' if ('abort' in msg or 'stopp' in msg or 'cancel' in msg or 'interrupt' in msg) else
                 'schema/structured' if ('schema' in msg or 'structured' in msg) else 'other')
            reasons[m]+=1
print('sessions',dict(per_session)); print('started',S,'result',R,'failed',F,'no outcome',S-R-F)
print('failure reasons',reasons.most_common())
