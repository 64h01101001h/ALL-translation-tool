import json,os,glob,collections
B=os.path.expanduser('~/.claude/projects/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2')
WF=os.path.join(B,'workflows'); SUB=os.path.join(B,'subagents','workflows')
seqcount=collections.Counter(); runs=[]
for f in sorted(glob.glob(os.path.join(WF,'wf_*.json'))):
    d=json.load(open(f)); k=os.path.basename(f)[:-5]
    if not (d.get('workflowName') or '').startswith('c05-align'): continue
    sq=(d.get('args') or {}).get('seqs') or []
    runs.append((d.get('timestamp'),k,d['workflowName'],d.get('status'),sq,d.get('totalTokens'),d.get('totalToolCalls'),d.get('durationMs'),d.get('error')))
runs.sort()
for t,k,n,s,sq,tt,tc,dm,err in runs:
    rng=(str(sq[0])+'-'+str(sq[-1])) if sq else 'NO-ARGS'
    print('%s %s %-14s %-9s %-9s ntok=%-10s ntool=%-6s dur=%-8s %s'%(t[:19],k,n,s,rng,tt,tc,dm,(err or '')[:60]))
    for x in sq: seqcount[x]+=1
print()
dupes={k:v for k,v in seqcount.items() if v>1}
print('segments dispatched more than once:',dict(sorted(dupes.items())))
print('distinct seqs dispatched:',len(seqcount),' total dispatches:',sum(seqcount.values()))
