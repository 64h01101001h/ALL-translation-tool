import json, glob, os, collections, statistics, re
base=os.path.expanduser('~/.claude/projects/-Users-adamderickandrade-ALL-translation-tool/')
metas=glob.glob(base+'*/subagents/*.meta.json')
tools=collections.Counter(); tchars=collections.Counter()
resbytes=collections.defaultdict(list)
percall=collections.defaultdict(list)
firstprompt=None
nagents=collections.Counter()
for p in metas:
    try: m=json.load(open(p))
    except Exception: continue
    d=m.get('description','')
    w=d.split()[0] if d else ''
    if w not in ('Propose','Reconcile'): continue
    if 'C05' not in d: continue
    jp=p.replace('.meta.json','.jsonl')
    if not os.path.exists(jp): continue
    nagents[w]+=1
    per=collections.Counter()
    for line in open(jp, errors='replace'):
        try: d2=json.loads(line)
        except Exception: continue
        msg=d2.get('message') or {}
        c=msg.get('content')
        if isinstance(c,list):
            for b in c:
                if isinstance(b,dict) and b.get('type')=='tool_use':
                    tools[(w,b.get('name'))]+=1
                    per[b.get('name')]+=1
                    if b.get('name')=='Bash':
                        cmd=b.get('input',{}).get('command','')
                        tchars[('cmd',cmd[:110])]+=1
        tur=d2.get('toolUseResult')
        if tur is not None:
            resbytes[w].append(len(json.dumps(tur)))
        if firstprompt is None and w=='Reconcile' and d2.get('type')=='user' and isinstance(c,str):
            firstprompt=c
    for k,v in per.items(): percall[(w,k)].append(v)
print('agents:',dict(nagents))
print('\n--- tool calls per role (total / mean per agent) ---')
for (w,t),n in tools.most_common(25):
    print(f'{w:10} {str(t):22} {n:>6}  mean/agent {n/nagents[w]:5.1f}')
print('\n--- tool result bytes ---')
for w,v in resbytes.items():
    print(w, 'n_results', len(v), 'total_MB', round(sum(v)/1e6,1), 'mean', int(statistics.mean(v)), 'max', max(v), 'per_agent_MB', round(sum(v)/nagents[w]/1e6,3))
print('\n--- most common bash commands ---')
for (k,c),n in tchars.most_common(25): print(f'{n:>4}  {c}')
