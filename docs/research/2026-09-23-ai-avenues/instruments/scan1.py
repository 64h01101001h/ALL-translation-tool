import json, os, glob, re, collections
D=os.path.expanduser('~/.claude/projects/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2/subagents/workflows')
rows=[]
for wf in sorted(os.listdir(D)):
    p=os.path.join(D,wf)
    if not os.path.isdir(p): continue
    for f in sorted(glob.glob(p+'/agent-*.jsonl')):
        aid=os.path.basename(f)[6:-6]
        first=None; tools=collections.Counter(); usage={'input':0,'cache_creation':0,'cache_read':0,'output':0}
        nassist=0; nlines=0
        try:
            with open(f) as fh:
                for line in fh:
                    nlines+=1
                    try: o=json.loads(line)
                    except Exception: continue
                    t=o.get('type')
                    if t=='user' and first is None:
                        c=o.get('message',{}).get('content')
                        if isinstance(c,str): first=c
                        elif isinstance(c,list):
                            first=' '.join(x.get('text','') for x in c if isinstance(x,dict))
                    if t=='assistant':
                        nassist+=1
                        m=o.get('message',{})
                        u=m.get('usage') or {}
                        usage['input']+=u.get('input_tokens',0)
                        usage['cache_creation']+=u.get('cache_creation_input_tokens',0)
                        usage['cache_read']+=u.get('cache_read_input_tokens',0)
                        usage['output']+=u.get('output_tokens',0)
                        for b in m.get('content',[]) or []:
                            if isinstance(b,dict) and b.get('type')=='tool_use':
                                tools[b.get('name')]+=1
        except Exception as e:
            pass
        rows.append(dict(wf=wf,aid=aid,first=(first or '')[:6000],tools=dict(tools),usage=usage,nassist=nassist,nlines=nlines,path=f))
json.dump(rows,open('/private/tmp/claude-501/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2/scratchpad/agents.json','w'))
print('agents',len(rows))
