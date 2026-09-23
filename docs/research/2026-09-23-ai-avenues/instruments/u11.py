import json, glob, os
base=os.path.expanduser('~/.claude/projects/-Users-adamderickandrade-ALL-translation-tool/')
for p in glob.glob(base+'*/subagents/*.meta.json'):
    m=json.load(open(p))
    d=m.get('description','')
    if not d.startswith('Propose') or 'C05' not in d: continue
    jp=p.replace('.meta.json','.jsonl')
    for line in open(jp, errors='replace'):
        d2=json.loads(line)
        if d2.get('type')=='user':
            c=(d2.get('message') or {}).get('content')
            s=c if isinstance(c,str) else json.dumps(c)
            print('PROMPT CHARS:', len(s))
            print(s[:700])
            raise SystemExit
