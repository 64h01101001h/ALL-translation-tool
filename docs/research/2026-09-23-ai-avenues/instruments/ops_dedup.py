import json,glob,os,datetime,collections
R=os.path.expanduser('~/.claude/projects/-Users-adamderickandrade-ALL-translation-tool/')
a=datetime.datetime(2026,9,17,19,22,tzinfo=datetime.timezone.utc);b=datetime.datetime(2026,9,18,10,22,tzinfo=datetime.timezone.utc)
first={};mx={};rows=collections.Counter();where={}
files=glob.glob(R+'*.jsonl')+glob.glob(R+'*/subagents/**/*.jsonl',recursive=True)
for f in files:
  for line in open(f,errors='ignore'):
    if '"usage"' not in line: continue
    try:o=json.loads(line)
    except: continue
    m=o.get('message') or {};u=m.get('usage');mid=m.get('id');ts=o.get('timestamp')
    if not u or not mid or not ts: continue
    t=datetime.datetime.fromisoformat(ts.replace('Z','+00:00'))
    if not (a<=t<=b): continue
    v=(u.get('input_tokens') or 0,u.get('cache_creation_input_tokens') or 0,u.get('cache_read_input_tokens') or 0,u.get('output_tokens') or 0)
    rows[mid]+=1
    where.setdefault(mid,'main' if '/subagents/' not in f else 'sub')
    if mid not in first: first[mid]=v
    mx[mid]=tuple(max(p,q) for p,q in zip(mx.get(mid,(0,0,0,0)),v))
p=(5,6.25,0.5,25)
for lab in ('main','sub'):
  F=[first[k] for k in first if where[k]==lab];M=[mx[k] for k in mx if where[k]==lab]
  sf=[sum(x[i] for x in F) for i in range(4)];sm=[sum(x[i] for x in M) for i in range(4)]
  print(lab,len(F),'first-row out %.2fM cost $%.0f | max-row out %.2fM cost $%.0f'%(sf[3]/1e6,sum(a*b for a,b in zip(p,sf))/1e6,sm[3]/1e6,sum(a*b for a,b in zip(p,sm))/1e6))
print('rows per msg',sum(rows.values())/len(rows), 'msgs with differing output', sum(1 for k in first if first[k][3]!=mx[k][3]))
