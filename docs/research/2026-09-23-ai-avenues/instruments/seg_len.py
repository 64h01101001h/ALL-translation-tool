import json, glob, collections, re, statistics, sqlite3
P='/Users/adamderickandrade/.claude/projects/-Users-adamderickandrade-ALL-translation-tool'
db=sqlite3.connect('file:/Users/adamderickandrade/ALL-translation-tool/build/hgm_spine_v27_2.db?mode=ro',uri=True)
files=glob.glob(P+'/*/subagents/**/*.jsonl', recursive=True)
res=collections.defaultdict(list); eff=collections.Counter()
for f in files:
    with open(f) as fh:
        for line in fh:
            try: r=json.loads(line)
            except: continue
            if r.get('type')=='user':
                c=r['message']['content']; first=c if isinstance(c,str) else json.dumps(c); break
        else: continue
    if re.search(r'RECONCILE_BRIEF|RECONCILE BRIEF|RECONCILER',first): role='reconcile'
    elif re.search(r'PROPOSE_BRIEF|PROPOSE BRIEF',first): role='propose'
    else: continue
    m=re.search(r'Course (\d+)|C(\d\d)[:\s]',first); crs=(m.group(1) or m.group(2)) if m else '?'
    if crs.lstrip('0')!='5': continue
    s=re.search(r'SEQ\s*=\s*(\d+)|segment (\d+)|C05:(\d+)|seq[=: ]+(\d+)',first)
    if not s: continue
    seq=int([g for g in s.groups() if g][0])
    row=db.execute("select length(wylie),length(english) from corpus_segments where course='C05' and seq=?",(seq,)).fetchone()
    if not row: continue
    grp='wf' if '/workflows/' in f else 'direct'
    res[(role,grp)].append(row[0])
for k,v in sorted(res.items()):
    print(k,'n',len(v),'median wylie chars',statistics.median(v),'mean',round(statistics.mean(v)))
