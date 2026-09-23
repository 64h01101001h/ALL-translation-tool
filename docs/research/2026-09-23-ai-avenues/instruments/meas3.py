import json,os,glob,collections,statistics
B=os.path.expanduser('~/.claude/projects/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2')
WF=os.path.join(B,'workflows'); SUB=os.path.join(B,'subagents','workflows')

def ids_of(wfid):
    d=os.path.join(SUB,wfid); s=set(); ag=set()
    for f in glob.glob(os.path.join(d,'agent-*.jsonl')):
        ag.add(os.path.basename(f))
        for line in open(f,errors='replace'):
            try:o=json.loads(line)
            except:continue
            if o.get('type')=='assistant':
                m=o.get('message') or {}
                if m.get('usage') and m.get('id'): s.add(m['id'])
    return s,ag

meta={}
for f in sorted(glob.glob(os.path.join(WF,'wf_*.json'))):
    d=json.load(open(f)); k=os.path.basename(f)[:-5]
    if not (d.get('workflowName') or '').startswith('c05-align'): continue
    meta[k]=d

comp=[k for k,d in meta.items() if d['workflowName']=='c05-align' and d.get('status')=='completed']
allcomp=set()
for k in comp:
    s,_=ids_of(k); allcomp|=s
print('distinct msg ids across 21 completed:',len(allcomp))

for k,d in meta.items():
    if k in comp: continue
    s,ag=ids_of(k)
    ov=len(s&allcomp)
    print(k,d['workflowName'],d.get('status'),'agents=',len(ag),'ids=',len(s),'overlap_with_completed=',ov)
    print('   top-level keys:',[x for x in d.keys()])
    print('   args:',json.dumps(d.get('args'))[:300])
    print('   tokens field:',d.get('tokens'),' durationMs:',d.get('durationMs'))

# harness per-agent tokens field
print()
print('--- harness workflowProgress agent token fields (completed runs) ---')
vals=[]
for k in comp:
    d=meta[k]
    def walk(o):
        out=[]
        if isinstance(o,dict):
            if 'tokens' in o and isinstance(o.get('tokens'),(int,float)): out.append(o['tokens'])
            for v in o.values(): out+=walk(v)
        elif isinstance(o,list):
            for v in o: out+=walk(v)
        return out
    t=walk(d)
    n=len(d.get('args',{}).get('seqs') or [])
    if t and n: vals.append(sum(t)/n)
if vals: print('harness tokens/segment: median %.0f mean %.0f n=%d'%(statistics.median(vals),statistics.mean(vals),len(vals)))
else: print('no per-agent tokens field found in wf json')
