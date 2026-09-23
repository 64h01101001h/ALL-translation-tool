import json, glob, os, re, collections
D=os.path.expanduser('~/.claude/projects/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2/subagents/workflows')
A=json.load(open('agents.json'))
idx=[(a['wf'],a['f'],a['role'],a['seq']) for a in A if a['role']]
tn=collections.Counter(); bashcat=collections.Counter(); genruns=0; genfail=0
agents_with_genfail=set(); genruns_by_agent=collections.Counter()
readfiles=collections.Counter()
for wf,fn,role,seq in idx:
    path=os.path.join(D,wf,fn)
    pending={}
    for l in open(path):
        try: r=json.loads(l)
        except: continue
        mg=r.get('message') or {}
        if r.get('type')=='assistant':
            for b in (mg.get('content') or []):
                if isinstance(b,dict) and b.get('type')=='tool_use':
                    name=b.get('name'); tn[name]+=1
                    inp=b.get('input') or {}
                    if name=='Bash':
                        cmd=(inp.get('command') or '')
                        if 'gen_alignment_page' in cmd:
                            bashcat['gen_alignment_page']+=1; genruns+=1; genruns_by_agent[(wf,fn)]+=1
                            pending[b.get('id')]=('gen',wf,fn)
                        elif 'sqlite3' in cmd: bashcat['sqlite3']+=1
                        elif 'alignment_full_v1' in cmd or 'alignment_evidence' in cmd: bashcat['bank grep/parse']+=1
                        elif re.search(r'\b(grep|rg)\b',cmd): bashcat['grep other']+=1
                        elif re.search(r'\b(cat|head|sed|tail|wc)\b',cmd): bashcat['cat/head/sed']+=1
                        elif 'python' in cmd: bashcat['python other']+=1
                        else: bashcat['other bash']+=1
                    elif name=='Read':
                        p=(inp.get('file_path') or '')
                        readfiles[os.path.basename(p)]+=1
        if r.get('type')=='user':
            c=mg.get('content')
            if isinstance(c,list):
                for b in c:
                    if isinstance(b,dict) and b.get('type')=='tool_result' and b.get('tool_use_id') in pending:
                        kind,w,f=pending.pop(b['tool_use_id'])
                        txt=b.get('content')
                        s=txt if isinstance(txt,str) else ' '.join(x.get('text','') for x in txt if isinstance(x,dict)) if isinstance(txt,list) else ''
                        if b.get('is_error') or re.search(r'exit=[1-9]|REFUS|Traceback|Error:', s):
                            genfail+=1; agents_with_genfail.add((w,f))
print('TOOL CALLS across', len(idx), 'c05 agents')
for k,v in tn.most_common(12): print(f'  {k:18s} {v:6d}  ({v/len(idx):.2f}/agent)')
print('\nBASH breakdown')
for k,v in bashcat.most_common(): print(f'  {k:22s} {v:6d}  ({v/len(idx):.2f}/agent)')
print('\nTOP Read targets')
for k,v in readfiles.most_common(12): print(f'  {k:42s} {v:5d}')
print(f'\ngenerator runs {genruns} = {genruns/len(idx):.2f}/agent ; runs that look FAILED {genfail} = {100*genfail/max(genruns,1):.1f}%')
print(f'agents with >=1 failed generator run: {len(agents_with_genfail)} = {100*len(agents_with_genfail)/len(idx):.1f}% of agents')
segs=collections.Counter()
for (w,f) in agents_with_genfail:
    pass
