import json,glob,os,collections
seen=set()
wins={'W0921':('2026-09-14T11:28','2026-09-15T02:53'),'W0919':('2026-09-12T08:10','2026-09-13T06:41'),'W0920':('2026-09-13T14:48','2026-09-14T08:51'),'W0922part':('2026-09-15T03:15','2026-09-15T11:03')}
agg={w:collections.Counter() for w in wins}
for f in sorted(glob.glob(os.path.expanduser('~/.codex/sessions/2026/09/1[1-7]/*.jsonl'))):
    model=None
    for line in open(f):
        if '"turn_context"' in line:
            model=json.loads(line)['payload'].get('model'); continue
        if '"token_count"' not in line: continue
        o=json.loads(line); info=(o['payload'].get('info') or {}); lu=info.get('last_token_usage') or {}
        if not lu.get('input_tokens'): continue
        key=(o['timestamp'],lu.get('input_tokens'),lu.get('output_tokens'))
        if key in seen: continue
        seen.add(key)
        ts=o['timestamp'][:16]
        for w,(a,b) in wins.items():
            if a<=ts<=b:
                c=agg[w]; c['n']+=1; c['in']+=lu['input_tokens']; c['cached']+=lu.get('cached_input_tokens',0); c['out']+=lu.get('output_tokens',0); c['reason']+=lu.get('reasoning_output_tokens',0); c['m:'+str(model)]+=1
for w,c in agg.items():
    unc=c['in']-c['cached']
    print(w, dict(c), 'uncached',unc)
