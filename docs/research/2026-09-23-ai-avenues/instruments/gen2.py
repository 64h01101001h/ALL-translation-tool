import json, os, re, collections
D=os.path.expanduser('~/.claude/projects/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2/subagents/workflows')
A=json.load(open('agents.json'))
idx=[(a['wf'],a['f'],a['role'],a['seq']) for a in A if a['role']]
REF=re.compile(r'REFUSED:\s*\S')
EXIT=re.compile(r'(?i)\bexit\s*=\s*([1-9]\d*)')
runs=0; refus=0; byrole=collections.Counter(); refbyagent=collections.Counter(); runbyagent=collections.Counter()
kinds=collections.Counter()
for wf,fn,role,seq in idx:
    path=os.path.join(D,wf,fn); pending={}
    for l in open(path):
        try: r=json.loads(l)
        except: continue
        mg=r.get('message') or {}
        if r.get('type')=='assistant':
            for b in (mg.get('content') or []):
                if isinstance(b,dict) and b.get('type')=='tool_use' and b.get('name')=='Bash':
                    cmd=(b.get('input') or {}).get('command') or ''
                    if 'gen_alignment_page.py' in cmd and re.search(r'python3?\s+\S*gen_alignment_page\.py',cmd):
                        runs+=1; byrole[role]+=1; runbyagent[(wf,fn)]+=1; pending[b.get('id')]=(wf,fn,role)
        if r.get('type')=='user':
            c=mg.get('content')
            if isinstance(c,list):
                for b in c:
                    if isinstance(b,dict) and b.get('type')=='tool_result' and b.get('tool_use_id') in pending:
                        w,f,ro=pending.pop(b['tool_use_id'])
                        t=b.get('content')
                        s=t if isinstance(t,str) else (' '.join(x.get('text','') for x in t if isinstance(x,dict)) if isinstance(t,list) else '')
                        m=REF.search(s)
                        if m or EXIT.search(s):
                            refus+=1; refbyagent[(w,f)]+=1
                            mm=re.search(r'REFUSED:\s*s?\d*\s*(.{0,60})',s)
                            if mm: kinds[re.sub(r"['\"][^'\"]{0,40}['\"]",'X',mm.group(1)).strip()[:50]]+=1
print(f'generator EXECUTIONS {runs}  (propose {byrole["propose"]}, reconcile {byrole["reconcile"]})')
print(f'  per reconcile agent {byrole["reconcile"]/208:.2f}  per segment {runs/204:.2f}')
print(f'  REFUSALS {refus} = {100*refus/runs:.1f}% of runs; per segment {refus/204:.2f}')
print(f'  agents with >=1 refusal {len(refbyagent)} = {100*len(refbyagent)/208:.1f}% of reconcilers')
print('\ntop refusal kinds:')
for k,v in kinds.most_common(10): print(f'   {v:4d}  {k}')
