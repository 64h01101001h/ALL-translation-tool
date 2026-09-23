import json,glob,os,datetime
P=os.path.expanduser('~/.claude/projects/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2/subagents/workflows/')
a=datetime.datetime(2026,9,17,19,22,tzinfo=datetime.timezone.utc);b=datetime.datetime(2026,9,18,10,22,tzinfo=datetime.timezone.utc)
gapw=0;gapn=0;totw=0;first=0;n=0
for f in glob.glob(P+'wf_*/agent-*.jsonl'):
  seen=set();prev=None;k=0
  for line in open(f,errors='ignore'):
    if '"usage"' not in line: continue
    try:o=json.loads(line)
    except: continue
    m=o.get('message') or {};u=m.get('usage');mid=m.get('id');ts=o.get('timestamp')
    if not u or not mid or mid in seen or not ts: continue
    seen.add(mid);t=datetime.datetime.fromisoformat(ts.replace('Z','+00:00'))
    if not (a<=t<=b): continue
    w=u.get('cache_creation_input_tokens') or 0;totw+=w;k+=1
    if k==1: first+=w
    elif prev and (t-prev).total_seconds()>300: gapw+=w;gapn+=1
    prev=t
print('total sub cache writes %.1fM; first-request writes %.1fM; writes after >5min gaps %.2fM in %d requests'%(totw/1e6,first/1e6,gapw/1e6,gapn))
