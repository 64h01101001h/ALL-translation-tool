import json,os,glob,statistics,re,collections
B=os.path.expanduser('~/.claude/projects/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2')
WF=os.path.join(B,'workflows')
allrun=[]
for p in glob.glob(os.path.join(WF,'wf_*.json')):
    d=json.load(open(p))
    if d.get('workflowName')!='c05-align' or d.get('status')!='completed': continue
    ags=[e for e in (d.get('workflowProgress') or []) if e.get('type')=='workflow_agent']
    segs=set()
    for a in ags:
        m=re.match(r'(propose|reconcile):(\d+)',a.get('label') or '')
        if m: segs.add(m.group(2))
    starts=[a.get('startedAt') for a in ags if a.get('startedAt')]
    lasts=[a.get('lastProgressAt') for a in ags if a.get('lastProgressAt')]
    durs=[a.get('durationMs')/1000.0 for a in ags if a.get('durationMs')]
    phase={}
    for a in ags:
        ph=a.get('phaseTitle') or '?'
        phase.setdefault(ph,[]).append(a.get('durationMs',0)/1000.0)
    wall=None
    if starts and lasts: wall=(max(lasts)-min(starts))/1000.0
    allrun.append(dict(nseg=len(segs),nag=len(ags),wall=wall,durs=durs,phase=phase,
                       tokens=[a.get('tokens',0) for a in ags],
                       tools=[a.get('toolCalls',0) for a in ags],
                       models=collections.Counter(a.get('model') for a in ags)))
print('runs:',len(allrun))
walls=[r['wall'] for r in allrun if r['wall']]
segs=sum(r['nseg'] for r in allrun)
print('segments',segs,'agents',sum(r['nag'] for r in allrun))
print('models used:',sum((r['models'] for r in allrun),collections.Counter()))
print('wall/run: median %.1f min mean %.1f min-max %.1f-%.1f'%(statistics.median(walls)/60,sum(walls)/len(walls)/60,min(walls)/60,max(walls)/60))
print('TOTAL wall %.2f h -> %.2f wall-min per segment (at 27-way fanout)'%(sum(walls)/3600,sum(walls)/60/segs))
alld=[x for r in allrun for x in r['durs']]
print('agent latency: n=%d mean %.2f min median %.2f max %.2f'%(len(alld),sum(alld)/len(alld)/60,statistics.median(alld)/60,max(alld)/60))
print('agent-minutes per segment (serial sum): %.1f'%(sum(alld)/60/segs))
ph=collections.defaultdict(list)
for r in allrun:
    for k,v in r['phase'].items(): ph[k].extend(v)
for k,v in ph.items():
    print('  phase %-12s n=%3d mean %.2f min median %.2f max %.2f'%(k,len(v),sum(v)/len(v)/60,statistics.median(v)/60,max(v)/60))
tk=[x for r in allrun for x in r['tokens']]
tl=[x for r in allrun for x in r['tools']]
print('harness tokens/agent: mean %.0f median %.0f; toolCalls/agent mean %.1f median %.0f max %d'%(sum(tk)/len(tk),statistics.median(tk),sum(tl)/len(tl),statistics.median(tl),max(tl)))
