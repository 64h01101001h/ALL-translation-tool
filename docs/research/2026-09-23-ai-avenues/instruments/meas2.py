import json,os,glob,statistics,collections
B=os.path.expanduser('~/.claude/projects/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2')
WF=os.path.join(B,'workflows'); SUB=os.path.join(B,'subagents','workflows')

def scan(wfid):
    d=os.path.join(SUB,wfid)
    if not os.path.isdir(d): return None
    byid={}; rows=0; rowsum=collections.Counter(); agents=0; models=collections.Counter()
    tiers=collections.Counter(); ttl=collections.Counter()
    for f in glob.glob(os.path.join(d,'agent-*.jsonl')):
        agents+=1
        for line in open(f,errors='replace'):
            line=line.strip()
            if not line: continue
            try: o=json.loads(line)
            except: continue
            if o.get('type')!='assistant': continue
            m=o.get('message') or {}
            u=m.get('usage')
            if not u: continue
            rows+=1
            for k in ('input_tokens','cache_creation_input_tokens','cache_read_input_tokens','output_tokens'):
                rowsum[k]+=u.get(k) or 0
            mid=m.get('id')
            models[m.get('model')]+=1
            tiers[u.get('service_tier')]+=1
            cc=u.get('cache_creation') or {}
            for k,v in cc.items(): ttl[k]+=v or 0
            e=byid.get(mid)
            if e is None:
                byid[mid]=dict(i=u.get('input_tokens') or 0,
                               cw=u.get('cache_creation_input_tokens') or 0,
                               cr=u.get('cache_read_input_tokens') or 0,
                               o=u.get('output_tokens') or 0)
            else:
                e['o']=max(e['o'],u.get('output_tokens') or 0)
    ded=collections.Counter()
    for v in byid.values():
        ded['input']+=v['i']; ded['cw']+=v['cw']; ded['cr']+=v['cr']; ded['out']+=v['o']
    return dict(agents=agents,rows=rows,ids=len(byid),rowsum=dict(rowsum),ded=dict(ded),
                models=dict(models),tiers=dict(tiers),ttl=dict(ttl))

res={}
allttl=collections.Counter(); allmodels=collections.Counter(); alltiers=collections.Counter()
for f in sorted(glob.glob(os.path.join(WF,'wf_*.json'))):
    d=json.load(open(f)); wfid=os.path.basename(f)[:-5]
    nm=d.get('workflowName')
    if not nm.startswith('c05-align'): continue
    st=d.get('status')
    args=(d.get('args') or {})
    seqs=args.get('seqs') or []
    s=scan(wfid)
    res[wfid]=dict(name=nm,status=st,nseq=len(seqs),dur=d.get('durationMs'),tok=d.get('tokens'),scan=s)
    if s:
        allttl.update(s['ttl']); allmodels.update(s['models']); alltiers.update(s['tiers'])

comp=[v for v in res.values() if v['name']=='c05-align' and v['status']=='completed' and v['scan']]
print('completed c05-align with transcripts:',len(comp))
tot=collections.Counter(); totrow=collections.Counter(); nseq=0; nag=0; nrow=0; nid=0
per=[]
for v in comp:
    s=v['scan']; n=v['nseq'] or 0
    nseq+=n; nag+=s['agents']; nrow+=s['rows']; nid+=s['ids']
    for k in s['ded']: tot[k]+=s['ded'][k]
    for k in s['rowsum']: totrow[k]+=s['rowsum'][k]
    if n:
        bil=(s['ded']['input']+s['ded']['cw']+s['ded']['out'])/n
        bilrow=(s['rowsum']['input_tokens']+s['rowsum']['cache_creation_input_tokens']+s['rowsum']['output_tokens'])/n
        per.append((v,bil,bilrow,s['ded']['cr']/n))
print('segments:',nseq,'agents:',nag,'assistant usage rows:',nrow,'distinct msg ids:',nid,'dup factor:',round(nrow/nid,4))
print('DEDUP totals:',dict(tot))
print('ROWSUM totals:',dict(totrow))
print()
print('PER SEGMENT (dedup): input %.0f cache_write %.0f cache_read %.0f output %.0f'%(tot['input']/nseq,tot['cw']/nseq,tot['cr']/nseq,tot['out']/nseq))
bill=(tot['input']+tot['cw']+tot['out'])/nseq
print('billable(in+cw+out)/seg %.0f  total moved/seg %.0f'%(bill,(tot['input']+tot['cw']+tot['cr']+tot['out'])/nseq))
print('ROW-SUM billable/seg %.0f (median over runs %.0f)'%((totrow['input_tokens']+totrow['cache_creation_input_tokens']+totrow['output_tokens'])/nseq, statistics.median([p[2] for p in per])))
print('dedup billable per-run median %.0f mean %.0f min %.0f max %.0f'%(statistics.median([p[1] for p in per]),statistics.mean([p[1] for p in per]),min(p[1] for p in per),max(p[1] for p in per)))
cost=(tot['input']*5 + tot['cw']*6.25 + tot['cr']*0.5 + tot['out']*25)/1e6
print('COST total $%.2f  per segment $%.4f'%(cost,cost/nseq))
print('components/seg: in $%.5f cw $%.5f cr $%.5f out $%.5f'%(tot['input']/nseq*5e-6,tot['cw']/nseq*6.25e-6,tot['cr']/nseq*0.5e-6,tot['out']/nseq*25e-6))
print()
print('models:',dict(allmodels)); print('tiers:',dict(alltiers)); print('cache ttl buckets:',dict(allttl))
print()
print('--- runs EXCLUDED by the doc filter ---')
for k,v in res.items():
    if not(v['name']=='c05-align' and v['status']=='completed'):
        s=v['scan']
        print(k,v['name'],v['status'],'nseq=',v['nseq'],'harness tokens=',v['tok'],'agents=',s['agents'] if s else None,'dedup billable=',(s['ded']['input']+s['ded']['cw']+s['ded']['out']) if s else None)
