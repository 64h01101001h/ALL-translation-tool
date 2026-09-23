import json, glob, os, collections, re
P='/Users/adamderickandrade/.claude/projects/-Users-adamderickandrade-ALL-translation-tool'
files=glob.glob(P+'/*/subagents/**/*.jsonl', recursive=True)
out=collections.defaultdict(list)
for f in files:
    first=None; ts=None; model=collections.Counter()
    with open(f) as fh:
        for line in fh:
            try: r=json.loads(line)
            except: continue
            if first is None and r.get('type')=='user':
                c=r['message']['content']; first=c if isinstance(c,str) else json.dumps(c); ts=r.get('timestamp')
            if r.get('type')=='assistant': model[r['message'].get('model')]+=1
    if not first: continue
    if re.search(r'RECONCILE_BRIEF|RECONCILE BRIEF|RECONCILER',first): role='reconcile'
    elif re.search(r'PROPOSE_BRIEF|PROPOSE BRIEF',first): role='propose'
    else: continue
    grp='wf' if '/workflows/' in f else 'direct'
    m=re.search(r'Course (\d+)|C(\d\d)[:\s]',first); crs=(m.group(1) or m.group(2)) if m else '?'
    out[(role,grp)].append((ts, crs, model.most_common(1)[0][0] if model else None, f.split('/')[7]))
for k,v in sorted(out.items()):
    v.sort()
    print(k, len(v), 'first', v[0][0], 'last', v[-1][0], 'courses', collections.Counter(x[1] for x in v).most_common(6), 'models', collections.Counter(x[2] for x in v).most_common(3), 'sessions', collections.Counter(x[3][:8] for x in v).most_common(4))
