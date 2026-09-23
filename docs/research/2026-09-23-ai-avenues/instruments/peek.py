import json, glob, os, collections
D=os.path.expanduser('~/.claude/projects/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2/subagents/workflows')
f=sorted(glob.glob(D+'/wf_025e4913-5f3/*.jsonl'))[0]
rows=[json.loads(l) for l in open(f)]
print('n rows', len(rows))
types=collections.Counter(r.get('type') for r in rows)
print(types)
# find assistant rows with usage
n=0
for r in rows:
    m=r.get('message') or {}
    if m.get('usage'):
        n+=1
        if n<=3:
            print(json.dumps({k:v for k,v in r.items() if k not in('message',)}, indent=1)[:600])
            print('MSG id',m.get('id'),'model',m.get('model'),'usage',m.get('usage'))
            print('content types',[c.get('type') for c in m.get('content',[]) if isinstance(c,dict)])
print('usage rows',n)
ids=[ (r['message']['id']) for r in rows if (r.get('message') or {}).get('usage')]
print('distinct ids',len(set(ids)))
c=collections.Counter(ids)
print('most common', c.most_common(3))
