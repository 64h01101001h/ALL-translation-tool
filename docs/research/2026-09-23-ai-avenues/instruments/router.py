import json,glob,os,sqlite3,re,statistics
from collections import defaultdict
BASE=os.path.expanduser('~/.claude/projects/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2')
R='/Users/adamderickandrade/ALL-translation-tool'
ids=set()
for f in glob.glob(BASE+'/workflows/wf_*.json'):
    d=json.load(open(f))
    if d.get('workflowName')=='c05-align' and d.get('status')=='completed': ids.add(d['runId'])
specs=defaultdict(dict)
for runId in ids:
    for jf in glob.glob(os.path.join(BASE,'subagents','workflows',runId)+'/agent-*.jsonl'):
        last=None
        for line in open(jf):
            try: o=json.loads(line)
            except: continue
            if o.get('type')!='assistant': continue
            for b in ((o.get('message') or {}).get('content') or []):
                if b.get('type')=='tool_use' and b.get('name')=='StructuredOutput': last=b.get('input') or {}
        if not last: continue
        s,a=last.get('seq'),last.get('angle')
        if s is None or not a: continue
        specs[s][a]={(x.get('tib','').strip(),(x.get('eng') or '').strip(),x.get('d')) for x in (last.get('spans') or []) if x.get('tib')}
con=sqlite3.connect(R+'/build/hgm_spine_v27_2.db')
seg={r[0]:(r[1] or '',r[2] or '') for r in con.execute("select seq,wylie,english from corpus_segments where course='C05'")}
L=json.load(open(R+'/data/alignment/alignment_full_v1.json'))['links']
idx=defaultdict(set)
for l in L:
    if l['course']=='C05' or l.get('d')!=5: continue
    if l.get('tib') and l.get('eng'): idx[l['tib'].strip()].add(l['eng'].strip())
for w,g in con.execute("select wylie,hgm_gloss from entries where hgm_gloss is not null and hgm_gloss!=''"):
    try: gl=json.loads(g)
    except: continue
    for e in gl:
        if isinstance(e,str) and e.strip(): idx[w.strip()].add(e.strip())
def bh(h,n):
    i=h.find(n)
    while i>=0:
        pre=h[i-1] if i else ' '; post=h[i+len(n)] if i+len(n)<len(h) else ' '
        if pre in ' ,.\n\t/;(' and post in ' ,.\n\t/;)': return True
        i=h.find(n,i+1)
    return False
def pear(x,y):
    n=len(x); mx=sum(x)/n; my=sum(y)/n
    sx=sum((a-mx)**2 for a in x)**.5; sy=sum((b-my)**2 for b in y)**.5
    return sum((a-mx)*(b-my) for a,b in zip(x,y))/(sx*sy) if sx and sy else 0
X=[];Y=[]
for s,v in specs.items():
    ks=[k for k in v if k!='reconciled']
    if len(ks)<2 or s not in seg: continue
    A,B=v[ks[0]],v[ks[1]]
    disagree=1-(len(A&B)/len(A|B) if (A|B) else 1)
    w,en=seg[s]
    syls=[y for y in re.split(r'[ ,./\n]+',w) if y]
    covered=set()
    for t,es in idx.items():
        if len(t)<2 or not bh(w,t): continue
        if not any(e in en for e in es): continue
        for y in re.split(r'[ ,./]+',t):
            if y: covered.add(y)
    unc=1-(sum(1 for y in syls if y in covered)/len(syls)) if syls else 0
    X.append(unc); Y.append(disagree)
print('n =',len(X))
print('uncovered-syllable share vs ANGLE-A/ANGLE-B disagreement:  r = %.3f'%pear(X,Y))
q=sorted(zip(X,Y)); k=len(q)//5
print('  bottom quintile mean disagreement %.3f | top quintile %.3f'%(statistics.mean([y for _,y in q[:k]]),statistics.mean([y for _,y in q[-k:]])))
print('  mean uncovered %.3f  mean disagreement %.3f'%(statistics.mean(X),statistics.mean(Y)))
