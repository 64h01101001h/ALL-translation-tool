import json, collections, statistics, datetime
A=json.load(open('agents.json'))
ag=[a for a in A if a['role']]
segs=collections.defaultdict(list)
for a in ag: segs[a['seq']].append(a)
full={s:v for s,v in segs.items() if len(v)>=3}
print('segments counted:',len(full),'agents:',sum(len(v) for v in full.values()))
P={'input':5/1e6,'cw':6.25/1e6,'cr':0.5/1e6,'out':25/1e6}
def cost(d): return sum(d[k]*P[k] for k in P)
per=[]
for s,v in full.items():
    d={k:sum(a['ded'][k] for a in v) for k in P}
    r={k:sum(a['raw'][k] for a in v) for k in P}
    per.append((s,d,r,sum(a['tools'] for a in v),sum(a['nids'] for a in v),sum(a['nrows'] for a in v)))
n=len(per)
agg={k:sum(p[1][k] for p in per)/n for k in P}
aggr={k:sum(p[2][k] for p in per)/n for k in P}
print('\n=== DEDUPED per segment (mean) ===')
for k in ('input','cw','out','cr'): print(f'  {k:6s} {agg[k]:12,.0f}')
bill=agg['input']+agg['cw']+agg['out']
print(f'  billable(in+cw+out) {bill:,.0f}   totalmoved {bill+agg["cr"]:,.0f}')
print(f'  COST ${cost(agg):.4f}/segment')
print('\n=== RAW (undeduped rows) per segment (mean) ===')
for k in ('input','cw','out','cr'): print(f'  {k:6s} {aggr[k]:12,.0f}')
billr=aggr['input']+aggr['cw']+aggr['out']
print(f'  billable {billr:,.0f}  ratio raw/ded = {billr/bill:.3f}')
print(f'  COST ${cost(aggr):.4f}/segment (if you wrongly summed rows)')
meds={k:statistics.median([p[1][k] for p in per]) for k in P}
print('\nmedian deduped billable per segment:', f"{meds['input']+meds['cw']+meds['out']:,.0f}")
print('median deduped cost:', f"${statistics.median([cost(p[1]) for p in per]):.4f}")
costs=sorted(cost(p[1]) for p in per)
print('cost p10/p50/p90:', f'${costs[int(.1*n)]:.2f} / ${costs[n//2]:.2f} / ${costs[int(.9*n)]:.2f}')
print('\ntools per segment mean', sum(p[3] for p in per)/n, ' per agent', sum(p[3] for p in per)/sum(len(v) for v in full.values()))
print('api requests (msg ids) per segment mean', sum(p[4] for p in per)/n)
print('usage rows per segment mean', sum(p[5] for p in per)/n, 'ratio rows/ids', sum(p[5] for p in per)/sum(p[4] for p in per))
# per role
for role in ('propose','reconcile'):
    v=[a for a in ag if a['role']==role]
    d={k:sum(a['ded'][k] for a in v)/len(v) for k in P}
    print(f'\n{role}: n={len(v)} out={d["out"]:,.0f} cw={d["cw"]:,.0f} cr={d["cr"]:,.0f} cost=${cost(d):.3f} tools={sum(a["tools"] for a in v)/len(v):.1f} reqs={sum(a["nids"] for a in v)/len(v):.1f}')
