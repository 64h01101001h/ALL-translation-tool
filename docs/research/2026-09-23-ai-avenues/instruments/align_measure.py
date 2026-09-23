import json,os,glob,re,collections,statistics,subprocess
D=os.path.expanduser('~/.claude/projects/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2/subagents/workflows')
files=subprocess.run(['grep','-rl','full-depth alignment',D,'--include=*.jsonl'],capture_output=True,text=True).stdout.split()
rows=[]
pat=re.compile(r'(propose the full-depth alignment for (C\d+) segment (\d+)|reconcile the two independent proposals for (C\d+) segment (\d+))')
for f in files:
    if f.endswith('journal.jsonl'): continue
    per={}; naive=collections.Counter(); blocks=0; tools=0; ts=[]; role=None; course=None; seq=None
    toolnames=collections.Counter()
    for line in open(f,errors='replace'):
        try: d=json.loads(line)
        except: continue
        if d.get('timestamp'): ts.append(d['timestamp'])
        t=d.get('type')
        if t=='user' and role is None:
            s=json.dumps(d.get('message') or {})
            m=pat.search(s)
            if m:
                if m.group(2): role,course,seq='propose',m.group(2),int(m.group(3))
                else: role,course,seq='reconcile',m.group(4),int(m.group(5))
        if t!='assistant': continue
        m2=d.get('message') or {}; u=m2.get('usage') or {}
        blocks+=1
        rid=d.get('requestId') or ('nr%d'%blocks)
        cur=per.setdefault(rid,dict(i=0,cw=0,cr=0,o=0))
        cur['i']=max(cur['i'],u.get('input_tokens') or 0)
        cur['cw']=max(cur['cw'],u.get('cache_creation_input_tokens') or 0)
        cur['cr']=max(cur['cr'],u.get('cache_read_input_tokens') or 0)
        cur['o']=max(cur['o'],u.get('output_tokens') or 0)
        naive['i']+=u.get('input_tokens') or 0; naive['cw']+=u.get('cache_creation_input_tokens') or 0
        naive['cr']+=u.get('cache_read_input_tokens') or 0; naive['o']+=u.get('output_tokens') or 0
        for b in (m2.get('content') or []):
            if isinstance(b,dict) and b.get('type')=='tool_use':
                tools+=1; toolnames[b.get('name')]+=1
    if not per: continue
    a=collections.Counter()
    for v in per.values():
        for k,x in v.items(): a[k]+=x
    rows.append(dict(f=f,wf=f.split('/wf_')[1].split('/')[0],role=role,course=course,seq=seq,
        req=len(per),blocks=blocks,tools=tools,i=a['i'],cw=a['cw'],cr=a['cr'],o=a['o'],
        ni=naive['i'],ncw=naive['cw'],ncr=naive['cr'],no=naive['o'],
        t0=min(ts) if ts else None,t1=max(ts) if ts else None,toolnames=dict(toolnames)))
json.dump(rows,open('align_rows.json','w'))
print('files',len(files),'measured',len(rows))
print('role counts',collections.Counter(r['role'] for r in rows))
print('course counts',collections.Counter(r['course'] for r in rows))
