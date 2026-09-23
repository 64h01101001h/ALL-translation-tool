import json,glob,os,re,collections,statistics,sqlite3,math
D=os.path.expanduser('~/.claude/projects/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2/subagents/workflows/')
db=sqlite3.connect('/Users/adamderickandrade/ALL-translation-tool/build/hgm_spine_v27_2.db')
rowmap={(c,s):((w or ''),(e or '')) for c,s,w,e in db.execute("select course,seq,wylie,english from corpus_segments")}
A=[]
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
        A.append(dict(wf=wf,inp=inp,ccre=ccre,cread=cread,out=out,billable=inp+ccre+out,
                      total=inp+ccre+cread+out,tools=tools,prompt=prompt))
ALIGN=[a for a in A if re.search(r'PROPOSE_BRIEF|RECONCILE_BRIEF|full-depth alignment|alignment campaign', a['prompt'] or '')]
print('total agents %d ; alignment agents %d ; alignment workflows %d'%(len(A),len(ALIGN),len(set(a['wf'] for a in ALIGN))))
def q(v,p): 
    v=sorted(v); return v[int(len(v)*p)] if v else 0
for nm,S in (('ALL agents',A),('ALIGNMENT agents',ALIGN)):
    b=[a['billable'] for a in S]; t=[a['total'] for a in S]; to=[a['tools'] for a in S]
    print(f"\n[{nm}] n={len(S)}")
    print("  billable(in+cachecreate+out): median %d  mean %d  p90 %d  SUM %d"%(statistics.median(b),statistics.mean(b),q(b,.9),sum(b)))
    print("  total incl cache_read:        median %d  mean %d  p90 %d  SUM %d"%(statistics.median(t),statistics.mean(t),q(t,.9),sum(t)))
    print("  tool calls: median %.0f mean %.1f p90 %d max %d"%(statistics.median(to),statistics.mean(to),q(to,.9),max(to)))
# find per-segment identity: look for 'seq=NNN' concrete or 'SEGMENT NNN'
pat=[re.compile(r"seq\s*=\s*(\d{1,4})\b"), re.compile(r"[Ss]egment\s+(\d{1,4})\b"), re.compile(r"\bC(\d\d):(\d{1,4})\b")]
def segs(p):
    s=set()
    for m in pat[0].finditer(p):
        if m.group(1)!='<SEQ>': s.add(int(m.group(1)))
    for m in pat[2].finditer(p): s.add(int(m.group(2)))
    return s
cnt=collections.Counter()
pts=[]
for a in ALIGN:
    ss=segs(a['prompt'])
    cnt[len(ss)]+=1
    m=re.search(r"course='(C\d\d)'",a['prompt']) or re.search(r'\bCourse\s+(\d+)',a['prompt'])
    co=None
    mm=re.search(r"course='(C\d\d)'",a['prompt'])
    if mm: co=mm.group(1)
    else:
        m2=re.search(r'Course\s+(\d+)',a['prompt'])
        if m2: co='C%02d'%int(m2.group(1))
    if len(ss)==1 and co and (co,list(ss)[0]) in rowmap:
        w,e=rowmap[(co,list(ss)[0])]
        pts.append((len(w.split())+len(e.split()), a['billable'], a['total'], a['tools']))
print('\nsegment-count-in-prompt distribution:',dict(sorted(cnt.items())[:8]))
print('usable (length,cost) points:',len(pts))
if len(pts)>5:
    def pear(x,y):
        n=len(x);mx=sum(x)/n;my=sum(y)/n
        sx=math.sqrt(sum((i-mx)**2 for i in x));sy=math.sqrt(sum((i-my)**2 for i in y))
        return sum((i-mx)*(j-my) for i,j in zip(x,y))/(sx*sy) if sx and sy else 0
    X=[p[0] for p in pts];B=[p[1] for p in pts];T=[p[2] for p in pts];TO=[p[3] for p in pts]
    print('  r(length, billable)=%.3f  r(length,total)=%.3f  r(length,toolcalls)=%.3f'%(pear(X,B),pear(X,T),pear(X,TO)))
    n=len(X);mx=sum(X)/n;my=sum(B)/n
    den=sum((x-mx)**2 for x in X)
    if den:
        bb=sum((x-mx)*(y-my) for x,y in zip(X,B))/den; aa=my-bb*mx
        med=statistics.median(X)
        print('  fit billable = %.0f + %.1f*length ; at median length %.0f the FIXED share is %.0f%%'%(aa,bb,med,100*aa/max(1e-9,(aa+bb*med))))
    print('  length median %.0f p10 %.0f p90 %.0f ; billable median %d'%(statistics.median(X),q(X,.1),q(X,.9),statistics.median(B)))
