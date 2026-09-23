import sys, json, gzip, re, collections
sys.path.insert(0,'/Users/adamderickandrade/ALL-translation-tool/engines')
from ewts_unicode import wylie_to_unicode
R='/Users/adamderickandrade/ALL-translation-tool/data/84000/'
g=json.load(open(R+'g84000.json'))['entries']
# terms: wylie -> (unicode, [english], set(toh))
terms={}
fail=0
for w,e in g.items():
    if len(w.split())<2: continue  # multi-syllable only, reduce substring noise
    try:
        u,ok=wylie_to_unicode(w)
        if not ok: fail+=1; continue
    except Exception:
        fail+=1; continue
    if not u or '⟨' in u or any(c.isascii() and c.isalpha() for c in u): fail+=1; continue
    eng=[re.sub(r'^<[^>]+>\s*','',x).strip() for x in e.get('g',[])]
    eng=[x for x in eng if x and len(x)>2]
    toh=set(e.get('t',[]))
    if eng and toh: terms[w]=(u.rstrip('་'),eng,toh)
print('multi-syl terms with eng+toh:',len(terms),'convfail',fail)
bytoh=collections.defaultdict(list)
for w,(u,eng,toh) in terms.items():
    for t in toh: bytoh[t].append((w,u,[x.lower() for x in eng]))
hits=0; segs_with=0; tohs=set(); termset=set(); n=0
with gzip.open(R+'tm_84000.tsv.gz','rt') as f:
    for line in f:
        if line.startswith('#') or line.startswith('text_id'): continue
        p=line.rstrip('\n').split('\t')
        if len(p)<6: continue
        n+=1
        toh=p[1].split('-')[0] if p[1].startswith('toh') else p[1]
        cands=bytoh.get(p[1]) or bytoh.get(toh) or []
        if not cands: continue
        tib=p[4]; en=p[5].lower(); k=0
        for w,u,engs in cands:
            if u in tib and any(re.search(r'\b'+re.escape(x)+r'\b',en) for x in engs):
                k+=1; termset.add(w)
        if k: segs_with+=1; hits+=k; tohs.add(toh)
print('TM segments',n,'segments with >=1 term pair',segs_with,'term-instance pairs',hits,'distinct terms',len(termset),'texts',len(tohs))
