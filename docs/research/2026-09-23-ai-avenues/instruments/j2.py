import glob, json, os, collections
base = os.path.expanduser('~/.claude/projects/-Users-adamderickandrade-ALL-translation-tool')
T=collections.Counter(); runs=0; inc=0; S=R=0; lab=collections.Counter(); keys=collections.Counter()
for j in glob.glob(base + '/*/subagents/workflows/*/journal.jsonl'):
    ev=[]
    for line in open(j, encoding='utf-8', errors='replace'):
        try: ev.append(json.loads(line))
        except Exception: pass
    runs+=1
    for e in ev:
        T[e.get('type')]+=1
        if e.get('type')=='started':
            l=e.get('label') or ''
            lab[l.split(':')[0] if l else '(none)']+=1
            keys[e.get('key')]+=1
    s=sum(1 for e in ev if e.get('type')=='started'); r=sum(1 for e in ev if e.get('type')=='result')
    S+=s; R+=r
    if s>r: inc+=1
print('journals',runs,'types',dict(T)); print('started',S,'results',R,'journals with unfinished agents',inc)
print('label prefixes', lab.most_common(15))
print('distinct keys',len(keys),'keys started more than once',sum(1 for k,v in keys.items() if v>1), 'extra starts', sum(v-1 for v in keys.values() if v>1))
