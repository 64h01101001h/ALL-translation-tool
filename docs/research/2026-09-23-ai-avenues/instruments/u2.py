import json, glob, os
base=os.path.expanduser('~/.claude/projects/-Users-adamderickandrade-ALL-translation-tool/')
for f in glob.glob(base+'*.jsonl'):
    for line in open(f, errors='replace'):
        if '"toolUseResult"' not in line: continue
        if 'propose:' not in line: continue
        d=json.loads(line)
        t=d.get('toolUseResult')
        s=json.dumps(t)[:3000]
        print(f.split('/')[-1]); print(s); raise SystemExit
