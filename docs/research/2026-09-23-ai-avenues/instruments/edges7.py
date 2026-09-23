exec(open('edges.py').read())
for f,course,seq,w,e,ts,es in SEGS:
    T={x[1]:x for x in ts}; E={x[1]:x for x in es}
    clause = any(v[0]<5 and not (k.endswith('S') and v[0]==1) for k,v in T.items())
    if clause: continue
    for lab,t in T.items():
        if lab.endswith('S') and t[0]==1: continue
        if lab not in E:
            # find parent
            par=[x for x in ts if x[0]==5 and x[3]<=t[3] and t[4]<=x[4] and x[1]!=lab]
            pe=[E.get(p[1]) for p in par]
            print(os.path.basename(f),seq,lab,t[0],repr(w[t[3]:t[4]]),'parent',[(p[1],repr(w[p[3]:p[4]])) for p in par], 'parent eng', [(x[3],x[4],x[5]) if x else None for x in pe])
