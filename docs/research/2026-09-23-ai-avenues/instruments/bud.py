import json,random,statistics
P='/private/tmp/claude-501/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2/scratchpad/h.json'
D=json.load(open(P))
new={int(k):set(v) for k,v in D['new'].items()}
feat={int(k):v for k,v in D['feat'].items()}
segs=sorted(new)
TOT=sum(feat[s]['cost'] for s in segs)
ALL=len(set().union(*new.values()))
def cap(order,frac,cost=True):
    budget=TOT*frac if cost else len(segs)*frac
    spent=0; got=set()
    for s in order:
        c=feat[s]['cost'] if cost else 1
        if spent+c>budget: break
        spent+=c; got|=new[s]
    return 100*len(got)/ALL
def rank(key): return sorted(segs,key=key,reverse=True)
def greedy(cost=True):
    rem=dict(new); got=set(); out=[]
    while rem:
        best=max(rem,key=lambda s:(len(rem[s]-got))/(feat[s]['cost'] if cost else 1))
        out.append(best); got|=rem.pop(best)
    return out
def rnd(frac,cost=True,n=200):
    v=[]
    for i in range(n):
        o=segs[:]; random.Random(i).shuffle(o); v.append(cap(o,frac,cost))
    return statistics.mean(v), statistics.pstdev(v)
for frac in (0.20,0.30):
    print('=== token budget %d%%'%(frac*100))
    m,sd=rnd(frac); print('  random            %.1f (sd %.1f)'%(m,sd))
    print('  rank by length    %.1f'%cap(rank(lambda s:feat[s]['length']),frac))
    print('  novel/token       %.1f'%cap(rank(lambda s:feat[s]['novel']/feat[s]['cost']),frac))
    print('  distinct/token    %.1f'%cap(rank(lambda s:feat[s]['dist']/feat[s]['cost']),frac))
    print('  novel-share       %.1f'%cap(rank(lambda s:feat[s]['share']),frac))
    print('  greedy oracle     %.1f'%cap(greedy(),frac))
print('=== segment budget 20%')
m,sd=rnd(0.20,False); print('  random            %.1f (sd %.1f)'%(m,sd))
print('  rank by length    %.1f'%cap(rank(lambda s:feat[s]['length']),0.20,False))
print('  distinct/token    %.1f'%cap(rank(lambda s:feat[s]['dist']/feat[s]['cost']),0.20,False))
print('  greedy oracle     %.1f'%cap(greedy(False),0.20,False))
# completeness
def need(order):
    got=set()
    for i,s in enumerate(order,1):
        got|=new[s]
        if len(got)==ALL: return i
    return len(order)
print('=== segments needed for 100%')
print('  oracle', need(greedy(False)), ' length-rank', need(rank(lambda s:feat[s]['length'])), ' of', len(segs))
