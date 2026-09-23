import glob, json, os, collections
base = os.path.expanduser('~/.claude/projects/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2/subagents/workflows')
kind={}; seg=collections.defaultdict(list)
import re
for f in glob.glob(base+'/*/agent-*.jsonl'):
    aid=os.path.basename(f)[6:-6]
    with open(f, encoding='utf-8', errors='replace') as fh:
        for l in fh:
            try: e=json.loads(l)
            except Exception: continue
            if e.get('type')=='user':
                c=e.get('message',{}).get('content'); c=c if isinstance(c,str) else json.dumps(c)
                m=re.search(r'YOUR TASK: (propose|reconcile)[^\n]*?segment (\d+)', c)
                if m: kind[aid]=(m.group(1), int(m.group(2)))
                break
out=collections.Counter(); starts=collections.Counter(); keyseg=collections.Counter()
for j in glob.glob(base+'/*/journal.jsonl'):
    ev=[json.loads(l) for l in open(j) if l.strip()]
    st={e['agentId']:e['key'] for e in ev if e.get('type')=='started'}
    fin={e['agentId']:e['type'] for e in ev if e.get('type') in ('result','failed')}
    for a,k in st.items():
        if a in kind:
            out[(kind[a][0], fin.get(a,'no-outcome'))]+=1
            keyseg[(kind[a][0],kind[a][1])]+=1
print(sorted(out.items()))
multi=[(k,v) for k,v in keyseg.items() if (k[0]=='reconcile' and v>1) or (k[0]=='propose' and v>2)]
print('segments by kind:', collections.Counter(k[0] for k in keyseg), 'segments with repeated agents of a kind:', len(multi), 'extra agent starts:', sum(v-(1 if k[0]=='reconcile' else 2) for k,v in multi))
