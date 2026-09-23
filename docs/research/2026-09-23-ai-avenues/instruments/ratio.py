exec(open('proxy.py').read().split("print('forward:")[0])
import statistics, random, math
for tgt_i in (3,4):
    ref=set(C[:tgt_i]); tgt=C[tgt_i]
    H=set(tuple(t.split()) for k in segs if k[0] in ref for t in seg_h[k])
    Hs=set(' '.join(h) for h in H); maxlen=max(len(h) for h in H)
    xs=[];ys=[];ns=[]
    for k in segs:
        if k[0]!=tgt: continue
        u,r=cover(sylls(rows[k][0]),H,maxlen)
        xs.append(len(u)); ys.append(sum(1 for t in seg_h[k] if t not in Hs)); ns.append(len(seg_h[k]))
    n=len(xs); mx=statistics.mean(xs); my=statistics.mean(ys)
    r=statistics.correlation(xs,ys)
    print(tgt,'segments',n,'mean uncovered syl',round(mx,2),'mean novel HW incidences',round(my,2),'pearson r=%.2f'%r,'novel share of HW incidences=%.3f'%(sum(ys)/sum(ns)))
    # simulate SRS n=60: ratio estimator vs plain mean for population total of y
    random.seed(3); idx=list(range(n)); Y=sum(ys); X=sum(xs)
    for m in (30,60,120):
        e1=[];e2=[]
        for rep in range(4000):
            s=random.sample(idx,m)
            e1.append(statistics.mean(ys[i] for i in s)*n)
            e2.append(sum(ys[i] for i in s)/max(sum(xs[i] for i in s),1)*X)
        rel=lambda e: statistics.pstdev(e)/Y
        print('   n=%3d  expansion estimator rel.SE=%.3f   ratio estimator (x=uncovered syllables) rel.SE=%.3f  ratio bias=%.3f'%(m,rel(e1),rel(e2),statistics.mean(e2)/Y-1))
