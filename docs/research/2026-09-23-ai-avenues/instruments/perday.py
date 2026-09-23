import json, glob, collections, statistics as st
exec(open('wfcost2.py').read().split("mode=sys.argv[1]")[0])
wf=collections.defaultdict(collections.Counter); t0={}; nag=collections.Counter()
for f in glob.glob(P+'/*/subagents/workflows/*/*.jsonl'):
    w=f.split('/')[-2]; u,fu,ts=agent_usage(f); wf[w].update(u); nag[w]+=1
    if ts and (w not in t0 or ts<t0[w]): t0[w]=ts
byday=collections.defaultdict(list)
for w,u in wf.items():
    byday[t0.get(w,"?")[:10]].append((nag[w],round(cost(u),1)))
for d in sorted(byday): print(d, sorted(byday[d])[-12:])
