import json, glob, os, collections, statistics
base=os.path.expanduser('~/.claude/projects/-Users-adamderickandrade-ALL-translation-tool/')
metas=glob.glob(base+'*/subagents/*.meta.json')
first=collections.defaultdict(list); last=collections.defaultdict(list)
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
        seq.append((u.get('input_tokens',0),u.get('cache_creation_input_tokens',0),u.get('cache_read_input_tokens',0),u.get('output_tokens',0)))
    if not seq: continue
    first[w].append(seq[0][0]+seq[0][1]+seq[0][2])
    last[w].append(seq[-1][0]+seq[-1][1]+seq[-1][2])
for w in first:
    print(w, 'n',len(first[w]))
    print('   first-turn context tokens: median', int(statistics.median(first[w])), 'mean', int(statistics.mean(first[w])))
    print('   last-turn  context tokens: median', int(statistics.median(last[w])), 'mean', int(statistics.mean(last[w])))
