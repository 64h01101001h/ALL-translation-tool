import json,glob,os,datetime,collections
a=datetime.datetime(2026,9,11,10,22,tzinfo=datetime.timezone.utc);b=datetime.datetime(2026,9,18,10,22,tzinfo=datetime.timezone.utc)
price=[('claude-opus-5-5',(4,5,0.2,20)),('claude-opus-5',(5,6.25,0.5,25)),('claude-fable-5-1',(10,12.5,0.25,50)),('claude-fable-5',(10,12.5,1,50)),('claude-sonnet-5',(2,2.5,0.2,10)),('claude-haiku-4-5',(1,1.25,0.1,5))]
first={};mx={};mod={}
for f in glob.glob(os.path.expanduser('~/.claude/projects/*/**/*.jsonl'),recursive=True):
  try:
   for line in open(f,errors='ignore'):
    if '"usage"' not in line: continue
    try:o=json.loads(line)
    except: continue
    m=o.get('message') or {};u=m.get('usage');mid=m.get('id');ts=o.get('timestamp')
    if not u or not mid or not ts: continue
    t=datetime.datetime.fromisoformat(ts.replace('Z','+00:00'))
    if not (a<=t<=b): continue
    v=(u.get('input_tokens') or 0,u.get('cache_creation_input_tokens') or 0,u.get('cache_read_input_tokens') or 0,u.get('output_tokens') or 0)
    if mid not in first: first[mid]=v; mod[mid]=m.get('model','')
    mx[mid]=tuple(max(p,q) for p,q in zip(mx.get(mid,(0,0,0,0)),v))
  except Exception: pass
def cost(D):
  s=0
  for k,v in D.items():
    p=next((pp for n,pp in price if mod[k].startswith(n)),None)
    if p: s+=sum(x*y for x,y in zip(p,v))/1e6
  return s
print('msgs',len(first),'first-row $%.0f  max-row $%.0f'%(cost(first),cost(mx)))
print(collections.Counter(mod.values()).most_common(5))
