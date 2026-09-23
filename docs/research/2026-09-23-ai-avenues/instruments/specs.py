import json, glob, collections, re
P='/Users/adamderickandrade/.claude/projects/-Users-adamderickandrade-ALL-translation-tool'
kinds=collections.Counter(); segs=collections.defaultdict(set)
for f in glob.glob(P+'/*/subagents/workflows/*/*.jsonl'):
    for line in open(f):
        if '"Write"' not in line or 'spec.json' not in line: continue
        e=json.loads(line); m=e.get('message') or {}
        for b in (m.get('content') or []):
            if isinstance(b,dict) and b.get('type')=='tool_use' and b.get('name')=='Write':
                p=b.get('input',{}).get('file_path','')
                if p.endswith('spec.json'):
                    mm=re.search(r'/(c0\d)/(\d+)-([a-z-]+)/spec\.json$',p)
                    if mm:
                        kinds[mm.group(3)]+=1; segs[mm.group(3)].add((mm.group(1),mm.group(2)))
                    else:
                        kinds['other:'+p.split('/')[-2][:20]]+=1
print(kinds.most_common(12))
print({k:len(v) for k,v in segs.items()})
