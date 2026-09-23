import json, collections, statistics, os
S=json.load(open('cost_summary.json'))
ag=[a for s in S for a in s['agents'] if a['role']]
segs=collections.defaultdict(list)
for a in ag: segs[a['seq']].append(a)
print('distinct segments', len(segs))
comp=[s for s,v in segs.items() if len(v)>=3]
print('segments with >=3 agents', len(comp))
print('agents per seg dist', collections.Counter(len(v) for v in segs.values()))
# workflows containing c05 agents
wfc=collections.Counter()
for s in S:
    n=len([a for a in s['agents'] if a['role']])
    if n: wfc[s['wf']]=n
print('c05 workflows', len(wfc))
print(sorted(wfc.values()))
