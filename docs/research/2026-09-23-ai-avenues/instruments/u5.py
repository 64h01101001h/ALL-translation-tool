import json, glob, os, collections, statistics, re
base=os.path.expanduser('~/.claude/projects/-Users-adamderickandrade-ALL-translation-tool/')
metas=glob.glob(base+'*/subagents/*.meta.json')
desc=collections.Counter()
match=[]
for p in metas:
    try: m=json.load(open(p))
    except Exception: continue
    d=m.get('description','')
    if 'propose:' in d or 'reconcile:' in d:
        match.append((p,d))
    desc[d.split(':')[0][:40]]+=1
print('matching workflow agents:', len(match))
for k,v in desc.most_common(15): print(v,repr(k))
