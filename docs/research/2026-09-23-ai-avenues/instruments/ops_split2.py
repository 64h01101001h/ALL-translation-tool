import json,glob,os,datetime,collections
P=os.path.expanduser('~/.claude/projects/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2/subagents/workflows/')
a=datetime.datetime(2026,9,17,19,22,tzinfo=datetime.timezone.utc);b=datetime.datetime(2026,9,18,10,22,tzinfo=datetime.timezone.utc)
tok=collections.defaultdict(lambda:[0,0,0,0]);nag=collections.Counter()
for f in glob.glob(P+'wf_*/agent-*.jsonl'):
  txt=open(f,errors='ignore').read(300000)
  kind='rec' if 'YOUR TASK: reconcile' in txt else ('prop' if 'YOUR TASK: propose' in txt else 'other')
  seen={};hit=False
  for line in open(f,errors='ignore'):
    if '"usage"' not in line: continue
    try:o=json.loads(line)
    except: continue
    m=o.get('message') or {};u=m.get('usage');mid=m.get('id');ts=o.get('timestamp')
    if not u or not mid or not ts: continue
    t=datetime.datetime.fromisoformat(ts.replace('Z','+00:00'))
    if not (a<=t<=b): continue
    hit=True;vv=(u.get("input_tokens") or 0,u.get("cache_creation_input_tokens") or 0,u.get("cache_read_input_tokens") or 0,u.get("output_tokens") or 0);seen[mid]=tuple(max(p,q) for p,q in zip(seen.get(mid,(0,0,0,0)),vv));continue
    x=tok[kind];x[0]+=u.get('input_tokens') or 0;x[1]+=u.get('cache_creation_input_tokens') or 0;x[2]+=u.get('cache_read_input_tokens') or 0;x[3]+=u.get('output_tokens') or 0
  if hit:
    nag[kind]+=1
    x=tok[kind]
    for v in seen.values():
      for i in range(4): x[i]+=v[i]
price={'opus5':(5,6.25,0.5,25),'opus55':(4,5,0.2,20),'fable51':(10,12.5,0.25,50)}
for k,v in tok.items():
  per=[z/195/1e6 for z in v]
  print(k,nag[k],'per-seg M tokens in/cw/cr/out: %.3f %.3f %.3f %.3f'%tuple(per), ' '.join('%s $%.2f'%(n,sum(pp*q for pp,q in zip(p,per))) for n,p in price.items()))
