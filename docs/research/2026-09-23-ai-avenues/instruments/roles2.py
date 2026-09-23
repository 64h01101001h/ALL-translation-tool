import json,glob,os,collections,statistics as st
root=os.path.expanduser('~/.claude/projects/-Users-adamderickandrade-ALL-translation-tool/')
P={'in':5.0,'cw':6.25,'cr':0.5,'out':25.0}
files=glob.glob(root+'*/subagents/**/*.jsonl',recursive=True)
agg=collections.defaultdict(list)
for f in files:
    lines=open(f,errors='ignore').read().splitlines()
    if not lines: continue
    try:
        t=json.loads(lines[0])['message']['content']
    except: continue
    if isinstance(t,list): t=' '.join(x.get('text','') for x in t if isinstance(x,dict))
    if 'You are the RECONCILER' in t[:400] or 'RECONCILER for' in t[:600]: role='reconcile'
    elif 'YOUR ANGLE IS' in t or 'PROPOSE_BRIEF' in t[:2000] or 'PROPOSE BRIEF' in t: role='propose'
    else: continue
    msgs={}
    for ln in lines:
        try: r=json.loads(ln)
        except: continue
        m=r.get('message') or {}
        if r.get('type')=='assistant' and isinstance(m,dict) and m.get('usage'):
            us=m['usage']; mid=m.get('id')
            cur=msgs.setdefault(mid,collections.Counter())
            for a,b in (('in','input_tokens'),('cw','cache_creation_input_tokens'),('cr','cache_read_input_tokens'),('out','output_tokens')):
                cur[a]=max(cur[a],us.get(b,0) or 0)
    if not msgs: continue
    u=collections.Counter()
    for v in msgs.values(): u+=v
    u['calls']=len(msgs)
    agg[role].append(u)
for role,L in agg.items():
    print(role,'n',len(L))
    for k in ('calls','in','cw','cr','out'):
        v=[x[k] for x in L]; print('  ',k,'median',st.median(v),'mean',round(st.mean(v)))
    c=[sum(x[k]*P[k]/1e6 for k in P) for x in L]
    print('   cost/agent median',round(st.median(c),2),'mean',round(st.mean(c),2))
