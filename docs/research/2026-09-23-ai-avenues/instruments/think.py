import json, glob, os, re, collections, statistics
D=os.path.expanduser('~/.claude/projects/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2/subagents/workflows')
A=json.load(open('agents.json'))
idx={(a['wf'],a['f']):a for a in A if a['role']}
rows=[]
for (wf,fn),a in idx.items():
    path=os.path.join(D,wf,fn)
    byid={}
    order=[]
    thinkchars=0; textchars=0
    for l in open(path):
        try: r=json.loads(l)
        except: continue
        mg=r.get('message') or {}
        if r.get('type')!='assistant': continue
        for blk in (mg.get('content') or []):
            if isinstance(blk,dict):
                if blk.get('type')=='thinking': thinkchars+=len(blk.get('thinking') or '')
                elif blk.get('type')=='text': textchars+=len(blk.get('text') or '')
        u=mg.get('usage')
        if not u: continue
        mid=mg.get('id')
        if mid not in byid: order.append(mid)
        d=byid.setdefault(mid,{'out':0,'think':0})
        d['out']=max(d['out'],u.get('output_tokens',0))
        det=u.get('output_tokens_details') or {}
        d['think']=max(d['think'],det.get('thinking_tokens',0))
    tot_out=sum(d['out'] for d in byid.values())
    tot_think=sum(d['think'] for d in byid.values())
    last_out=byid[order[-1]]['out'] if order else 0
    rows.append(dict(role=a['role'],seq=a['seq'],tools=a['tools'],reqs=len(byid),out=tot_out,
                     think=tot_think,last_out=last_out,thinkchars=thinkchars,textchars=textchars))
json.dump(rows,open('think.json','w'))
for role in ('propose','reconcile'):
    v=[r for r in rows if r['role']==role]
    n=len(v)
    print(f'--- {role} n={n}')
    print(f"  out mean {sum(r['out'] for r in v)/n:,.0f}  thinking_tokens field mean {sum(r['think'] for r in v)/n:,.0f}")
    print(f"  thinking CHARS mean {sum(r['thinkchars'] for r in v)/n:,.0f}  text chars {sum(r['textchars'] for r in v)/n:,.0f}")
    print(f"  last-request output mean {sum(r['last_out'] for r in v)/n:,.0f}  (= final structured emission + its thinking)")
    print(f"  requests mean {sum(r['reqs'] for r in v)/n:.1f}  tools mean {sum(r['tools'] for r in v)/n:.1f}")
# correlation tools vs output
import math
v=[r for r in rows]
xs=[r['tools'] for r in v]; ys=[r['out'] for r in v]
mx=sum(xs)/len(xs); my=sum(ys)/len(ys)
cov=sum((x-mx)*(y-my) for x,y in zip(xs,ys))
sx=math.sqrt(sum((x-mx)**2 for x in xs)); sy=math.sqrt(sum((y-my)**2 for y in ys))
print(f'\ncorr(tool calls, output tokens) r={cov/(sx*sy):.3f}  n={len(v)}')
# quartiles of tool use
v2=sorted(v,key=lambda r:r['tools'])
q=len(v2)//4
for i,label in enumerate(['Q1 fewest tools','Q2','Q3','Q4 most tools']):
    chunk=v2[i*q:(i+1)*q]
    print(f'  {label}: tools {sum(r["tools"] for r in chunk)/len(chunk):.1f}  output {sum(r["out"] for r in chunk)/len(chunk):,.0f}')
