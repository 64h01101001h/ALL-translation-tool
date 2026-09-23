import json, glob, collections, statistics as st, sys
P='/Users/adamderickandrade/.claude/projects/-Users-adamderickandrade-ALL-translation-tool'
PR={'in':5.0,'out':25.0,'cr':0.5,'cw5':6.25,'cw1h':10.0}
def agent_usage(f):
    last={}; first_user=None; ts=None
    for line in open(f):
        try: e=json.loads(line)
        except: continue
        if first_user is None and e.get('type')=='user':
            c=(e.get('message') or {}).get('content'); first_user=c if isinstance(c,str) else json.dumps(c)[:20000]; ts=e.get('timestamp','')
        m=e.get('message')
        if isinstance(m,dict) and m.get('usage'):
            rid=e.get('requestId') or e.get('uuid')
            us=m['usage']
            prev=last.get(rid)
            if prev is None or us.get('output_tokens',0)>=prev.get('output_tokens',0): last[rid]=us
    u=collections.Counter()
    for us in last.values():
        u['in']+=us.get('input_tokens',0); u['out']+=us.get('output_tokens',0); u['cr']+=us.get('cache_read_input_tokens',0)
        cc=us.get('cache_creation') or {}
        if cc: u['cw5']+=cc.get('ephemeral_5m_input_tokens',0); u['cw1h']+=cc.get('ephemeral_1h_input_tokens',0)
        else: u['cw5']+=us.get('cache_creation_input_tokens',0)
        u['calls']+=1
    return u, first_user or '', ts or ''
def cost(u): return sum(u[k]*PR[k]/1e6 for k in PR)
mode=sys.argv[1]
if mode=='wf':
    wf=collections.defaultdict(collections.Counter); t0={}
    for f in glob.glob(P+'/*/subagents/workflows/*/*.jsonl'):
        w=f.split('/')[-2]; u,fu,ts=agent_usage(f); wf[w].update(u)
        if ts and (w not in t0 or ts<t0[w]): t0[w]=ts
    sel=[(t0[w],w,cost(u),u) for w,u in wf.items() if t0.get(w,'')[:10] in ('2026-09-17','2026-09-18') and u['calls']>100]
    cs=sorted(c for _,_,c,_ in sel)
    print('n batches',len(cs),'median $/batch',round(st.median(cs),2),'per seg (9)',round(st.median(cs)/9,2))
    tot=collections.Counter()
    for *_,u in sel: tot.update(u)
    n=len(sel)*9
    print('per segment tokens: cr',tot['cr']/n,'cw5',tot['cw5']/n,'cw1h',tot['cw1h']/n,'out',tot['out']/n,'in',tot['in']/n,'calls',tot['calls']/n)
else:
    agg=collections.defaultdict(collections.Counter)
    for f in glob.glob(P+'/*/subagents/**/*.jsonl', recursive=True):
        u,fu,ts=agent_usage(f)
        if not ts.startswith('2026-09-04'): continue
        t=fu.lower()
        k='other'
        if 'audit_brief' in t or 'first reader' in t or 'first-read' in t: k='audit_first'
        elif 'second reader' in t or 'second_reader' in t: k='audit_second'
        elif 'skeptic' in t: k='skeptic'
        elif 'propose' in t[:3000]: k='propose/reconcile'
        elif 'reconcil' in t[:3000]: k='propose/reconcile'
        u['agents']=1; agg[k].update(u)
    for k,u in agg.items(): print(k,'agents',u['agents'],'$',round(cost(u),2))
