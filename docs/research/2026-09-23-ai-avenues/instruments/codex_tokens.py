import json,glob,os,collections
CWD='/Users/adamderickandrade/Documents/ChatGPT/Geshe Michael Roach Tib _ Eng Alignment'
seen=set()
agg=collections.defaultdict(lambda: collections.Counter())
byday=collections.defaultdict(lambda: collections.Counter())
roles=collections.Counter()
nresp=collections.Counter()
for f in sorted(glob.glob(os.path.expanduser('~/.codex/sessions/2026/09/1[1-5]/*.jsonl'))):
    cwd=None; model=None; src=None; name=None
    for line in open(f):
        if cwd is None and '"session_meta"' in line:
            o=json.loads(line); p=o['payload']; cwd=p.get('cwd'); src=p.get('thread_source') or json.dumps(p.get('source'))[:30]
            try: name=p['source']['subagent']['thread_spawn'].get('agent_nickname') or p['source']['subagent']['thread_spawn'].get('agent_role')
            except Exception: name=None
            if cwd!=CWD: break
            continue
        if '"turn_context"' in line:
            o=json.loads(line); model=o['payload'].get('model'); continue
        if '"token_count"' not in line: continue
        o=json.loads(line); info=(o['payload'].get('info') or {})
        lu=info.get('last_token_usage') or {}
        if not lu.get('input_tokens'): continue
        key=(o['timestamp'],lu.get('input_tokens'),lu.get('output_tokens'))
        if key in seen: continue
        seen.add(key)
        for k in ['input_tokens','cached_input_tokens','cache_write_input_tokens','output_tokens','reasoning_output_tokens']:
            agg[model][k]+=lu.get(k,0) or 0
            byday[o['timestamp'][:10]][k]+=lu.get(k,0) or 0
        nresp[model]+=1
        byday[o['timestamp'][:10]]['n']+=1
for m,c in agg.items(): print(m, nresp[m], dict(c))
tot=collections.Counter()
for c in agg.values(): tot.update(c)
print('TOTAL',sum(nresp.values()),dict(tot))
for d,c in sorted(byday.items()): print(d,dict(c))
