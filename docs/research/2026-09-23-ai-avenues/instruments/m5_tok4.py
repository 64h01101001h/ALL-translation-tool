import json,glob,os,re,collections,statistics,sqlite3,math
D=os.path.expanduser('~/.claude/projects/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2/subagents/workflows/')
db=sqlite3.connect('/Users/adamderickandrade/ALL-translation-tool/build/hgm_spine_v27_2.db')
rowmap={(c,s):((w or ''),(e or '')) for c,s,w,e in db.execute("select course,seq,wylie,english from corpus_segments")}
TASK=re.compile(r'YOUR TASK:\s*(\w+)[^\n]*?\b(C\d\d)\s+segment\s+(\d+)')
recs=[]
for wf in sorted(os.listdir(D)):
    for f in sorted(glob.glob(os.path.join(D,wf,'*.jsonl'))):
        seen=set(); inp=ccre=cread=out=tools=0; prompt=''
        for line in open(f,errors='replace'):
            try: r=json.loads(line)
            except: continue
            if r.get('type')=='user' and not prompt:
                c=r.get('message',{}).get('content')
                if isinstance(c,str): prompt=c
            if r.get('type')=='assistant':
                m=r.get('message',{}); mid=m.get('id'); u=m.get('usage')
                if not u: continue
                if mid and mid in seen: continue
                if mid: seen.add(mid)
                inp+=u.get('input_tokens',0); ccre+=u.get('cache_creation_input_tokens',0)
                cread+=u.get('cache_read_input_tokens',0); out+=u.get('output_tokens',0)
                for b in m.get('content',[]) or []:
                    if isinstance(b,dict) and b.get('type')=='tool_use': tools+=1
        m=TASK.search(prompt or '')
        if m:
            recs.append(dict(wf=wf,role=m.group(1).lower(),course=m.group(2),seg=int(m.group(3)),
                bill=inp+ccre+out,tot=inp+ccre+cread+out,cread=cread,out=out,ccre=ccre,inp=inp,tools=tools))
print('agents with an explicit single-segment task:',len(recs))
print('roles:',collections.Counter(r['role'] for r in recs))
print('courses:',collections.Counter(r['course'] for r in recs))
seg=collections.defaultdict(lambda: dict(bill=0,tot=0,n=0,tools=0))
for r in recs:
    k=(r['course'],r['seg']); seg[k]['bill']+=r['bill']; seg[k]['tot']+=r['tot']; seg[k]['n']+=1; seg[k]['tools']+=r['tools']
print('distinct segments covered:',len(seg))
full=[v for v in seg.values() if v['n']>=3]
print('segments with >=3 agents (complete propose+propose+reconcile):',len(full))
def q(v,p):
    v=sorted(v); return v[int(len(v)*p)]
for nm,S in (('ALL segments',list(seg.values())),('complete (n>=3)',full)):
    b=[v['bill'] for v in S]; t=[v['tot'] for v in S]; to=[v['tools'] for v in S]
    print(f"\n[{nm}] n={len(S)}  agents/segment mean {statistics.mean([v['n'] for v in S]):.2f}")
    print("  BILLABLE (in+cache_create+out) per segment: median %d mean %d p90 %d"%(statistics.median(b),statistics.mean(b),q(b,.9)))
    print("  TOTAL incl cache_read per segment:          median %d mean %d p90 %d"%(statistics.median(t),statistics.mean(t),q(t,.9)))
    print("  tool calls per segment: median %.0f mean %.1f max %d"%(statistics.median(to),statistics.mean(to),max(to)))
print('\nper-AGENT tool calls: median %.0f mean %.1f p90 %d max %d'%(statistics.median([r['tools'] for r in recs]),statistics.mean([r['tools'] for r in recs]),q([r['tools'] for r in recs],.9),max(r['tools'] for r in recs)))
# cost vs length
pts=[]
for k,v in seg.items():
    if k in rowmap and v['n']>=3:
        w,e=rowmap[k]; pts.append((len(w.split())+len(e.split()), v['bill'], v['tot']))
def pear(x,y):
    n=len(x);mx=sum(x)/n;my=sum(y)/n
    sx=math.sqrt(sum((i-mx)**2 for i in x));sy=math.sqrt(sum((i-my)**2 for i in y))
    return sum((i-mx)*(j-my) for i,j in zip(x,y))/(sx*sy) if sx and sy else 0
X=[p[0] for p in pts];B=[p[1] for p in pts];T=[p[2] for p in pts]
print('\n(length, cost) points: %d  r(len,billable)=%.3f  r(len,total)=%.3f'%(len(pts),pear(X,B),pear(X,T)))
mx=sum(X)/len(X);my=sum(B)/len(B)
bb=sum((x-mx)*(y-my) for x,y in zip(X,B))/sum((x-mx)**2 for x in X); aa=my-bb*mx
med=statistics.median(X)
print('  fit billable = %.0f + %.1f*length ; at median length %.0f fixed share = %.0f%%'%(aa,bb,med,100*aa/(aa+bb*med)))
myt=sum(T)/len(T); bt=sum((x-mx)*(y-myt) for x,y in zip(X,T))/sum((x-mx)**2 for x in X); at=myt-bt*mx
print('  fit total    = %.0f + %.1f*length ; at median length %.0f fixed share = %.0f%%'%(at,bt,med,100*at/(at+bt*med)))
print('  segment length median %.0f p10 %.0f p90 %.0f'%(med,q(X,.1),q(X,.9)))
