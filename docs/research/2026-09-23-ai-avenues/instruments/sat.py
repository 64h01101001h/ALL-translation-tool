exec(open('cov.py').read().split("def incidence_stats")[0])
seen=set(); out=[]
order=sorted(segs,key=lambda k:(k[0],k[1]))
B=18
for i in range(0,len(order),B):
    blk=order[i:i+B]
    types=set().union(*(seg_h[k] for k in blk))
    new=types-seen; seen|=types
    out.append((blk[0][0],blk[0][1],len(new)/max(len(types),1)))
import statistics
for c in ['C01','C02','C03','C04','C05']:
    v=[x[2] for x in out if x[0]==c]
    print(c,'batches',len(v),'median new-type share %.2f'%statistics.median(v),'last 5:',[round(x,2) for x in v[-5:]])
