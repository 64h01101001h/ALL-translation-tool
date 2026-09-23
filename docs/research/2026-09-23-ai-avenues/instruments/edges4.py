exec(open('edges.py').read())
c=collections.Counter(); ex=collections.defaultdict(list)
for f,course,seq,w,e,ts,es in SEGS:
    T={}; E={}
    for x in ts: T.setdefault(x[1],[]).append(x)
    for x in es: E.setdefault(x[1],[]).append(x)
    flags=set()
    for lab,v in T.items():
        if len(v)>1: flags.add('dup_label_tib')
    for lab,v in E.items():
        if len(v)>1: flags.add('dup_label_eng')
    for lab in E:
        if lab not in T: flags.add('eng_only_label')
    for lab,v in T.items():
        d=v[0][0]
        if lab.endswith('S') and d==1: continue
        if '-' in lab: flags.add('hyphen_label')
        if d<5: flags.add('clause_d%d'%d)
        if lab not in E: flags.add('tib_only_no_nul_marker')
        else:
            ev=E[lab][0]
            if ev[3]==ev[4] and ev[5] is None: flags.add('empty_eng_no_nul')
            if ev[0]!=d: flags.add('d_mismatch')
            if ev[2]!=v[0][2]: flags.add('cls_mismatch')
    if not flags: flags.add('clean')
    for fl in flags:
        c[fl]+=1
        if len(ex[fl])<3: ex[fl].append((os.path.basename(f),seq))
for k,v in c.most_common(): print(k,v,ex[k])
