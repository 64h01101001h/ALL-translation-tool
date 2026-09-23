import json, collections, random, math
B='/Users/adamderickandrade/ALL-translation-tool/data/alignment/alignment_full_v1.json'
d=json.load(open(B)); L=d['links']
sub=[l for l in L if l['d']==5 and l.get('eng') and l.get('tib') and l.get('seg') is not None]
byseg=collections.defaultdict(list)
for l in sub: byseg[(l['course'],l['seg'])].append(l)
segs=list(byseg)
print('segments with d=5 pairs', len(segs), 'links', len(sub))
random.seed(20260922)
def curve(order):
    pairs=set(); hw=set(); out=[]
    for i,s in enumerate(order,1):
        for l in byseg[s]:
            pairs.add((l['tib'],l['eng'])); hw.add(l['tib'])
        if i%200==0 or i==len(order): out.append((i,len(pairs),len(hw)))
    return out
runs=[curve(random.sample(segs,len(segs))) for _ in range(5)]
print('\nsegments | mean distinct pairs | mean headwords | new pairs/segment (marginal)')
prev=(0,0,0)
for idx in range(len(runs[0])):
    n=runs[0][idx][0]
    mp=sum(r[idx][1] for r in runs)/len(runs); mh=sum(r[idx][2] for r in runs)/len(runs)
    print('%7d %18.0f %15.0f %12.2f'%(n,mp,mh,(mp-prev[1])/(n-prev[0])))
    prev=(n,mp,mh)
# fit Heaps: P = k * n^b on the averaged curve
import statistics
xs=[]; ys=[]
for idx in range(len(runs[0])):
    n=runs[0][idx][0]; mp=sum(r[idx][1] for r in runs)/len(runs)
    xs.append(math.log(n)); ys.append(math.log(mp))
mx=statistics.mean(xs); my=statistics.mean(ys)
b=sum((x-mx)*(y-my) for x,y in zip(xs,ys))/sum((x-mx)**2 for x in xs)
k=math.exp(my-b*mx)
print('\nHeaps fit on distinct pairs: P = %.3f * n^%.3f  (r2 check below)'%(k,b))
ss_res=sum((y-(math.log(k)+b*x))**2 for x,y in zip(xs,ys)); ss_tot=sum((y-my)**2 for y in ys)
print('log-log r2 = %.4f'%(1-ss_res/ss_tot))
for target in (1748,4073+1748,7920):
    print('projected distinct d=5 pairs at %5d unique segments: %7.0f'%(target,k*target**b))
