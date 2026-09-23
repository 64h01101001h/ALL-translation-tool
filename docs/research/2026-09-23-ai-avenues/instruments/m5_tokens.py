import json,glob,os,re,collections,statistics,sqlite3
D=os.path.expanduser('~/.claude/projects/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2/subagents/workflows/')
db=sqlite3.connect('/Users/adamderickandrade/ALL-translation-tool/build/hgm_spine_v27_2.db')
rowmap={(c,s):(w or '',e or '') for c,s,w,e in db.execute("select course,seq,wylie,english from corpus_segments")}
agents=[]
for wf in sorted(os.listdir(D)):
    for f in sorted(glob.glob(os.path.join(D,wf,'*.jsonl'))):
        seen=set(); billable=0; cread=0; out=0; inp=0; ccre=0; tools=0; prompt=''
        for line in open(f, errors='replace'):
            try: r=json.loads(line)
            except: continue
            if r.get('type')=='user' and not prompt:
                c=r.get('message',{}).get('content')
                if isinstance(c,str): prompt=c
            if r.get('type')=='assistant':
                m=r.get('message',{}); mid=m.get('id')
                u=m.get('usage')
                if not u: continue
                key=(mid, u.get('output_tokens'))
                if mid and mid in seen: continue
                if mid: seen.add(mid)
                inp+=u.get('input_tokens',0); ccre+=u.get('cache_creation_input_tokens',0)
                cread+=u.get('cache_read_input_tokens',0); out+=u.get('output_tokens',0)
                for b in m.get('content',[]) or []:
                    if isinstance(b,dict) and b.get('type')=='tool_use': tools+=1
        billable=inp+ccre+out
        agents.append(dict(wf=wf,f=os.path.basename(f),inp=inp,ccre=ccre,cread=cread,out=out,
                           billable=billable, total=inp+ccre+cread+out, tools=tools, prompt=prompt))
print('agents:',len(agents))
b=[a['billable'] for a in agents if a['billable']>0]
t=[a['total'] for a in agents if a['billable']>0]
print('per-agent billable (in+cache_create+out): median %d mean %d'%(statistics.median(b),statistics.mean(b)))
print('per-agent TOTAL incl cache_read:          median %d mean %d'%(statistics.median(t),statistics.mean(t)))
print('per-agent tool calls: median %.0f mean %.1f max %d'%(statistics.median([a['tools'] for a in agents]),statistics.mean([a['tools'] for a in agents]),max(a['tools'] for a in agents)))
# segments mentioned
SEG=re.compile(r"SEGMENTS?\s+TO\s+\w+[^\n]*?((?:\d+\s*,?\s*)+)", re.I)
def segs_of(p):
    m=re.findall(r"seq\s*(?:=|IN|in)\s*\(?\s*([\d,\s]+)\)?", p)
    out=set()
    for g in m:
        for x in re.findall(r'\d+',g): out.add(int(x))
    return out
byseg=collections.defaultdict(list)
matched=0
for a in agents:
    ss=segs_of(a['prompt'])
    ss={x for x in ss if x<3000}
    if len(ss)==1:
        matched+=1; a['seg']=list(ss)[0]
print('agents whose prompt names exactly one segment:',matched)
# derive course
course_of={}
for a in agents:
    m=re.search(r"course='(C\d\d)'", a['prompt']) or re.search(r'\b(C0[1-9]|C1[0-8])\b', a['prompt'])
    a['course']=m.group(1) if m else None
pairs=[]
for a in agents:
    if a.get('seg') and a['course'] and (a['course'],a['seg']) in rowmap and a['billable']>0:
        w,e=rowmap[(a['course'],a['seg'])]
        L=len(w.split())+len(e.split())
        pairs.append((L,a['billable'],a['tools'],a['total']))
print('usable (segment-length, cost) points:',len(pairs))
if pairs:
    import math
    def pear(x,y):
        n=len(x);mx=sum(x)/n;my=sum(y)/n
        sx=math.sqrt(sum((i-mx)**2 for i in x));sy=math.sqrt(sum((i-my)**2 for i in y))
        return sum((i-mx)*(j-my) for i,j in zip(x,y))/(sx*sy) if sx and sy else 0
    X=[p[0] for p in pairs];Y=[p[1] for p in pairs];T=[p[3] for p in pairs]
    print('corr(segment length, billable tokens) r=%.3f'%pear(X,Y))
    print('corr(segment length, total tokens)    r=%.3f'%pear(X,T))
    print('corr(segment length, tool calls)      r=%.3f'%pear(X,[p[2] for p in pairs]))
    # linear fit  cost = a + b*L
    n=len(X);mx=sum(X)/n;my=sum(Y)/n
    bb=sum((x-mx)*(y-my) for x,y in zip(X,Y))/sum((x-mx)**2 for x in X)
    aa=my-bb*mx
    print('fit billable = %.0f + %.0f * length   (median length %.0f -> fixed share %.0f%%)'%(aa,bb,statistics.median(X),100*aa/(aa+bb*statistics.median(X))))
    print('length: median %.0f p10 %.0f p90 %.0f'%(statistics.median(X),sorted(X)[len(X)//10],sorted(X)[9*len(X)//10]))
json.dump([{k:v for k,v in a.items() if k!='prompt'} for a in agents],open('/private/tmp/claude-501/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2/scratchpad/agents.json','w'))
