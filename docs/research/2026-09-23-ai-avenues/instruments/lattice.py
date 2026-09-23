import json,sqlite3,collections,re,time,sys
R='/Users/adamderickandrade/ALL-translation-tool/'
d=json.load(open(R+'data/alignment/alignment_full_v1.json'))
L=[l for l in d['links'] if l.get('seg') is not None]
db=sqlite3.connect(R+'build/hgm_spine_v27_2.db'); c=db.cursor()
seg={}
c.execute("select course,seq,wylie,english from corpus_segments")
for co,sq,w,e in c.fetchall(): seg[(co,sq)]=(w,e or '')
# lexicon
dictheads=set(); gloss=collections.defaultdict(set)
c.execute("select wylie,hgm_gloss from entries")
for w,g in c.fetchall():
    if not w: continue
    dictheads.add(w)
    if g:
        try:
            for s in json.loads(g): gloss[w].add(s)
        except Exception: pass
print('dict headwords',len(dictheads),'glossed',len(gloss))
bankheads=set(l['tib'] for l in L if l['d']==5)
print('bank d5 headwords',len(bankheads))
lex=dictheads|bankheads
print('lexicon',len(lex))

def syls(w):
    # split wylie into syllables; keep offsets
    out=[];i=0
    for m in re.finditer(r"[^\s]+",w):
        out.append((m.start(),m.end(),m.group()))
    return out

# build lattice: at each syllable boundary, longest-to-shortest lexicon matches
maxsyl=8
def lattice_spans(w):
    S=syls(w); res=set()
    for i in range(len(S)):
        for j in range(i+1,min(i+maxsyl,len(S))+1):
            txt=w[S[i][0]:S[j-1][1]]
            t=txt.strip().strip(',').strip()
            if t in lex: res.add(t)
            if txt in lex: res.add(txt)
    return res

# leave-one-course-out English attestation
bycourse=collections.defaultdict(list)
for l in L:
    if l['d']==5 and l.get('eng'): bycourse[l['course']].append(l)
attest=collections.defaultdict(collections.Counter)
for l in L:
    if l['d']==5 and l.get('eng'): attest[(l['course'],l['tib'])][l['eng']]+=1
t0=time.time()
for course in ['C01','C02','C03','C04','C05']:
    other=collections.defaultdict(collections.Counter)
    for l in L:
        if l['d']==5 and l.get('eng') and l['course']!=course: other[l['tib']][l['eng']]+=1
    tot=0; tibhit=0; enghit=0; engsrc=collections.Counter()
    for l in bycourse[course]:
        w,e=seg[(course,l['seg'])]
        sp=lattice_spans(w)
        tot+=1
        if l['tib'] in sp or l['tib'].strip() in sp: tibhit+=1
        cands=set(other.get(l['tib'],{}))|set(gloss.get(l['tib'],set()))
        cands={x for x in cands if x and x in e}
        if l['eng'] in cands: enghit+=1
    print('%s n=%d tib-in-lattice %.1f%%  eng-already-attested %.1f%%'%(course,tot,100*tibhit/tot,100*enghit/tot))
print('elapsed %.1fs'%(time.time()-t0))
