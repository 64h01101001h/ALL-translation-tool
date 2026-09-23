import json,re,sqlite3,collections
R='/Users/adamderickandrade/ALL-translation-tool/'
full=json.load(open(R+'data/alignment/alignment_full_v1.json'))
L=full['links']
con=sqlite3.connect(R+'build/hgm_spine_v27_2.db');cur=con.cursor()
cur.execute("select course,seq,english from corpus_segments")
eng={(c,s):(e or '') for c,s,e in cur.fetchall()}
out={}
# --- particles
d6=[x for x in L if x['d']==6]
out['particles']={'d6_total':len(d6),'d6_glossed':sum(1 for x in d6 if x['eng']),
  'd6_null':sum(1 for x in d6 if not x['eng']),
  'distinct_particle_forms':len({x['tib'] for x in d6}),
  'top_forms':collections.Counter(x['tib'] for x in d6).most_common(15),
  'case_flag_true':sum(1 for x in d6 if x['case'])}
# --- omissions (null eng) by depth
out['omissions']={'null_links':sum(1 for x in L if x['eng'] is None),
  'by_depth':dict(collections.Counter(x['d'] for x in L if x['eng'] is None)),
  'distinct_tib_null_only':len({x['tib'] for x in L if x['eng'] is None} - {x['tib'] for x in L if x['eng']})}
# --- crossing / non-monotone English order (free hard-case detector)
bys=collections.defaultdict(list)
for x in L:
    if x['d'] in (5,6) and x['eng']: bys[(x['course'],x['seg'])].append(x)
cross=0; tot=0; inv_tot=0; crossing_segs=[]
for k,v in bys.items():
    E=eng.get(k,'')
    if not E: continue
    pos=[]; cur_i=0; ok=True
    for x in v:
        i=E.find(x['eng'], 0)
        if i<0: ok=False; break
        pos.append(i)
    if not ok or len(pos)<2: continue
    tot+=1
    inv=sum(1 for a in range(len(pos)) for b in range(a+1,len(pos)) if pos[a]>pos[b])
    if inv: cross+=1; inv_tot+=inv; crossing_segs.append((k,inv,len(pos)))
out['crossing']={'segments_scored':tot,'segments_with_inversion':cross,
  'share':round(cross/tot,4) if tot else None,'total_inversions':inv_tot,
  'worst':sorted(crossing_segs,key=lambda t:-t[1])[:5]}
# --- literal vs non-attested: does eng match an hgm_gloss of the headword?
cur.execute("select wylie,hgm_gloss from entries where hgm_gloss is not null and hgm_gloss not in ('[]','null')")
g={}
for w,gg in cur.fetchall():
    try: arr=json.loads(gg)
    except Exception: arr=[]
    s=g.setdefault(w,set())
    for a in arr:
        if isinstance(a,str): s.add(a.strip().lower())
ev=json.load(open(R+'data/alignment/alignment_evidence_v1.json'))
att=nonatt=nodict=0; examples=[]
for h,rs in ev['pairs'].items():
    for r in rs:
        e=r['eng'].strip().lower()
        if h not in g: nodict+=1
        elif e in g[h]: att+=1
        else:
            nonatt+=1
            if len(examples)<8: examples.append((h,r['eng']))
out['gloss_attestation']={'records':att+nonatt+nodict,'eng_matches_hgm_gloss':att,
  'headword_in_dict_but_eng_novel':nonatt,'headword_has_no_hgm_gloss':nodict,'examples_novel':examples}
# --- unusual spelling proxy: headword not in entries at all
cur.execute("select distinct wylie from entries")
allw={r[0] for r in cur.fetchall()}
out['unusual']={'evidence_headwords':len(ev['pairs']),
  'not_in_dictionary_at_all':sum(1 for h in ev['pairs'] if h not in allw)}
print(json.dumps(out,indent=1,default=str))
