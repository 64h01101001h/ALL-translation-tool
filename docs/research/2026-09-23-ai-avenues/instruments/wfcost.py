import json, glob, collections, statistics as st
P='/Users/adamderickandrade/.claude/projects/-Users-adamderickandrade-ALL-translation-tool'
PR={'in':5.0,'out':25.0,'cr':0.5,'cw5':6.25,'cw1h':10.0}
wf=collections.defaultdict(collections.Counter)
first={}
for f in glob.glob(P+'/*/subagents/workflows/*/*.jsonl'):
    w=f.split('/')[-2]; seen=set()
    for line in open(f):
        try: e=json.loads(line)
        except: continue
        m=e.get('message')
        if isinstance(m,dict) and m.get('usage'):
            rid=e.get('requestId') or e.get('uuid')
            if rid in seen: continue
            seen.add(rid); us=m['usage']; u=wf[w]
            u['in']+=us.get('input_tokens',0); u['out']+=us.get('output_tokens',0); u['cr']+=us.get('cache_read_input_tokens',0)
            cc=us.get('cache_creation') or {}
            if cc: u['cw5']+=cc.get('ephemeral_5m_input_tokens',0); u['cw1h']+=cc.get('ephemeral_1h_input_tokens',0)
            else: u['cw5']+=us.get('cache_creation_input_tokens',0)
            u['calls']+=1
            t=e.get('timestamp','')
            if w not in first or t<first[w]: first[w]=t
rows=[]
for w,u in wf.items():
    c=sum(u[k]*PR[k]/1e6 for k in PR); tot=u['in']+u['out']+u['cr']+u['cw5']+u['cw1h']
    rows.append((first[w][:16],w,round(c,2),u['calls'],tot, round(u['cr']/max(tot,1),3)))
rows.sort()
sel=[r for r in rows if r[0].startswith('2026-09-18') or r[0].startswith('2026-09-17T2')]
for r in sel: print(r)
cs=[r[2] for r in sel if r[3]>100]
print('median $/9-seg batch', st.median(cs), 'per segment', st.median(cs)/9, 'n', len(cs))
