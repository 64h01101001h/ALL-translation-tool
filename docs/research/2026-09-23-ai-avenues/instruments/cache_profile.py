import json,glob,os,sys,statistics as st
from datetime import datetime
BASE=os.path.expanduser('~/.claude/projects/-Users-adamderickandrade-ALL-translation-tool/')
runs={}
for f in glob.glob(BASE+'*/workflows/wf_*.json'):
    d=json.load(open(f))
    if d.get('workflowName')=='c05-align' and d.get('status')=='completed':
        runs[d['runId']]=(f.split('/')[-3], d)
def ts(s): return datetime.fromisoformat(s.replace('Z','+00:00')).timestamp()
agents=[]
for rid,(sess,d) in sorted(runs.items(), key=lambda x:x[1][1]['timestamp']):
    for af in glob.glob(BASE+f'{sess}/subagents/workflows/{rid}/agent-*.jsonl'):
        calls={}  # msg id -> (ts, usage, model)
        order=[]
        role=None
        for line in open(af):
            try: e=json.loads(line)
            except: continue
            m=e.get('message')
            if (role is None or role=='other') and e.get('type')=='user' and isinstance(m,dict):
                c=m.get('content'); c=c if isinstance(c,str) else json.dumps(c)
                cl=c.lower()
                if 'reconcile the two' in cl or 'you are the reconciler' in cl: role='reconcile'
                elif 'propose the full-depth' in cl or 'your angle' in cl or 'angle:' in cl: role='propose'
                elif 'transport step' in cl: role='prefetch'
                else: role=role or 'other'
            if isinstance(m,dict) and m.get('usage') and m.get('id'):
                mid=m['id']; u=m['usage']
                if mid not in calls:
                    calls[mid]=[e.get('timestamp'),u,m.get('model')]; order.append(mid)
                else:
                    # keep max output
                    if u.get('output_tokens',0)>calls[mid][1].get('output_tokens',0): calls[mid][1]=u
        if not order: continue
        seq=[calls[i] for i in order]
        seq.sort(key=lambda x: x[0])
        R=sum(c[1].get('cache_read_input_tokens',0) for c in seq)
        W5=sum((c[1].get('cache_creation') or {}).get('ephemeral_5m_input_tokens',c[1].get('cache_creation_input_tokens',0)) for c in seq)
        W1=sum((c[1].get('cache_creation') or {}).get('ephemeral_1h_input_tokens',0) for c in seq)
        I=sum(c[1].get('input_tokens',0) for c in seq)
        O=sum(c[1].get('output_tokens',0) for c in seq)
        T=sum((c[1].get('output_tokens_details') or {}).get('thinking_tokens',0) for c in seq)
        gaps=[ts(seq[i][0])-ts(seq[i-1][0]) for i in range(1,len(seq))]
        # TTL-expiry detection: a call whose cache_read is < previous call's (read+write) by > 5000
        expiries=0; rewrite=0
        for i in range(1,len(seq)):
            pu=seq[i-1][1]; cu=seq[i][1]
            prev=pu.get('cache_read_input_tokens',0)+pu.get('cache_creation_input_tokens',0)
            if cu.get('cache_read_input_tokens',0) < prev-5000:
                expiries+=1; rewrite+=cu.get('cache_creation_input_tokens',0)
        models={}
        for c in seq: models[c[2]]=models.get(c[2],0)+1
        agents.append(dict(run=rid,role=role,n=len(seq),R=R,W5=W5,W1=W1,I=I,O=O,T=T,
            first_R=seq[0][1].get('cache_read_input_tokens',0),first_W=seq[0][1].get('cache_creation_input_tokens',0),
            t0=ts(seq[0][0]),t1=ts(seq[-1][0]),maxgap=max(gaps) if gaps else 0,gaps_over_300=sum(g>300 for g in gaps),
            expiries=expiries,rewrite=rewrite,models=models,tier=seq[0][1].get('service_tier')))
json.dump(agents,open(os.path.dirname(__file__)+'/agents.json','w'))
print('runs',len(runs),'agents',len(agents))
from collections import Counter
print(Counter(a['role'] for a in agents))
mc=Counter()
for a in agents: mc.update(a['models'])
print('models',mc)
print('tiers',Counter(a['tier'] for a in agents))
def summ(k,xs): 
    xs=sorted(xs); return f"{k}: n={len(xs)} mean={st.mean(xs):.0f} median={st.median(xs):.0f} min={xs[0]} max={xs[-1]}"
for role in ['propose','reconcile','prefetch']:
    A=[a for a in agents if a['role']==role]
    if not A: continue
    print('==',role)
    for k in ['n','R','W5','W1','I','O','T','first_R','first_W','maxgap','gaps_over_300','expiries','rewrite']:
        print('  ',summ(k,[a[k] for a in A]))
nseg=sum(1 for a in agents if a['role']=='reconcile')
tot=lambda k: sum(a[k] for a in agents)
print('segments(reconcilers)',nseg)
for k in ['n','R','W5','W1','I','O','T','rewrite']:
    print(k,'per segment',tot(k)/nseg)
