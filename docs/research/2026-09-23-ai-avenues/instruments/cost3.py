import json, glob, os, re, collections
D=os.path.expanduser('~/.claude/projects/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2/subagents/workflows')
out=[]
for wf in sorted(os.listdir(D)):
    for f in sorted(glob.glob(os.path.join(D,wf,'*.jsonl'))):
        role=None;seq=None;angle=None;firstrow=None
        byid={}; raw=collections.Counter(); nrows=0; toolcalls=0; times=[]; tools=collections.Counter()
        with open(f) as fh:
            for i,l in enumerate(fh):
                try: r=json.loads(l)
                except: continue
                mg=r.get('message') or {}
                c=mg.get('content')
                txt=c if isinstance(c,str) else (' '.join(x.get('text','') for x in c if isinstance(x,dict)) if isinstance(c,list) else '')
                if role is None and r.get('type')=='user':
                    m1=re.search(r'propose the full-depth alignment for (C\d+) segment (\d+)',txt)
                    m2=re.search(r'reconcile the two independent proposals for (C\d+) segment (\d+)',txt)
                    if m2: role='reconcile'; seq=int(m2.group(2))
                    elif m1:
                        role='propose'; seq=int(m1.group(2))
                        angle='tibetan-first' if 'YOUR ANGLE IS TIBETAN-FIRST' in txt else ('english-first' if 'YOUR ANGLE IS ENGLISH-FIRST' in txt else None)
                if r.get('type')=='assistant':
                    for blk in (mg.get('content') or []):
                        if isinstance(blk,dict) and blk.get('type')=='tool_use':
                            toolcalls+=1; tools[blk.get('name')]+=1
                if r.get('timestamp'): times.append(r['timestamp'])
                u=mg.get('usage')
                if u:
                    nrows+=1
                    mid=mg.get('id')
                    d=byid.setdefault(mid,{'input':0,'cw':0,'cr':0,'out':0})
                    d['input']=max(d['input'],u.get('input_tokens',0)); d['cw']=max(d['cw'],u.get('cache_creation_input_tokens',0))
                    d['cr']=max(d['cr'],u.get('cache_read_input_tokens',0)); d['out']=max(d['out'],u.get('output_tokens',0))
                    raw['input']+=u.get('input_tokens',0); raw['cw']+=u.get('cache_creation_input_tokens',0)
                    raw['cr']+=u.get('cache_read_input_tokens',0); raw['out']+=u.get('output_tokens',0)
        tot={k:sum(d[k] for d in byid.values()) for k in ('input','cw','cr','out')}
        out.append(dict(wf=wf,f=os.path.basename(f),role=role,seq=seq,angle=angle,ded=tot,raw=dict(raw),
                        nrows=nrows,nids=len(byid),tools=toolcalls,toolnames=dict(tools),
                        t0=min(times) if times else None,t1=max(times) if times else None))
json.dump(out,open('agents.json','w'))
ag=[a for a in out if a['role']]
print('total agent files',len(out),'c05 agents',len(ag))
print(collections.Counter(a['role'] for a in ag))
segs=collections.defaultdict(list)
for a in ag: segs[a['seq']].append(a)
print('distinct segs',len(segs), 'dist', collections.Counter(len(v) for v in segs.values()))
wfs=collections.Counter(a['wf'] for a in ag)
print('workflows with c05 agents',len(wfs), sorted(wfs.values()))
