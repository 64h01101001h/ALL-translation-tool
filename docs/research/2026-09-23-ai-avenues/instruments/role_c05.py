import json, glob, collections, re, statistics
P='/Users/adamderickandrade/.claude/projects/-Users-adamderickandrade-ALL-translation-tool'
files=glob.glob(P+'/*/subagents/**/*.jsonl', recursive=True)
K=('input_tokens','cache_creation_input_tokens','cache_read_input_tokens','output_tokens')
res=collections.defaultdict(list)
for f in files:
    first=None; msgs={}; gens=0
    with open(f) as fh:
        for line in fh:
            try: r=json.loads(line)
            except: continue
            if first is None and r.get('type')=='user':
                c=r['message']['content']; first=c if isinstance(c,str) else json.dumps(c)
            if r.get('type')=='assistant':
                m=r['message']; cur=msgs.setdefault(m.get('id'),collections.Counter())
                for k in K: cur[k]=max(cur[k],(m.get('usage') or {}).get(k,0) or 0)
                if 'gen_alignment_page.py' in json.dumps(m.get('content',''))[:20000] or 'span_id_compiler' in json.dumps(m.get('content',''))[:20000]: gens+=1
    if not first or not msgs: continue
    if re.search(r'RECONCILE_BRIEF|RECONCILE BRIEF|RECONCILER',first): role='reconcile'
    elif re.search(r'PROPOSE_BRIEF|PROPOSE BRIEF',first): role='propose'
    else: continue
    m=re.search(r'Course (\d+)|C(\d\d)[:\s]',first); crs=(m.group(1) or m.group(2)) if m else '?'
    if crs.lstrip('0')!='5': continue
    grp='wf' if '/workflows/' in f else 'direct'
    u=collections.Counter()
    for c in msgs.values(): u.update(c)
    cost=(u['input_tokens']*5+u['cache_creation_input_tokens']*6.25+u['cache_read_input_tokens']*0.5+u['output_tokens']*25)/1e6
    res[(role,grp)].append((cost,len(msgs),u['output_tokens'],gens,len(first)))
for k,v in sorted(res.items()):
    print(k,'n',len(v),'median $%.2f'%statistics.median(x[0] for x in v),'mean $%.2f'%statistics.mean(x[0] for x in v),
      'median turns',statistics.median(x[1] for x in v),'median out',statistics.median(x[2] for x in v),
      'median gen-calls',statistics.median(x[3] for x in v),'median prompt chars',statistics.median(x[4] for x in v))
