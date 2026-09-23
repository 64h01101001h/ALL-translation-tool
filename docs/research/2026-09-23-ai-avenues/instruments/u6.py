import json, glob, os, collections, statistics
base=os.path.expanduser('~/.claude/projects/-Users-adamderickandrade-ALL-translation-tool/')
metas=glob.glob(base+'*/subagents/*.meta.json')
roles=collections.defaultdict(list)
models=collections.Counter()
for p in metas:
    try: m=json.load(open(p))
    except Exception: continue
    d=m.get('description','')
    role=None
    for key in ('Propose','Reconcile','Refute'):
        if d.startswith(key): role=key+' '+d.split()[-1]
    if not role: continue
    jp=p.replace('.meta.json','.jsonl')
    if not os.path.exists(jp): continue
    tot=bill=out=inp=cc=cr=0
    mdl=set()
    for line in open(jp, errors='replace'):
        if '"usage"' not in line: continue
        try: d2=json.loads(line)
        except Exception: continue
        msg=d2.get('message') or {}
        u=msg.get('usage')
        if not isinstance(u,dict): continue
        if msg.get('model'): mdl.add(msg['model'])
        i=u.get('input_tokens',0); c=u.get('cache_creation_input_tokens',0)
        r=u.get('cache_read_input_tokens',0); o=u.get('output_tokens',0)
        inp+=i; cc+=c; cr+=r; out+=o
    roles[role].append((inp,cc,cr,out))
    for x in mdl: models[(role,x)]+=1
print(f"{'role':18} {'n':>4} {'med_billable':>12} {'mean_bill':>10} {'mean_cacheread':>14} {'mean_out':>9}")
tot_by={}
for role in sorted(roles):
    v=roles[role]
    bill=[a+b+d for a,b,c,d in v]
    allt=[a+b+c+d for a,b,c,d in v]
    outs=[d for a,b,c,d in v]
    crs=[c for a,b,c,d in v]
    tot_by[role]=(sum(bill),sum(allt),len(v))
    print(f"{role:18} {len(v):>4} {int(statistics.median(bill)):>12,} {int(statistics.mean(bill)):>10,} {int(statistics.mean(crs)):>14,} {int(statistics.mean(outs)):>9,}")
print()
for k,v in models.most_common(20): print(k,v)
