import json,glob,os,collections,statistics as st
root=os.path.expanduser('~/.claude/projects/-Users-adamderickandrade-ALL-translation-tool/')
P={'in':5.0,'cw':6.25,'cr':0.5,'out':25.0}
wfs=collections.defaultdict(lambda: collections.defaultdict(collections.Counter))
per=collections.defaultdict(list)
for f in glob.glob(root+'*/subagents/workflows/*/*.jsonl'):
    wf=f.split('/')[-2]
    lines=open(f,errors='ignore').read().splitlines()
    if not lines: continue
    try:
        t=json.loads(lines[0])['message']['content']
    except: continue
    if isinstance(t,list): t=' '.join(x.get('text','') for x in t if isinstance(x,dict))
    if not t.startswith('Full-depth Tibetan-English alignment'): continue
    if 'YOUR ANGLE IS' in t: role='propose'
    elif 'RECONCILER' in t or 'RECONCILE BRIEF' in t: role='reconcile'
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
    per[role].append(sum(u[k]*P[k]/1e6 for k in P))
tot=collections.defaultdict(collections.Counter); nseg=0
for wf,roles in wfs.items():
    if roles['reconcile']['agents']==0: continue
    nseg+=roles['reconcile']['agents']
    for r,u in roles.items(): tot[r]+=u
print('segments (reconcile agents):',nseg, 'workflows', len(wfs))
allc=0
for r,u in tot.items():
    cost=sum(u[k]*P[k]/1e6 for k in P); allc+=cost
    print(r,'agents',u['agents'],'calls/agent',round(u['calls']/u['agents'],1),
      'per-seg: cr',round(u['cr']/nseg),'cw',round(u['cw']/nseg),'out',round(u['out']/nseg),
      'cost/seg $',round(cost/nseg,2))
print('total/seg',round(allc/nseg,2))
for r,v in per.items(): print(r,'per-agent cost median',round(st.median(v),2),'mean',round(st.mean(v),2),'n',len(v))
