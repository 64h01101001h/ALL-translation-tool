import json,glob,os,collections,statistics as st
root=os.path.expanduser('~/.claude/projects/-Users-adamderickandrade-ALL-translation-tool/')
P={'in':5.0,'cw':6.25,'cr':0.5,'out':25.0}  # $/M, Opus 5 list per repo's own reconstruction; verify
wfs=collections.defaultdict(lambda: collections.defaultdict(lambda: collections.Counter()))
for f in glob.glob(root+'*/subagents/workflows/*/*.jsonl'):
    wf=f.split('/')[-2]
    lines=open(f,errors='ignore').read().splitlines()
    if not lines: continue
    head="\n".join(lines[:4])[:400000]
    first=lines[0][:3000]
    if 'RECONCILER' in first: role='reconcile'
    elif 'YOUR ANGLE IS' in head or 'analyst' in first.lower(): role='propose'
    else: role='other'
    seen=set(); u=collections.Counter()
    for ln in lines:
        try: r=json.loads(ln)
        except: continue
        m=r.get('message') or {}
        if r.get('type')=='assistant' and isinstance(m,dict) and m.get('usage'):
            if m.get('id') in seen: continue
            seen.add(m.get('id')); u['calls']+=1
            us=m['usage']
            u['in']+=us.get('input_tokens',0) or 0
            u['cw']+=us.get('cache_creation_input_tokens',0) or 0
            u['cr']+=us.get('cache_read_input_tokens',0) or 0
            u['out']+=us.get('output_tokens',0) or 0
    u['agents']=1
    wfs[wf][role]+=u
tot=collections.defaultdict(collections.Counter); nseg=0
for wf,roles in wfs.items():
    if roles['reconcile']['agents']==0 or roles['propose']['agents']==0: continue
    nseg+=roles['reconcile']['agents']
    for r,u in roles.items(): tot[r]+=u
print('first-line samples of other:', [x for x in []])
print('workflows with both roles; segments (=reconcile agents):',nseg)
for r,u in tot.items():
    cost=sum(u[k]*P[k]/1e6 for k in P)
    print(r,'agents',u['agents'],'calls/agent',round(u['calls']/u['agents'],1),
      'per-seg: cr',round(u['cr']/nseg),'cw',round(u['cw']/nseg),'out',round(u['out']/nseg),'in',round(u['in']/nseg),
      'cost/seg $',round(cost/nseg,2))
