exec(open('neardup.py').read().split("for thr in")[0])
import difflib
n=0; kinds=collections.Counter()
for k,(jt,je,o) in best.items():
    if not o or jt<1 or je<1: continue
    a,b=seg[k],seg[o]
    dw = a[0]!=b[0]; de = a[1]!=b[1]
    kinds[("W" if dw else "-")+("E" if de else "-")]+=1
    if n<4:
        n+=1
        for x,y in ((a[0],b[0]),(a[1],b[1])):
            if x!=y:
                sm=difflib.SequenceMatcher(None,x,y)
                print(k,o,[ (t,x[i1:i2],y[j1:j2]) for t,i1,i2,j1,j2 in sm.get_opcodes() if t!='equal'][:4])
print(kinds)
