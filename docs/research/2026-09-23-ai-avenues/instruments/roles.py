import json,glob,os,sys,collections,statistics as st
root=os.path.expanduser('~/.claude/projects/-Users-adamderickandrade-ALL-translation-tool/')
files=glob.glob(root+'*/subagents/**/*.jsonl',recursive=True)
agg=collections.defaultdict(lambda: collections.defaultdict(list))
for f in files:
    try:
        lines=open(f,errors='ignore').read().splitlines()
    except: continue
    if not lines: continue
    head="\n".join(lines[:4])[:400000]
    if 'You are the RECONCILER' in head or 'RECONCILE BRIEF' in head or 'RECONCILE_BRIEF' in head: role='reconcile'
    elif 'PROPOSE_BRIEF' in head or 'YOUR ANGLE IS TIBETAN-FIRST' in head or 'YOUR ANGLE IS ENGLISH-FIRST' in head or 'ONE OF TWO independent analysts' in head: role='propose'
    else: continue
    u=collections.Counter(); calls=0; seen=set(); model=collections.Counter()
    for ln in lines:
        try: r=json.loads(ln)
        except: continue
        m=r.get('message') or {}
        if r.get('type')=='assistant' and isinstance(m,dict) and m.get('usage'):
            mid=m.get('id')
            if mid in seen: continue
            seen.add(mid); calls+=1
            model[m.get('model')]+=1
            for k in ('input_tokens','cache_creation_input_tokens','cache_read_input_tokens','output_tokens'):
                u[k]+=m['usage'].get(k,0) or 0
    if calls==0: continue
    a=agg[role]
    a['calls'].append(calls)
    for k,v in u.items(): a[k].append(v)
    a['model'].append(model.most_common(1)[0][0])
for role,a in agg.items():
    print(role,'n=',len(a['calls']))
    for k in ('calls','input_tokens','cache_creation_input_tokens','cache_read_input_tokens','output_tokens'):
        v=a[k]; print('  ',k,'median',st.median(v),'mean',round(st.mean(v)))
    print('  models',collections.Counter(a['model']).most_common(4))
    # cost at opus 5 list? placeholder rates in/out/cw/cr per M
