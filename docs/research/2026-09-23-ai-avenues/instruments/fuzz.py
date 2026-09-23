from harness import *
import random, collections
random.seed(20260923)
SEGS=[("C05",169),("C05",103),("C05",34),("C03",427),("C03",207),("C05",67),("C05",120),("C05",71),("C03",160),("C05",430)]
REP_E={"the","of","and","in","to","a","is","that","which","not","be"}
REP_T={"la","ni","gyi","kyi","pa","ba","dang","de",",","'i","ma","mi","gis","kyis","yin","na"}
out=collections.Counter(); mism=[]; crashes=[]
def pick_ranges(toks, text, k, rep):
    idx=list(range(len(toks)))
    pref=[i for i in idx if text[toks[i][0]:toks[i][1]] in rep]
    chosen=set(); rngs=[]
    for _ in range(k):
        start=random.choice(pref) if pref and random.random()<0.6 else random.choice(idx)
        ln=1 if random.random()<0.6 else random.randint(1,4)
        end=min(len(toks)-1,start+ln-1)
        if any(i in chosen for i in range(start,end+1)): continue
        chosen.update(range(start,end+1)); rngs.append((start,end))
    return rngs
def bracket(text,toks,s,e):
    a,b=toks[s][0],toks[e][1]
    if random.random()<0.85 or b-a<2: return (a,b)
    # trim a char at one end
    if random.random()<0.5: return (a+1,b)
    return (a,b-1)
N=0
for it in range(900):
    C,Q=random.choice(SEGS); w,e=row(C,Q)
    tt,et=mytok(w,'tib'),mytok(e,'eng')
    tr=pick_ranges(tt,w,random.randint(1,6),REP_T); er=pick_ranges(et,e,len(tr),REP_E)
    spans=[]; 
    for n,(s,t) in enumerate(tr):
        d=random.choice([5,5,5,6])
        eng=bracket(e,et,*er[n]) if n<len(er) and random.random()<0.8 else None
        spans.append(dict(id="w%d"%n,d=d,tib=bracket(w,tt,s,t),eng=eng))
        # members
        if d==5 and t>s and random.random()<0.5:
            ms=random.randint(s,t); me=ms
            mt=(tt[ms][0],tt[me][1])
            if mt!=(tt[s][0],tt[t][1]):
                meng=None
                if eng and random.random()<0.6:
                    inner=[i for i,(x,y) in enumerate(et) if eng[0]<=x and y<=eng[1]]
                    if len(inner)>1:
                        j=random.choice(inner); meng=(et[j][0],et[j][1])
                spans.append(dict(id="m%d"%n,d=7,tib=mt,eng=meng))
    import io, contextlib
    buf=io.StringIO()
    try:
        with contextlib.redirect_stdout(buf):
            r=attempt("fuzz%d"%it,C,Q,spans)
    except Exception as x:
        crashes.append((it,repr(x))); continue
    N+=1
    st=r[0] if r[0]!="REFUSED" else "REFUSED "+r[1]
    out[st]+=1
    if r[0] in ("MISMATCH","CLI_REFUSED"): mism.append((it,C,Q,r[1]))
print("proposals",N,"harness crashes",crashes[:3])
for k,v in out.most_common(): print("  %-40s %d"%(k,v))
print("MISMATCHES / CLI refusals after compile:",mism[:5])
