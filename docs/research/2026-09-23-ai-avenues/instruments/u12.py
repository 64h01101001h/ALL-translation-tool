import json, glob, os, collections, re
base=os.path.expanduser('~/.claude/projects/-Users-adamderickandrade-ALL-translation-tool/')
cat=collections.Counter(); nag=collections.Counter(); turns=collections.Counter()
def classify(c):
    l=c.lower()
    if 'gen_alignment_page' in l: return 'RUN GENERATOR (the proof)'
    if 'alignment_full_v1' in l or 'alignment_evidence' in l: return 'precedent lookup in 10.9MB bank'
    if 'propose_brief' in l or 'reconcile_brief' in l or 'alignment_briefs' in l: return 'read the brief'
    if 'campaign.md' in l: return 'read the campaign ledger'
    if 'corpus_segments' in l or 'hgm_spine' in l: return 'fetch the segment text'
    if re.search(r'\bmkdir\b|^\s*d=|cat > |heredoc|python3 - <<|>\s*"?\$d|cd /private/tmp', l): return 'scratch-file plumbing'
    if 'pages_c' in l or 'data/alignment/pages' in l: return 'look at landed pages'
    if 'tools/' in l: return 'read other tools'
    return 'other'
for p in glob.glob(base+'*/subagents/*.meta.json'):
    try: m=json.load(open(p))
    except Exception: continue
    d=m.get('description','')
    w=d.split()[0] if d else ''
    if w not in ('Propose','Reconcile') or 'C05' not in d: continue
    jp=p.replace('.meta.json','.jsonl'); 
    if not os.path.exists(jp): continue
    nag[w]+=1
    for line in open(jp, errors='replace'):
        try: d2=json.loads(line)
        except Exception: continue
        c=(d2.get('message') or {}).get('content')
        if not isinstance(c,list): continue
        for b in c:
            if isinstance(b,dict) and b.get('type')=='tool_use':
                turns[w]+=1
                if b.get('name')=='Bash':
                    cat[(w,classify(b.get('input',{}).get('command','')))]+=1
                else:
                    cat[(w,b.get('name')+' (file tool)')]+=1
print('agents',dict(nag),'toolcalls',dict(turns))
for w in ('Propose','Reconcile'):
    print('\n===',w,' (n=%d agents)'%nag[w])
    tot=sum(v for (a,k),v in cat.items() if a==w)
    for (a,k),v in sorted(cat.items(), key=lambda x:-x[1]):
        if a!=w: continue
        print(f'  {v:>5} ({100*v/tot:4.1f}%)  {v/nag[w]:4.1f}/agent   {k}')
