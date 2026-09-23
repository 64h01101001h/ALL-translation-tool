import json, glob, os, collections, statistics, re
base=os.path.expanduser('~/.claude/projects/-Users-adamderickandrade-ALL-translation-tool/')
metas=glob.glob(base+'*/subagents/*.meta.json')
roles=collections.defaultdict(list)
models=collections.Counter()
for p in metas:
    try: m=json.load(open(p))
    except Exception: continue
    d=m.get('description','')
    w=d.split()[0] if d else ''
    if w not in ('Propose','Reconcile','Refute'): continue
    course=re.search(r'C0?\d\d?', d)
    course=course.group(0) if course else '?'
    jp=p.replace('.meta.json','.jsonl')
    if not os.path.exists(jp): continue
    inp=cc=cr=out=0; mdl=set(); turns=0
    for line in open(jp, errors='replace'):
        if '"usage"' not in line: continue
        try: d2=json.loads(line)
        except Exception: continue
        msg=d2.get('message') or {}
        u=msg.get('usage')
        if not isinstance(u,dict): continue
        if msg.get('model'): mdl.add(msg['model'])
        inp+=u.get('input_tokens',0); cc+=u.get('cache_creation_input_tokens',0)
        cr+=u.get('cache_read_input_tokens',0); out+=u.get('output_tokens',0); turns+=1
    roles[(w,course)].append((inp,cc,cr,out,turns))
    for x in mdl: models[(w,x)]+=1
print(f"{'role':12} {'course':6} {'n':>4} {'med_bill':>9} {'mean_bill':>9} {'mean_cread':>11} {'mean_out':>8} {'turns':>6}")
grand=collections.defaultdict(lambda:[0,0,0])
for k in sorted(roles):
    v=roles[k]
    bill=[a+b+d for a,b,c,d,t in v]
    crs=[c for a,b,c,d,t in v]; outs=[d for a,b,c,d,t in v]; tn=[t for a,b,c,d,t in v]
    print(f"{k[0]:12} {k[1]:6} {len(v):>4} {int(statistics.median(bill)):>9,} {int(statistics.mean(bill)):>9,} {int(statistics.mean(crs)):>11,} {int(statistics.mean(outs)):>8,} {statistics.mean(tn):>6.1f}")
    grand[k[0]][0]+=sum(bill); grand[k[0]][1]+=sum(crs); grand[k[0]][2]+=len(v)
print()
tb=sum(g[0] for g in grand.values())
for r,g in grand.items():
    print(f"{r:12} n={g[2]:>4} billable={g[0]:>14,} ({100*g[0]/tb:5.1f}%)  cache_read={g[1]:>14,}")
print()
for k,v in models.most_common(): print(k,v)
