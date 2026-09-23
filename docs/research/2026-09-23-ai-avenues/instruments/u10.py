import json, glob, os, collections, statistics
base=os.path.expanduser('~/.claude/projects/-Users-adamderickandrade-ALL-translation-tool/')
metas=glob.glob(base+'*/subagents/*.meta.json')
rows=collections.defaultdict(lambda: collections.defaultdict(list))
for p in metas:
    try: m=json.load(open(p))
    except Exception: continue
    d=m.get('description','')
    w=d.split()[0] if d else ''
    if w not in ('Propose','Reconcile') or 'C05' not in d: continue
    jp=p.replace('.meta.json','.jsonl')
    if not os.path.exists(jp): continue
    seq=[]
    for line in open(jp, errors='replace'):
        if '"usage"' not in line: continue
        try: d2=json.loads(line)
        except Exception: continue
        u=(d2.get('message') or {}).get('usage')
        if not isinstance(u,dict): continue
        seq.append(u)
    if not seq: continue
    u=seq[0]
    rows[w]['in'].append(u.get('input_tokens',0))
    rows[w]['cc'].append(u.get('cache_creation_input_tokens',0))
    rows[w]['cr'].append(u.get('cache_read_input_tokens',0))
for w in rows:
    print(w)
    for k in ('in','cc','cr'):
        v=rows[w][k]
        print(f'   first-turn {k}: median {int(statistics.median(v)):>7,}  mean {int(statistics.mean(v)):>7,}')
