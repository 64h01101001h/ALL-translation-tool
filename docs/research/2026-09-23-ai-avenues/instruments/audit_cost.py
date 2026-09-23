import json, glob, os, collections, re
P='/Users/adamderickandrade/.claude/projects/-Users-adamderickandrade-ALL-translation-tool'
files=glob.glob(P+'/*/subagents/**/*.jsonl', recursive=True)
PR={'in':5.0,'out':25.0,'cr':0.5,'cw5':6.25,'cw1h':10.0}
def classify(first):
    t=first.lower()
    if 'audit_brief' in t or 'first reader' in t or 'first-read' in t: return 'audit_first'
    if 'second reader' in t or 'second_reader' in t: return 'audit_second'
    if 'skeptic' in t and ('licence' in t or 'license' in t or 'stands' in t): return 'skeptic'
    if 'propose_brief' in t or 'propose' in t[:3000] and 'analyst' in t[:3000]: return 'propose'
    if 'reconcile_brief' in t or 'reconcil' in t[:3000]: return 'reconcile'
    return 'other'
agg=collections.defaultdict(lambda: collections.Counter())
dates=collections.defaultdict(collections.Counter)
for f in files:
    first=None; u=collections.Counter(); ts=None; seen=set()
    try:
        for line in open(f):
            e=json.loads(line)
            if first is None and e.get('type')=='user':
                m=e.get('message',{}); c=m.get('content')
                first=c if isinstance(c,str) else json.dumps(c)[:20000]
                ts=e.get('timestamp','')[:10]
            m=e.get('message')
            if isinstance(m,dict) and m.get('usage') and e.get('requestId') not in seen:
                seen.add(e.get('requestId'))
                us=m['usage']
                u['in']+=us.get('input_tokens',0); u['out']+=us.get('output_tokens',0)
                u['cr']+=us.get('cache_read_input_tokens',0)
                cc=us.get('cache_creation') or {}
                u['cw5']+=cc.get('ephemeral_5m_input_tokens', us.get('cache_creation_input_tokens',0) if not cc else 0)
                u['cw1h']+=cc.get('ephemeral_1h_input_tokens',0)
                u['calls']+=1
    except Exception as ex:
        continue
    if first is None: continue
    k=classify(first)
    u['agents']=1
    agg[(ts,k)].update(u)
rows=sorted(agg.items())
for (ts,k),u in rows:
    if k in ("propose","reconcile"):
        cost=sum(u[x]*PR[x]/1e6 for x in PR)
        print(ts,k,'agents',u['agents'],'calls',u['calls'],'cost$',round(cost,2),'per agent',round(cost/max(1,u['agents']),3))
