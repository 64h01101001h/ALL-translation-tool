import json, glob, os, collections, sys
P='/Users/adamderickandrade/.claude/projects/-Users-adamderickandrade-ALL-translation-tool'
files=glob.glob(P+'/*/subagents/**/*.jsonl', recursive=True)
agg=collections.defaultdict(lambda: collections.Counter())
models=collections.defaultdict(collections.Counter)
for f in files:
    role=None; seen=set(); u=collections.Counter(); n=0
    try:
        with open(f) as fh:
            for i,line in enumerate(fh):
                try: r=json.loads(line)
                except: continue
                if role is None and r.get('type')=='user':
                    c=r.get('message',{}).get('content')
                    txt=c if isinstance(c,str) else json.dumps(c)[:200000]
                    if 'RECONCILE BRIEF' in txt: role='reconcile'
                    elif 'PROPOSE BRIEF' in txt:
                        role='propose'
                    else: role='other'
                if r.get('type')=='assistant':
                    m=r.get('message',{}); mid=m.get('id')
                    if mid in seen: continue
                    seen.add(mid); us=m.get('usage') or {}
                    n+=1
                    for k in ('input_tokens','cache_creation_input_tokens','cache_read_input_tokens','output_tokens'):
                        u[k]+=us.get(k,0) or 0
                    models[role][m.get('model')]+=1
    except Exception as e: continue
    if role in ('propose','reconcile'):
        agg[role].update(u); agg[role]['agents']+=1; agg[role]['turns']+=n
for role,u in agg.items():
    a=u['agents']
    cost=(u['input_tokens']*5+u['cache_creation_input_tokens']*6.25+u['cache_read_input_tokens']*0.5+u['output_tokens']*25)/1e6
    print(role, 'agents',a,'turns/agent %.1f'%(u['turns']/a),
      'per agent: in %.0f cw %.0f cr %.0f out %.0f'%(u['input_tokens']/a,u['cache_creation_input_tokens']/a,u['cache_read_input_tokens']/a,u['output_tokens']/a),
      'cost/agent@$5/$25 %.2f'%(cost/a), 'total %.0f'%cost)
for role in models: print(role, models[role].most_common(4))
