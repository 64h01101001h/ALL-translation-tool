import json, glob, collections, os, re, sys
base=os.path.expanduser('~/.claude/projects/-Users-adamderickandrade-ALL-translation-tool/')
rows=[]
for f in glob.glob(base+'*.jsonl'):
    try:
        for line in open(f, errors='replace'):
            if 'propose:' not in line and 'reconcile:' not in line: continue
            try: d=json.loads(line)
            except Exception: continue
            rows.append((f,d))
    except Exception as e: pass
print('lines with labels:', len(rows))
# inspect shape of one
keys=collections.Counter()
for f,d in rows[:2000]:
    keys[tuple(sorted(d.keys()))]+=1
for k,v in keys.most_common(8): print(v,k)
