import json, glob, os, collections, sys
P='/Users/adamderickandrade/.claude/projects/-Users-adamderickandrade-ALL-translation-tool'
PRICE={ # input, write5m, write1h, read, output per M
 'claude-opus-5':(5,6.25,10,0.5,25),'claude-opus-4-8':(5,6.25,10,0.5,25),
 'claude-opus-5-5':(4,5,8,0.2,20),'claude-fable-5-1':(10,12.5,20,0.25,50),'claude-fable-5':(10,12.5,20,1,50),
}
def usage_cost(m,u):
    p=PRICE[m]
    cc=u.get('cache_creation') or {}
    w5=cc.get('ephemeral_5m_input_tokens'); w1=cc.get('ephemeral_1h_input_tokens')
    if w5 is None and w1 is None:
        w5=u.get('cache_creation_input_tokens',0); w1=0
    return (u.get('input_tokens',0)*p[0]+(w5 or 0)*p[1]+(w1 or 0)*p[2]+u.get('cache_read_input_tokens',0)*p[3]+u.get('output_tokens',0)*p[4])/1e6
runs=[]
for rec in glob.glob(P+'/*/workflows/wf_*.json'):
    try: d=json.load(open(rec))
    except Exception as e: continue
    if d.get('workflowName')!='c05-align': continue
    sess=rec.split('/')[-3]
    runs.append((d.get('timestamp'),d['runId'],sess,d))
runs.sort()
mode=sys.argv[1] if len(sys.argv)>1 else 'last'
tot=0; segs=0; models=collections.Counter(); unknown=collections.Counter()
tok=collections.Counter()
per_run={}
for ts,rid,sess,d in runs:
    ad=f"{P}/{sess}/subagents/workflows/{rid}"
    nseg=len((d.get('args') or {}).get('seqs') or [])
    segs+=nseg
    rc=0; out=0
    for f in glob.glob(ad+'/agent-*.jsonl'):
        seen={}
        for line in open(f):
            try: j=json.loads(line)
            except: continue
            m=j.get('message') or {}
            if j.get('type')!='assistant' or 'usage' not in m: continue
            key=(m.get('id'),j.get('requestId'))
            if mode=='first':
                if key in seen: continue
                seen[key]=(m.get('model'),m['usage'])
            else:
                seen[key]=(m.get('model'),m['usage'])
        for key,(model,u) in seen.items():
            if model not in PRICE:
                unknown[model]+=1; continue
            models[model]+=1
            c=usage_cost(model,u); rc+=c; out+=u.get('output_tokens',0)
            for k in ('input_tokens','cache_read_input_tokens','cache_creation_input_tokens','output_tokens'): tok[k]+=u.get(k,0)
    per_run[rid]=(nseg,rc,out,ts)
    tot+=rc
print('mode',mode,'runs',len(runs),'segments',segs,'total $%.2f'%tot,'per seg $%.2f'%(tot/max(segs,1)))
print('models',dict(models),'unknown',dict(unknown))
print('tokens',dict(tok))
r=per_run.get('wf_025e4913-5f3'); print('wf_025e4913-5f3',r)
vals=sorted(v[1]/v[0] for v in per_run.values() if v[0])
print('per-run $/seg min %.2f median %.2f max %.2f'%(vals[0],vals[len(vals)//2],vals[-1]))
