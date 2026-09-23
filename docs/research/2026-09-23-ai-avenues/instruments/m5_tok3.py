import json,glob,os,re,collections,statistics
D=os.path.expanduser('~/.claude/projects/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2/subagents/workflows/')
wfagg=collections.defaultdict(lambda: dict(bill=0,tot=0,n=0,segs=set(),align=0,tools=0))
for wf in sorted(os.listdir(D)):
    for f in sorted(glob.glob(os.path.join(D,wf,'*.jsonl'))):
        seen=set(); inp=ccre=cread=out=tools=0; prompt=''
        for line in open(f,errors='replace'):
            try: r=json.loads(line)
            except: continue
            if r.get('type')=='user' and not prompt:
                c=r.get('message',{}).get('content')
                if isinstance(c,str): prompt=c
            if r.get('type')=='assistant':
                m=r.get('message',{}); mid=m.get('id'); u=m.get('usage')
                if not u: continue
                if mid and mid in seen: continue
                if mid: seen.add(mid)
                inp+=u.get('input_tokens',0); ccre+=u.get('cache_creation_input_tokens',0)
                cread+=u.get('cache_read_input_tokens',0); out+=u.get('output_tokens',0)
                for b in m.get('content',[]) or []:
                    if isinstance(b,dict) and b.get('type')=='tool_use': tools+=1
        a=wfagg[wf]; a['bill']+=inp+ccre+out; a['tot']+=inp+ccre+cread+out; a['n']+=1; a['tools']+=tools
        if re.search(r'PROPOSE_BRIEF|RECONCILE_BRIEF|full-depth alignment|alignment campaign',prompt or ''):
            a['align']+=1
            for m in re.finditer(r"seq\s*=\s*(\d{1,4})\b",prompt):
                if m.group(1)!='<SEQ>': a['segs'].add(int(m.group(1)))
            for m in re.finditer(r"\bC\d\d:(\d{1,4})\b",prompt): a['segs'].add(int(m.group(1)))
            for m in re.finditer(r"SEGMENTS? (?:TO [A-Z]+|ASSIGNED)[^\n]*?:?\s*([\d,\s]+)",prompt):
                for x in re.findall(r'\d+',m.group(1)): a['segs'].add(int(x))
rows=[(wf,a) for wf,a in wfagg.items() if a['align']>0]
print('alignment workflows:',len(rows))
print("%-20s %6s %6s %5s %12s %14s %10s"%("workflow","agents","align","segs","billable","total","bill/seg"))
tb=tt=ts=0
for wf,a in sorted(rows,key=lambda x:-x[1]['bill']):
    s=len(a['segs'])
    print("%-20s %6d %6d %5d %12d %14d %10s"%(wf,a['n'],a['align'],s,a['bill'],a['tot'], ('%d'%(a['bill']/s)) if s else '-'))
    if s: tb+=a['bill']; tt+=a['tot']; ts+=s
print("\nAGGREGATE over workflows with identifiable segments: segments=%d billable=%d total=%d"%(ts,tb,tt))
print("  billable per segment = %d ; total(incl cache_read) per segment = %d"%(tb/ts, tt/ts))
