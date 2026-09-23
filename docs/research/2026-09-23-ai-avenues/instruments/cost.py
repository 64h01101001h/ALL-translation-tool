import json,glob,os,statistics
base=os.path.expanduser('~/.claude/projects/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2')
wfdir=base+'/workflows'
tdir=base+'/subagents/workflows'
runs=[]
for f in sorted(glob.glob(wfdir+'/wf_*.json')):
    d=json.load(open(f))
    if d.get('workflowName')!='c05-align': continue
    runs.append(d)
comp=[d for d in runs if d['status']=='completed']
print('c05-align runs',len(runs),'completed',len(comp))

def scan(runid):
    dd=os.path.join(tdir,runid)
    if not os.path.isdir(dd): return None
    dedup={}   # message.id -> usage
    naive={'input':0,'cr':0,'cw':0,'out':0}
    nlines=0
    toolcalls=0
    agents=set()
    for jf in glob.glob(dd+'/agent-*.jsonl'):
        agents.add(os.path.basename(jf))
        for line in open(jf):
            try: r=json.loads(line)
            except: continue
            m=r.get('message') or {}
            if r.get('type')=='assistant':
                u=m.get('usage') or {}
                if not u: continue
                nlines+=1
                i=u.get('input_tokens',0); cr=u.get('cache_read_input_tokens',0)
                cw=u.get('cache_creation_input_tokens',0); o=u.get('output_tokens',0)
                naive['input']+=i;naive['cr']+=cr;naive['cw']+=cw;naive['out']+=o
                mid=m.get('id')
                if mid is None: mid='anon%d'%nlines
                prev=dedup.get(mid)
                if prev is None: dedup[mid]=[i,cr,cw,o]
                else:
                    prev[0]=max(prev[0],i);prev[1]=max(prev[1],cr);prev[2]=max(prev[2],cw);prev[3]=max(prev[3],o)
                for c in (m.get('content') or []):
                    if isinstance(c,dict) and c.get('type')=='tool_use': toolcalls+=1
    D=[0,0,0,0]
    for v in dedup.values():
        for k in range(4): D[k]+=v[k]
    return dict(dedup=D,naive=naive,nlines=nlines,nmsg=len(dedup),tools=toolcalls,agents=len(agents))

tot=[0,0,0,0]; totn={'input':0,'cr':0,'cw':0,'out':0}; segs=0; nl=0; nm=0; tc=0; ag=0
per=[]
for d in comp:
    s=scan(d['runId'])
    if s is None:
        print('MISSING transcripts',d['runId']); continue
    n=len(d['args']['seqs'])
    segs+=n; nl+=s['nlines']; nm+=s['nmsg']; tc+=s['tools']; ag+=s['agents']
    for k in range(4): tot[k]+=s['dedup'][k]
    for k in totn: totn[k]+=s['naive'][k]
    per.append((d['runId'],n,s['dedup'],s['naive'],d['totalTokens']))
print('segments',segs,'agents',ag,'assistant-lines',nl,'unique msg ids',nm,'toolcalls',tc)
i,cr,cw,o=tot
print('DEDUP per segment: input %.0f cache_read %.0f cache_write %.0f output %.0f'%(i/segs,cr/segs,cw/segs,o/segs))
print('  billable(in+cw+out) %.0f  all-moved %.0f'%((i+cw+o)/segs,(i+cr+cw+o)/segs))
cost=(i*5+cr*0.5+cw*6.25+o*25)/1e6
print('  cost/segment $%.4f  (cache_read share %.1f%%)'%(cost/segs, (cr*0.5)/(i*5+cr*0.5+cw*6.25+o*25)*100))
ni,ncr,ncw,no=totn['input'],totn['cr'],totn['cw'],totn['out']
print('NAIVE per segment: billable(in+cw+out) %.0f  all %.0f'%((ni+ncw+no)/segs,(ni+ncr+ncw+no)/segs))
print('duplication factor %.3f'%((ni+ncw+no)/(i+cw+o)))
# per-run naive billable medians
vals=[(nv['input']+nv['cw']+nv['out'])/n for _,n,_,nv,_ in per]
dvals=[(dv[0]+dv[2]+dv[3])/n for _,n,dv,_,_ in per]
print('naive billable/seg median %.0f mean %.0f'%(statistics.median(vals),statistics.mean(vals)))
print('dedup billable/seg median %.0f mean %.0f'%(statistics.median(dvals),statistics.mean(dvals)))
ht=[t for _,_,_,_,t in per]
print('harness totalTokens/seg mean %.0f'%(sum(ht)/segs))
