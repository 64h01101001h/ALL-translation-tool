import json, glob, collections
P='/Users/adamderickandrade/.claude/projects/-Users-adamderickandrade-ALL-translation-tool'
S=P+'/463f1ae3-dff6-4666-a672-f84264aa4eb2'
ad=S+'/subagents/workflows/wf_025e4913-5f3'
lastc=collections.Counter(); maxc=collections.Counter(); w1=0; w5=0; nkeys=0; nonmono=0
for f in glob.glob(ad+'/agent-*.jsonl'):
    seen={}; mx={}
    for line in open(f):
        try: j=json.loads(line)
        except: continue
        m=j.get('message') or {}
        if j.get('type')!='assistant' or 'usage' not in m: continue
        k=(m.get('id'),j.get('requestId'))
        u=m['usage']
        if k in seen and u.get('output_tokens',0)<seen[k].get('output_tokens',0): nonmono+=1
        seen[k]=u
        mx.setdefault(k,collections.Counter())
        for kk in ('input_tokens','cache_read_input_tokens','cache_creation_input_tokens','output_tokens'):
            mx[k][kk]=max(mx[k][kk],u.get(kk,0))
    for k,u in seen.items():
        nkeys+=1
        for kk in ('input_tokens','cache_read_input_tokens','cache_creation_input_tokens','output_tokens'): lastc[kk]+=u.get(kk,0)
        cc=u.get('cache_creation') or {}
        w1+=cc.get('ephemeral_1h_input_tokens',0) or 0; w5+=cc.get('ephemeral_5m_input_tokens',0) or 0
    for k,c in mx.items(): maxc.update(c)
print('keys',nkeys,'last',dict(lastc)); print('max',dict(maxc)); print('w5',w5,'w1',w1,'nonmonotonic',nonmono)
d=json.load(open(S+'/workflows/wf_025e4913-5f3.json'))
print('record totalTokens',d['totalTokens'])
print('in+out+write',lastc['input_tokens']+lastc['output_tokens']+lastc['cache_creation_input_tokens'])
