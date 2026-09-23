import json,glob,os,collections,sys
rows=[]
rl=[]  # rate limit snapshots
for d in ['10','11','12','13','14','15','16','17']:
    for f in sorted(glob.glob(os.path.expanduser(f'~/.codex/sessions/2026/09/{d}/*.jsonl'))):
        meta=None; models=collections.Counter(); efforts=collections.Counter(); tiers=collections.Counter()
        first=last=None; tot=None; nturn=0; ntc=0; reached=collections.Counter(); plan=set()
        with open(f) as fh:
            for line in fh:
                if '"session_meta"' not in line and '"turn_context"' not in line and '"token_count"' not in line and '"timestamp"' not in line[:40]:
                    continue
                try: o=json.loads(line)
                except: continue
                ts=o.get('timestamp')
                if ts:
                    first=first or ts; last=ts
                t=o.get('type'); p=o.get('payload') or {}
                if t=='session_meta':
                    meta=p
                elif t=='turn_context':
                    nturn+=1
                    models[p.get('model')]+=1
                    efforts[str(p.get('effort') or p.get('reasoning_effort') or (p.get('collaboration_mode') or {}).get('settings',{}).get('reasoning_effort'))]+=1
                    tiers[str(p.get('service_tier'))]+=1
                elif t=='event_msg' and p.get('type')=='token_count':
                    ntc+=1
                    info=p.get('info') or {}
                    if info.get('total_token_usage'): tot=info['total_token_usage']
                    r=p.get('rate_limits') or {}
                    if r:
                        plan.add(r.get('plan_type'))
                        if r.get('rate_limit_reached_type'): reached[r.get('rate_limit_reached_type')]+=1
                        pr=r.get('primary') or {}
                        rl.append((ts, os.path.basename(f)[-45:-6], pr.get('used_percent'), pr.get('window_minutes'), pr.get('resets_at'), (r.get('secondary') or {}).get('used_percent'), r.get('rate_limit_reached_type'), r.get('limit_id')))
        src=(meta or {}).get('source')
        rows.append(dict(file=os.path.basename(f), size=os.path.getsize(f), id=(meta or {}).get('id'), parent=(meta or {}).get('parent_thread_id'),
            src=json.dumps(src)[:60], thread_source=(meta or {}).get('thread_source'), cwd=(meta or {}).get('cwd'), cli=(meta or {}).get('cli_version'), first=first,last=last,
            models=dict(models), efforts=dict(efforts), tiers=dict(tiers), nturn=nturn, tot=tot, plan=list(plan), reached=dict(reached)))
json.dump(rows,open('codex_rows.json','w'),indent=1)
json.dump(rl,open('codex_rl.json','w'))
print(len(rows), len(rl))
