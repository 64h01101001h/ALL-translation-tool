import json,os,collections
rows=json.load(open('agents.json'))
out=[]
for r in rows:
    if r['kind'] not in ('propose','reconcile'): continue
    cmds=[]
    with open(r['path']) as fh:
        for line in fh:
            try:o=json.loads(line)
            except Exception:continue
            if o.get('type')!='assistant':continue
            for b in o.get('message',{}).get('content',[]) or []:
                if isinstance(b,dict) and b.get('type')=='tool_use':
                    if b.get('name')=='Bash':
                        cmds.append(('Bash',b.get('input',{}).get('command','')))
                    elif b.get('name')=='Read':
                        cmds.append(('Read',b.get('input',{}).get('file_path','')))
                    elif b.get('name')=='Write':
                        cmds.append(('Write',b.get('input',{}).get('file_path','')))
    out.append(dict(wf=r['wf'],aid=r['aid'],kind=r['kind'],seq=r['seq'],angle=r.get('angle'),cmds=cmds))
json.dump(out,open('cmds.json','w'))
print(len(out), sum(len(o['cmds']) for o in out))
