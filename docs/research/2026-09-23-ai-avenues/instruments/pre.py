import json, glob, os, collections
base=os.path.expanduser('~/.claude/projects/-Users-adamderickandrade-ALL-translation-tool/')
for p in glob.glob(base+'*/subagents/*.meta.json'):
    m=json.load(open(p)); d=m.get('description','')
    if not d.startswith('Propose') or 'C05' not in d: continue
    jp=p.replace('.meta.json','.jsonl')
    n=0; tot=0
    for line in open(jp, errors='replace'):
        d2=json.loads(line)
        if d2.get('type')!='user': continue
        c=(d2.get('message') or {}).get('content')
        s=c if isinstance(c,str) else json.dumps(c)
        n+=1; tot+=len(s)
        if n<=2:
            print(f'--- user turn {n}: {len(s)} chars')
            # show any system-reminder headers
            import re
            for mm in re.finditer(r'<system-reminder>(.{0,120})', s, re.S):
                print('    reminder:', mm.group(1)[:110].replace('\n',' '))
        if n>=2: break
    print('agent first two user turns total chars:', tot)
    # attachments
    break
