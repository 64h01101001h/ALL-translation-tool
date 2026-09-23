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
order=['C01','C02','C03','C04','C05']
print('FORWARD-ONLY (bank = strictly earlier courses only) vs LEAVE-ONE-OUT (bank = all other courses)')
for i,course in enumerate(order):
    for mode in ('fwd','loo'):
        if mode=='fwd':
            if i==0: continue
            src=[l for l in d5 if l['course'] in order[:i]]
        else:
            src=[l for l in d5 if l['course']!=course]
        att=collections.defaultdict(collections.Counter)
        for l in src: att[l['tib']][l['eng']]+=1
        tot=0; hit=0
        for l in d5:
            if l['course']!=course: continue
            w,e=seg[(course,l['seg'])]
            tot+=1
            cands={x for x in (set(att.get(l['tib'],{}))|gloss.get(l['tib'],set())) if x and x in e}
            if l['eng'] in cands: hit+=1
        print('  %s %s n=%d eng-already-attested %.1f%%  (bank size %d links)'%(course,mode,tot,100*hit/tot,len(src)))
# gloss-only baseline (no bank at all) -- what a brand-new course gets
att={}
for course in order:
    tot=0;hit=0
    for l in d5:
        if l['course']!=course: continue
        w,e=seg[(course,l['seg'])]; tot+=1
        cands={x for x in gloss.get(l['tib'],set()) if x and x in e}
        if l['eng'] in cands: hit+=1
    print('  %s DICTIONARY-ONLY (no bank) %.1f%%'%(course,100*hit/tot))
