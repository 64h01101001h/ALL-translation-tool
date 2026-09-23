import sys, json, sqlite3, re, random, collections
R='/Users/adamderickandrade/ALL-translation-tool/'
sys.path.insert(0,R+'engines')
from ewts_unicode import wylie_to_unicode
# load botok token streams keyed by segment unicode
tok={}
with open(R+'build/botok_tok_corpus.tsv',encoding='utf-8') as f:
    for line in f:
        key,_,rest=line.rstrip('\n').partition('\t')
        toks=re.findall(r'text=(.*?)\x1ftype=(\w+)\x1fstart=(\d+)\x1flen=(\d+)',rest)
        tok[key]=[(t,ty,int(s),int(l)) for t,ty,s,l in toks]
print('botok segs',len(tok))
con=sqlite3.connect('file:'+R+'build/hgm_spine_v27_2.db?mode=ro',uri=True)
W={(c,s):w for c,s,w in con.execute("select course,seq,wylie from corpus_segments")}
bank=json.load(open(R+'data/alignment/alignment_full_v1.json'))['links']
d5=[x for x in bank if x['d']==5 and x.get('tib')]
random.seed(3); sample=random.sample(d5,3000)
res=collections.Counter(); nsyl=collections.Counter()
for x in sample:
    w=W.get((x['course'],x['seg']))
    if not w: res['noseg']+=1; continue
    u,ok=wylie_to_unicode(w)
    if not ok or u not in tok: res['seg_not_in_botok_ref']+=1; continue
    su,ok2=wylie_to_unicode(x['tib'])
    if not ok2 or not su: res['span_conv_fail']+=1; continue
    su=su.strip('་ ')
    pos=[m.start() for m in re.finditer(re.escape(su),u)]
    if not pos: res['span_not_found_in_uni']+=1; continue
    toks=tok[u]
    starts=set(s for t,ty,s,l in toks); ends=set()
    for t,ty,s,l in toks:
        e=s+l
        # tolerate trailing tsheg inside token
        ends.add(e); 
        if t.endswith('་'): ends.add(e-1)
    single=set()
    for t,ty,s,l in toks:
        single.add((s,t.rstrip('་')))
    best='crosses'
    for p in pos:
        e=p+len(su)
        if (p,su) in single: best='one_token'; break
        if p in starts and (e in ends or e+1 in starts or e in starts): best='whole_tokens'
    res[best]+=1
    nsyl[len(x['tib'].split())]+=1
n=res['one_token']+res['whole_tokens']+res['crosses']
print(dict(res))
print('one botok token %.1f%%, several whole tokens %.1f%%, crosses a token boundary %.1f%% (n=%d)'%(100*res['one_token']/n,100*res['whole_tokens']/n,100*res['crosses']/n,n))
print('d5 syllable lengths',sorted(nsyl.items())[:8])
