import json, glob, os, collections, re
P='/Users/adamderickandrade/.claude/projects/-Users-adamderickandrade-ALL-translation-tool'
files=glob.glob(P+'/*/subagents/**/*.jsonl', recursive=True)
agg=collections.defaultdict(collections.Counter); models=collections.defaultdict(collections.Counter)
def classify(f):
    meta=f[:-6]+'.meta.json'
    d=''
    if os.path.exists(meta):
        try: d=json.load(open(meta)).get('description','') or ''
        except: pass
    first=''
    try:
        with open(f) as fh:
            for line in fh:
                r=json.loads(line)
                if r.get('type')=='user':
                    c=r['message']['content']; first=c if isinstance(c,str) else json.dumps(c); break
    except: pass
    s=(d+' '+first[:3000])
    if re.search(r'RECONCILE_BRIEF|RECONCILE BRIEF|^Reconcile', s): return 'reconcile'
    if re.search(r'PROPOSE_BRIEF|PROPOSE BRIEF|^Propose', s): return 'propose'
    return None
for f in files:
    role=classify(f)
    if not role: continue
    seen=set(); u=collections.Counter(); n=0
    with open(f) as fh:
        for line in fh:
            try: r=json.loads(line)
            except: continue
            if r.get('type')!='assistant': continue
            m=r.get('message',{}); mid=m.get('id')
            if mid in seen: continue
            seen.add(mid); us=m.get('usage') or {}; n+=1
            for k in ('input_tokens','cache_creation_input_tokens','cache_read_input_tokens','output_tokens'):
                u[k]+=us.get(k,0) or 0
            models[role][m.get('model')]+=1
    if n==0: continue
    agg[role].update(u); agg[role]['agents']+=1; agg[role]['turns']+=n
tot=0
for role,u in agg.items():
    a=u['agents']
    cost=(u['input_tokens']*5+u['cache_creation_input_tokens']*6.25+u['cache_read_input_tokens']*0.5+u['output_tokens']*25)/1e6
    tot+=cost
    print(role,'agents',a,'turns/agent %.1f'%(u['turns']/a),
      '| per agent: in %.0f cw %.0f cr %.0f out %.0f'%(u['input_tokens']/a,u['cache_creation_input_tokens']/a,u['cache_read_input_tokens']/a,u['output_tokens']/a),
      '| $/agent@5/25 %.3f'%(cost/a),'| total $%.0f'%cost)
    agg[role]['cost']=cost
for role in agg: print(role,'share of cost %.3f'%(agg[role]['cost']/tot))
for role in models: print(role, models[role].most_common(4))
