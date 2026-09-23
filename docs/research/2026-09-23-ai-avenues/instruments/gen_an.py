import json, glob, os, re, collections
D=os.path.expanduser('~/.claude/projects/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2/subagents/workflows')
A=json.load(open('agents.json'))
idx=[(a['wf'],a['f'],a['role'],a['seq']) for a in A if a['role']]
samples=[]; genruns=0; fails=0; perag=collections.Counter(); failag=collections.Counter()
catgen=0
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
                    if 'gen_alignment_page' in cmd:
                        if re.search(r'\b(cat|sed|head|grep|rg)\b[^|]*gen_alignment_page\.py',cmd): catgen+=1; continue
                        genruns+=1; perag[(wf,fn)]+=1; pending[b.get('id')]=(wf,fn,role,seq)
        if r.get('type')=='user':
            c=mg.get('content')
            if isinstance(c,list):
                for b in c:
                    if isinstance(b,dict) and b.get('type')=='tool_result' and b.get('tool_use_id') in pending:
                        meta=pending.pop(b['tool_use_id'])
                        t=b.get('content')
                        s=t if isinstance(t,str) else (' '.join(x.get('text','') for x in t if isinstance(x,dict)) if isinstance(t,list) else '')
                        bad = bool(b.get('is_error')) or bool(re.search(r'exit=[1-9]', s)) or ('REFUSE' in s.upper())
                        if bad:
                            fails+=1; failag[(meta[0],meta[1])]+=1
                            if len(samples)<6: samples.append(s[:500])
print(f'generator EXECUTIONS {genruns} = {genruns/len(idx):.2f}/agent, {genruns/204:.2f}/segment')
print(f'  failed/refused {fails} = {100*fails/genruns:.1f}% of runs')
print(f'  agents with >=1 refusal: {len(failag)} = {100*len(failag)/len(idx):.1f}%')
print(f'  refusals per agent {fails/len(idx):.2f} ; per segment {fails/204:.2f}')
print(f'  gen source READ via cat/sed/grep: {catgen}')
print('\nSAMPLE refusal outputs:')
for s in samples: print('  ---',repr(s[:280]))
