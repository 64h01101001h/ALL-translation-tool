import json,sqlite3,collections,re
R='/Users/adamderickandrade/ALL-translation-tool/'
d=json.load(open(R+'data/alignment/alignment_full_v1.json'))
L=[l for l in d['links'] if l.get('seg') is not None]
db=sqlite3.connect(R+'build/hgm_spine_v27_2.db'); c=db.cursor()
seg={}
c.execute("select course,seq,wylie,english from corpus_segments")
for co,sq,w,e in c.fetchall(): seg[(co,sq)]=(w,e or '')
gloss=collections.defaultdict(set)
c.execute("select wylie,hgm_gloss from entries")
for w,g in c.fetchall():
    if g:
        try:
            for s in json.loads(g): gloss[w].add(s)
        except Exception: pass
d5=[l for l in L if l['d']==5 and l.get('eng')]
print('course  n  offered%  true-in-list%  top1-correct%  top1-correct|offered%  mean-cands')
for course in ['C01','C02','C03','C04','C05']:
    att=collections.defaultdict(collections.Counter)
    for l in d5:
        if l['course']!=course: att[l['tib']][l['eng']]+=1
    n=0;offered=0;inlist=0;top1=0;top1o=0;nc=0
    for l in d5:
        if l['course']!=course: continue
        w,e=seg[(course,l['seg'])]
        cc=collections.Counter()
        for k,v in att.get(l['tib'],{}).items(): cc[k]+=v
        for k in gloss.get(l['tib'],set()): cc[k]+=0
        cands=[(k,v) for k,v in cc.items() if k and k in e]
        n+=1; nc+=len(cands)
        if cands:
            offered+=1
            cands.sort(key=lambda x:(-x[1],-len(x[0])))
            if l['eng'] in [k for k,_ in cands]: inlist+=1
            if cands[0][0]==l['eng']: top1+=1; top1o+=1
    print('  %s %5d  %5.1f  %5.1f  %5.1f  %5.1f  %.1f'%(course,n,100*offered/n,100*inlist/n,100*top1/n,100*top1o/max(offered,1),nc/n))
