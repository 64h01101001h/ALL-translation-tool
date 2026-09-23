import json,glob,os,collections
base=os.path.expanduser('~/.claude/projects/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2')
R='/Users/adamderickandrade/ALL-translation-tool/'
specs=collections.defaultdict(dict)
for wf in sorted(glob.glob(base+'/workflows/wf_*.json')):
    d=json.load(open(wf))
    if d.get('workflowName')!='c05-align' or d['status']!='completed': continue
    for jf in glob.glob(base+'/subagents/workflows/'+d['runId']+'/agent-*.jsonl'):
        last=None
        for line in open(jf):
            try: r=json.loads(line)
            except: continue
            if r.get('type')=='assistant':
                for c in (r['message'].get('content') or []):
                    if isinstance(c,dict) and c.get('type')=='tool_use' and c.get('name')=='StructuredOutput':
                        last=c['input']
        if last and 'seq' in last: specs[last['seq']][last.get('angle','?')]=last.get('spans') or []
bank=json.load(open(R+'data/alignment/alignment_full_v1.json'))['links']
att=collections.defaultdict(collections.Counter)
for l in bank:
    if l.get('eng') and l['course']!='C05': att[l['tib']][l['eng']]+=1
win=0;lose=0;tie=0;nopre=0;other=0
ex=[]
for seq,by in specs.items():
    angles=[a for a in by if a!='reconciled']
    if 'reconciled' not in by or len(angles)<2: continue
    A={s['tib']:s.get('eng') for s in by[angles[0]] if s.get('eng')}
    B={s['tib']:s.get('eng') for s in by[angles[1]] if s.get('eng')}
    Rd={s['tib']:s.get('eng') for s in by['reconciled'] if s.get('eng')}
    for t in set(A)&set(B):
        if A[t]==B[t] or t not in Rd: continue
        a,b=A[t],B[t]; r=Rd[t]
        ca,cb=att[t][a],att[t][b]
        if ca==cb: tie+=1; continue
        hi,lo=(a,b) if ca>cb else (b,a)
        if ca==0 and cb==0: nopre+=1; continue
        if r==hi: win+=1; ex.append((t,hi,max(ca,cb),lo,min(ca,cb)))
        elif r==lo: lose+=1
        else: other+=1
tot=win+lose
print('CONTESTED English choices where precedent differs: %d'%(tot+tie+other))
print('  reconciler chose the MORE-attested rendering: %d (%.1f%%)'%(win,100*win/max(tot,1)))
print('  reconciler chose the LESS-attested rendering: %d (%.1f%%)'%(lose,100*lose/max(tot,1)))
print('  chose a third option: %d; equal precedent: %d; no precedent either way: %d'%(other,tie,nopre))
print('examples (tib, chosen, count | rejected, count):')
for e in ex[:10]: print('  ',e)
