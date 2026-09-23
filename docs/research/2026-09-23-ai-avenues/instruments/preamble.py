import json,glob,os,statistics
base=os.path.expanduser('~/.claude/projects/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2')
comp=set()
for f in glob.glob(base+'/workflows/wf_*.json'):
    try: o=json.load(open(f))
    except Exception: continue
    if o.get('workflowName')=='c05-align' and o.get('status')=='completed':
        comp.add(os.path.basename(f)[:-5])
print('completed c05-align runs:',len(comp))
first=[]
for wf in comp:
    for f in glob.glob(base+'/subagents/workflows/%s/agent-*.jsonl'%wf):
        for line in open(f,encoding='utf-8',errors='replace'):
            try: o=json.loads(line)
            except Exception: continue
            m=o.get('message') or {}
            u=m.get('usage')
            if o.get('type')=='assistant' and u:
                first.append((u.get('input_tokens',0) or 0)+(u.get('cache_creation_input_tokens',0) or 0)+(u.get('cache_read_input_tokens',0) or 0))
                break
print('first-request prompt size per agent: n=%d mean %.0f median %.0f min %d max %d'%(
    len(first),statistics.mean(first),statistics.median(first),min(first),max(first)))
print('x3 agents per segment = %.0f tokens of preamble per segment'%(3*statistics.mean(first)))
