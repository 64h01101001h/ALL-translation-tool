import json,glob,os,collections
base=os.path.expanduser('~/.claude/projects/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2')
runs=[]
for wf in sorted(glob.glob(base+'/workflows/wf_*.json')):
    d=json.load(open(wf))
    if d.get('workflowName')=='c05-align' and d['status']=='completed': runs.append(d['runId'])
ch=collections.Counter(); n=collections.Counter(); segs=0
toolchars=collections.Counter()
for rid in runs:
    segs+=9
    for jf in glob.glob(base+'/subagents/workflows/'+rid+'/agent-*.jsonl'):
        seen=set()
        for line in open(jf):
            try: r=json.loads(line)
            except: continue
            if r.get('type')!='assistant': continue
            m=r['message']; mid=m.get('id')
            if mid in seen: continue
            seen.add(mid)
            for c in (m.get('content') or []):
                if not isinstance(c,dict): continue
                t=c.get('type')
                if t=='thinking': ch['thinking']+=len(c.get('thinking') or ''); n['thinking']+=1
                elif t=='text': ch['text']+=len(c.get('text') or ''); n['text']+=1
                elif t=='tool_use':
                    s=len(json.dumps(c.get('input') or {}))
                    if c.get('name')=='StructuredOutput': ch['final_spec']+=s; n['final_spec']+=1
                    else: ch['tool_args']+=s; n['tool_args']+=1; toolchars[c.get('name')]+=s
print('segments',segs)
tot=sum(ch.values())
for k,v in ch.most_common():
    print('  %-12s %9d chars (%4.1f%%)  ~%6.0f tok/segment  blocks %d'%(k,v,100*v/tot,v/3.6/segs,n[k]))
print('  TOTAL ~%.0f output tok/segment estimated from chars/3.6 (measured billed output: 104,580)'%(tot/3.6/segs))
print('tool arg chars by tool:',dict(toolchars.most_common(8)))
