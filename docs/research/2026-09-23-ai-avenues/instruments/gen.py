import json,os,glob,re,collections,statistics
B=os.path.expanduser('~/.claude/projects/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2')
WF=os.path.join(B,'workflows'); SA=os.path.join(B,'subagents','workflows')
good=[]
for p in glob.glob(os.path.join(WF,'wf_*.json')):
    d=json.load(open(p))
    if d.get('workflowName')=='c05-align' and d.get('status')=='completed':
        good.append(os.path.basename(p)[:-5])
print('runs',len(good))

# map tool_use id -> command, then find tool_result
first_run=[]; allruns=[]
per_agent=collections.Counter()
for wf in good:
    for f in glob.glob(os.path.join(SA,wf,'agent-*.jsonl')):
        cmds={}   # tool_use_id -> command string
        seq=[]    # (idx, tool_use_id) for gen_alignment_page invocations
        results={}
        lines=[]
        for line in open(f,errors='replace'):
            line=line.strip()
            if not line: continue
            try: o=json.loads(line)
            except Exception: continue
            lines.append(o)
        for o in lines:
            m=o.get('message') or {}
            c=m.get('content')
            if not isinstance(c,list): continue
            for blk in c:
                if not isinstance(blk,dict): continue
                if blk.get('type')=='tool_use':
                    inp=blk.get('input') or {}
                    cmd=inp.get('command') or ''
                    if 'gen_alignment_page.py' in str(cmd):
                        cmds[blk.get('id')]=cmd
                        seq.append(blk.get('id'))
                elif blk.get('type')=='tool_result':
                    tid=blk.get('tool_use_id')
                    if tid in cmds:
                        cc=blk.get('content')
                        txt=''
                        if isinstance(cc,str): txt=cc
                        elif isinstance(cc,list):
                            txt=' '.join(x.get('text','') for x in cc if isinstance(x,dict))
                        results[tid]=txt
        # also look at toolUseResult field style
        for o in lines:
            tur=o.get('toolUseResult')
            if isinstance(tur,dict):
                pass
        if not seq: continue
        per_agent[len(seq)]+=1
        outs=[]
        for tid in seq:
            txt=results.get(tid,'')
            refused = 'REFUSED' in txt
            # exit code echo
            mm=re.search(r'EXIT[= ]+(\d+)|exit[= ]+(\d+)',txt)
            code=None
            if mm: code=int(mm.group(1) or mm.group(2))
            outs.append((refused,code,txt[:200]))
        allruns.append((f,outs))
        first_run.append(outs[0])
n=len(first_run)
ref=sum(1 for r,c,t in first_run if r)
code0=sum(1 for r,c,t in first_run if c==0)
codenz=sum(1 for r,c,t in first_run if c is not None and c!=0)
print('agents with >=1 generator run:',n)
print('first run REFUSED text present:',ref, '%.1f%%'%(100*ref/n))
print('first run echoed exit=0:',code0,' echoed nonzero:',codenz,' no echo:',n-code0-codenz)
tot=sum(len(o) for _,o in allruns)
totref=sum(1 for _,o in allruns for r,c,t in o if r)
print('total generator runs:',tot,'runs whose result text contains REFUSED:',totref,'%.1f%%'%(100*totref/tot))
print('runs per agent mean %.2f median %d max %d'%(tot/len(allruns),statistics.median([len(o) for _,o in allruns]),max(len(o) for _,o in allruns)))
# agents whose LAST run refused
lastref=sum(1 for _,o in allruns if o[-1][0])
print('agents whose LAST generator run still REFUSED:',lastref)
# sample refusal texts
seen=0
for _,o in allruns:
    for r,c,t in o:
        if r and seen<6:
            print('  REFUSAL SAMPLE:',t.replace('\n',' | ')[:180]); seen+=1
