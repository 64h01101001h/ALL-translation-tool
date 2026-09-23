import json, random, collections, re, sqlite3
R='/Users/adamderickandrade/ALL-translation-tool/'
bank=json.load(open(R+'data/alignment/alignment_full_v1.json'))['links']
d5=[x for x in bank if x['d']==5 and x.get('eng') and x.get('tib')]
print('d5 links',len(d5),'bank links',len(bank))
# segment text for dedup: identical wylie segments count once
con=sqlite3.connect('file:'+R+'build/hgm_spine_v27_2.db?mode=ro',uri=True)
W={(c,s):w for c,s,w in con.execute("select course,seq,wylie from corpus_segments")}
norm=lambda t:' '.join(re.sub(r"[/,;|_*!]+"," ",t).split())
by=collections.defaultdict(list)
for x in d5: by[norm(x['tib'])].append(x)
random.seed(7); sample=random.sample(d5,2000)
res=collections.Counter()
for x in sample:
    t=norm(x['tib']); k=(x['course'],x['seg']); wk=W.get(k)
    # leave out same segment AND any segment with identical wylie text (duplicates)
    others=[y for y in by[t] if (y['course'],y['seg'])!=k and W.get((y['course'],y['seg']))!=wk]
    if not others: res['no_precedent']+=1; continue
    c=collections.Counter(y['eng'].strip().lower() for y in others)
    e=x['eng'].strip().lower()
    top=[g for g,_ in c.most_common()]
    res['has_precedent']+=1
    if top[0]==e: res['prec_top1']+=1
    if e in top[:3]: res['prec_top3']+=1
    if e in c: res['prec_any']+=1
n=len(sample)
print(dict(res))
print('no precedent %.1f%%; top1 of all %.1f%%; any-precedent-match of all %.1f%%'%(100*res['no_precedent']/n,100*res['prec_top1']/n,100*res['prec_any']/n))
print('among with-precedent: top1 %.1f%% top3 %.1f%% any %.1f%%'%(100*res['prec_top1']/res['has_precedent'],100*res['prec_top3']/res['has_precedent'],100*res['prec_any']/res['has_precedent']))
