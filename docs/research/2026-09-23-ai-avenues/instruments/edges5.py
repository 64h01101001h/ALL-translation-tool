exec(open('edges.py').read())
n=0
for f,course,seq,w,e,ts,es in SEGS:
    if not (os.path.basename(f)=='c1p101.html' and seq==338): continue
    T={x[1]:x for x in ts}; E={}
    for x in es: E.setdefault(x[1],[]).append(x)
    for lab,t in T.items():
        for ev in E.get(lab,[]):
            if ev[0]!=t[0] or ev[2]!=t[2]:
                print(lab,'TIB',t[0],t[2],repr(w[t[3]:t[4]]),'| ENG',ev[0],ev[2],repr(e[ev[3]:ev[4]]),ev[5])
