import json,glob,os,datetime,collections
P=os.path.expanduser('~/.claude/projects/-Users-adamderickandrade-ALL-translation-tool/')
price={'claude-opus-5-5':(4,5,0.2,20),'claude-opus-5':(5,6.25,0.5,25),'claude-fable-5-1':(10,12.5,0.25,50)}
a=datetime.datetime(2026,9,17,19,22,tzinfo=datetime.timezone.utc);b=datetime.datetime(2026,9,18,10,22,tzinfo=datetime.timezone.utc)
seen=set();byfile=collections.Counter();bymodel=collections.Counter();cw5=collections.Counter()
for f in glob.glob(P+'*.jsonl')+glob.glob(P+'*/subagents/**/*.jsonl',recursive=True):
  for line in open(f,errors='ignore'):
    if '"usage"' not in line: continue
    try:o=json.loads(line)
    except: continue
    m=o.get('message') or {};u=m.get('usage');mid=m.get('id');ts=o.get('timestamp')
    if not u or not mid or not ts or mid in seen: continue
    t=datetime.datetime.fromisoformat(ts.replace('Z','+00:00'))
    if not (a<=t<=b): continue
    seen.add(mid)
    key=f.replace(P,'')
    key=key.split('/subagents/')[0]+('/sub:'+key.split('/')[-2] if '/subagents/' in key else '/main')
    p=next((v for k,v in price.items() if m.get('model','').startswith(k)),(0,0,0,0))
    c=((u.get('input_tokens') or 0)*p[0]+(u.get('cache_creation_input_tokens') or 0)*p[1]+(u.get('cache_read_input_tokens') or 0)*p[2]+(u.get('output_tokens') or 0)*p[3])/1e6
    byfile[key]+=c;bymodel[m.get('model')]+=1
    cc=u.get('cache_creation') or {}
    cw5['5m']+=cc.get('ephemeral_5m_input_tokens') or 0; cw5['1h']+=cc.get('ephemeral_1h_input_tokens') or 0
for k,v in byfile.most_common(12): print('%-80s $%.0f'%(k,v))
print(bymodel.most_common()); print(cw5)
