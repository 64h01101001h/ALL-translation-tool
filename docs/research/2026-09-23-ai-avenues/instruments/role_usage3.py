import json, glob, os, collections, re
P='/Users/adamderickandrade/.claude/projects/-Users-adamderickandrade-ALL-translation-tool'
files=glob.glob(P+'/*/subagents/**/*.jsonl', recursive=True)
K=('input_tokens','cache_creation_input_tokens','cache_read_input_tokens','output_tokens')
def classify(f):
    meta=f[:-6]+'.meta.json'; d=''
    if os.path.exists(meta):
        try: d=json.load(open(meta)).get('description','') or ''
        except: pass
    first=''
    with open(f) as fh:
        for line in fh:
            try: r=json.loads(line)
            except: continue
            if r.get('type')=='user':
                c=r['message']['content']; first=c if isinstance(c,str) else json.dumps(c); break
    s=(d+' '+first)
    if re.search(r'RECONCILE_BRIEF|RECONCILE BRIEF|RECONCILER|^Reconcile', s): return 'reconcile'
    if re.search(r'PROPOSE_BRIEF|PROPOSE BRIEF|^Propose', s): return 'propose'
    return None
agg=collections.defaultdict(collections.Counter)
for f in files:
    role=classify(f)
    if not role: continue
    grp='wf' if '/workflows/' in f else 'direct'
    msgs={}
    with open(f) as fh:
        for line in fh:
            try: r=json.loads(line)
            except: continue
            if r.get('type')!='assistant': continue
            m=r.get('message',{}); mid=m.get('id'); us=m.get('usage') or {}
            cur=msgs.setdefault(mid,collections.Counter())
            for k in K: cur[k]=max(cur[k], us.get(k,0) or 0)
    if not msgs: continue
    key=(role,grp)
    for c in msgs.values(): agg[key].update(c)
    agg[key]['agents']+=1; agg[key]['turns']+=len(msgs)
for key,u in sorted(agg.items()):
    a=u['agents']
    cost=(u['input_tokens']*5+u['cache_creation_input_tokens']*6.25+u['cache_read_input_tokens']*0.5+u['output_tokens']*25)/1e6
    print(key,'agents',a,'turns/agent %.1f'%(u['turns']/a),
      '| per agent: in %.0f cw %.0f cr %.0f out %.0f'%(u['input_tokens']/a,u['cache_creation_input_tokens']/a,u['cache_read_input_tokens']/a,u['output_tokens']/a),
      '| $/agent@5/25 %.3f'%(cost/a))
