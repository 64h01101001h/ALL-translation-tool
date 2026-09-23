import json,os,glob,statistics,collections,re,sys
D=os.path.expanduser('~/.claude/projects/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2/subagents/workflows')
rows=[]
for wf in sorted(os.listdir(D)):
    wfp=os.path.join(D,wf)
    if not os.path.isdir(wfp): continue
    for f in sorted(glob.glob(wfp+'/agent-*.jsonl')):
        per={}   # requestId -> dict
        naive=collections.Counter()
        nblocks=0
        toolcalls=0
        firstprompt=''
        ts=[]
        for line in open(f,errors='replace'):
            try: d=json.loads(line)
            except: continue
            t=d.get('type')
            if d.get('timestamp'): ts.append(d['timestamp'])
            if t=='user' and not firstprompt:
                m=d.get('message') or {}
                c=m.get('content')
                if isinstance(c,str): firstprompt=c[:400]
                elif isinstance(c,list):
                    for b in c:
                        if isinstance(b,dict) and b.get('type')=='text': firstprompt=b['text'][:400];break
            if t!='assistant': continue
            m=d.get('message') or {}
            u=m.get('usage') or {}
            nblocks+=1
            rid=d.get('requestId') or ('noreq%d'%nblocks)
            cur=per.setdefault(rid,{'input_tokens':0,'cache_creation_input_tokens':0,'cache_read_input_tokens':0,'output_tokens':0})
            for k in cur: cur[k]=max(cur[k],u.get(k) or 0)
            for k in cur: naive[k]+= (u.get(k) or 0)
            for b in (m.get('content') or []):
                if isinstance(b,dict) and b.get('type')=='tool_use': toolcalls+=1
        if not per: continue
        agg=collections.Counter()
        for rid,v in per.items():
            for k,x in v.items(): agg[k]+=x
        rows.append(dict(wf=wf,agent=os.path.basename(f),requests=len(per),blocks=nblocks,tools=toolcalls,
            inp=agg['input_tokens'],cw=agg['cache_creation_input_tokens'],cr=agg['cache_read_input_tokens'],out=agg['output_tokens'],
            n_inp=naive['input_tokens'],n_cw=naive['cache_creation_input_tokens'],n_cr=naive['cache_read_input_tokens'],n_out=naive['output_tokens'],
            prompt=firstprompt, t0=min(ts) if ts else None, t1=max(ts) if ts else None))
json.dump(rows,open('/private/tmp/claude-501/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2/scratchpad/rows.json','w'))
print('agents:',len(rows))
print('blocks/request overall:', sum(r['blocks'] for r in rows)/sum(r['requests'] for r in rows))
tot=collections.Counter()
for r in rows:
    for k in ('inp','cw','cr','out','n_inp','n_cw','n_cr','n_out'): tot[k]+=r[k]
print('DEDUPED totals:', {k:tot[k] for k in ('inp','cw','cr','out')})
print('NAIVE   totals:', {k:tot[k] for k in ('n_inp','n_cw','n_cr','n_out')})
print('naive/dedup cw:', tot['n_cw']/tot['cw'], 'cr:',tot['n_cr']/tot['cr'], 'out:',tot['n_out']/tot['out'])
