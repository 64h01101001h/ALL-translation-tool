import json,glob,os,collections,statistics
base=os.path.expanduser('~/.claude/projects/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2')
specs=collections.defaultdict(dict)   # (run,seq) -> {angle:spans}
for wf in sorted(glob.glob(base+'/workflows/wf_*.json')):
    d=json.load(open(wf))
    if d.get('workflowName')!='c05-align' or d['status']!='completed': continue
    rid=d['runId']
    for jf in glob.glob(base+'/subagents/workflows/'+rid+'/agent-*.jsonl'):
        last=None
        for line in open(jf):
            try: r=json.loads(line)
            except: continue
            if r.get('type')=='assistant':
                for c in (r['message'].get('content') or []):
                    if isinstance(c,dict) and c.get('type')=='tool_use' and c.get('name')=='StructuredOutput':
                        last=c['input']
        if last and 'seq' in last:
            specs[last['seq']][last.get('angle','?')]=last.get('spans') or []
def key(s): return (s.get('tib'),s.get('eng'),s.get('d'))
jacs=[];inA=[];inB=[];inboth=[];neither=[];orig=[];dropped=0;unan=0;segs=0
for seq,by in specs.items():
    angles=[a for a in by if a!='reconciled']
    if 'reconciled' not in by or len(angles)<2: continue
    segs+=1
    A=set(map(key,by[angles[0]])); B=set(map(key,by[angles[1]])); Rr=set(map(key,by['reconciled']))
    jacs.append(len(A&B)/max(len(A|B),1))
    if Rr:
        inboth.append(len(Rr&A&B)/len(Rr)); inA.append(len(Rr&A)/len(Rr))
        inB.append(len(Rr&B)/len(Rr)); neither.append(len(Rr-(A|B))/len(Rr))
    u=A&B
    unan+=len(u); dropped+=len(u-Rr)
print('segments with full triple:',segs)
print('Jaccard A vs B: mean %.3f median %.3f'%(statistics.mean(jacs),statistics.median(jacs)))
print('final spans in BOTH proposals: %.1f%%'%(100*statistics.mean(inboth)))
print('final spans in A: %.1f%%  in B: %.1f%%  in NEITHER (original to reconciler): %.1f%%'%(100*statistics.mean(inA),100*statistics.mean(inB),100*statistics.mean(neither)))
print('unanimous spans %d, dropped by reconciler %d (%.2f%%)'%(unan,dropped,100*dropped/unan))
