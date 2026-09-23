import json, os, glob, re
D=os.path.expanduser('~/.claude/projects/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2/subagents/workflows')
out=[]
for wf in sorted(os.listdir(D)):
    p=os.path.join(D,wf)
    if not os.path.isdir(p): continue
    for f in sorted(glob.glob(p+'/agent-*.jsonl')):
        lines=[]
        for l in open(f,errors='replace'):
            l=l.strip()
            if not l: continue
            try: lines.append(json.loads(l))
            except: pass
        if not lines: continue
        head=json.dumps(lines[0])
        if not ('PROPOSE_BRIEF' in head or 'RECONCILE_BRIEF' in head or 'full-depth alignment' in head): continue
        prompt=''
        m0=lines[0].get('message',{})
        c=m0.get('content')
        prompt = c if isinstance(c,str) else (' '.join(b.get('text','') for b in c if isinstance(b,dict)) if isinstance(c,list) else '')
        kind = 'reconcile' if 'YOUR TASK: reconcile' in prompt else ('propose' if 'YOUR TASK: propose' in prompt else 'unknown')
        mseq=re.search(r'segment (\d+)',prompt); seq=mseq.group(1) if mseq else None
        # tool results sizes
        results={}
        for o in lines:
            if o.get('type')!='user': continue
            mm=o.get('message',{}) ; cc=mm.get('content')
            if isinstance(cc,list):
                for b in cc:
                    if isinstance(b,dict) and b.get('type')=='tool_result':
                        t=b.get('content')
                        if isinstance(t,list): t=' '.join(x.get('text','') for x in t if isinstance(x,dict))
                        results[b.get('tool_use_id')] = len(t or '')
        byid={}; order=[]
        for o in lines:
            if o.get('type')!='assistant': continue
            mm=o.get('message',{}); mid=mm.get('id'); u=mm.get('usage',{}) or {}
            if mid not in byid:
                byid[mid]=dict(i=u.get('input_tokens',0) or 0, cc=u.get('cache_creation_input_tokens',0) or 0,
                               cr=u.get('cache_read_input_tokens',0) or 0, o=0, tools=[], think=0, textlen=0,
                               model=mm.get('model'))
                order.append(mid)
            e=byid[mid]
            e['o']=max(e['o'], u.get('output_tokens',0) or 0)
            for b in mm.get('content',[]) or []:
                if not isinstance(b,dict): continue
                if b.get('type')=='tool_use':
                    inp=b.get('input',{}) or {}
                    key=None
                    if b.get('name')=='Bash': key=inp.get('command','')
                    elif b.get('name') in ('Read','Write','Edit'): key=str(inp.get('file_path',''))
                    else: key=''
                    rec=(b.get('name'), key, b.get('id'), results.get(b.get('id'),0))
                    if not any(r[2]==b.get('id') for r in e['tools']): e['tools'].append(rec)
                elif b.get('type')=='thinking': e['think']=max(e['think'], len(b.get('thinking','')))
                elif b.get('type')=='text': e['textlen']=max(e['textlen'], len(b.get('text','')))
        for mid in order:
            e=byid[mid]
            out.append(dict(wf=wf, agent=os.path.basename(f), kind=kind, seq=seq, mid=mid,
                            i=e['i'], cc=e['cc'], cr=e['cr'], o=e['o'], think=e['think'], textlen=e['textlen'],
                            model=e['model'], tools=[[t[0],t[1],t[3]] for t in e['tools']]))
json.dump(out, open('acalls.json','w'))
print('alignment calls:', len(out), 'agents:', len({(c['wf'],c['agent']) for c in out}))
