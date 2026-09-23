exec(open('edges.py').read())
c=collections.Counter(); ex=collections.defaultdict(list)
for f,course,seq,w,e,ts,es in SEGS:
    T={}; E={}
    for x in ts: T.setdefault(x[1],[]).append(x)
    for x in es: E.setdefault(x[1],[]).append(x)
    clause = any(v[0][0]<5 and not (k.endswith('S') and v[0][0]==1) for k,v in T.items())
    flags=set()
    for lab in E:
        if lab not in T: flags.add('eng_only_label')
    for lab,v in T.items():
        if lab.endswith('S') and v[0][0]==1: continue
        if lab not in E: flags.add('tib_only_no_marker')
        else:
            ev=E[lab][0]
            if ev[3]==ev[4] and ev[5] is not None and ev[0]!=v[0][0]: flags.add('nul_d_mismatch')
            if ev[3]!=ev[4] and ev[0]!=v[0][0]: flags.add('d_mismatch_real')
            if ev[3]!=ev[4] and ev[2]!=v[0][2]: flags.add('cls_mismatch_real')
            if ev[3]==ev[4] and ev[2]!='u': flags.add('nul_with_cls')
    d=os.path.basename(os.path.dirname(f))
    for fl in flags:
        c[(clause,fl)]+=1
        if len(ex[(clause,fl)])<4: ex[(clause,fl)].append((d,os.path.basename(f),seq))
    c[(clause,'segs')]+=1
for k,v in sorted(c.items()): print(k,v,ex.get(k))
