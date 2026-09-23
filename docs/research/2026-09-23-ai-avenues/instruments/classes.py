import sys, os, re, io, glob, json, collections
sys.path.insert(0, 'tools')
import test_no_supplied_span_head as g
ALIGN='data/alignment'
allow=json.load(io.open(g.ALLOW,encoding='utf-8'))
hits=[]
for d in ['pages','pages_c01','pages_c03']:
    for f in sorted(glob.glob(os.path.join(ALIGN,d,'*.html'))):
        page=os.path.basename(f)[:-5]
        html=io.open(f,encoding='utf-8').read()
        texts={}
        for i,t in g.ID.findall(html): texts.setdefault(i,[]).append(t.strip())
        for m in g.PAT.finditer(html):
            key="%s/%s"%(page,m.group(2))
            if key in allow.get(d,{}): continue
            tibs=texts.get(m.group(2),[])
            eng=m.group(0).split(">")[-1]
            if g.licensed_by_class(tibs,eng): continue
            # full english text of the span: find in texts the non-tib one
            hits.append((d,page,m.group(2),m.group(1),eng,tibs))
print('hits',len(hits), collections.Counter(h[0] for h in hits))
fw=collections.Counter(h[4] for h in hits); print('function word', fw.most_common())
# classes by (tib side first text, function word)
cls=collections.Counter()
for h in hits:
    tibs=h[5]
    tib=tibs[0] if tibs else ''
    cls[(tib, h[4].lower())]+=1
print('distinct (tib,fw) classes', len(cls))
tot=len(hits); acc=0
for i,(k,c) in enumerate(cls.most_common()):
    acc+=c
    if i in (9,49,99,199,499): print('top',i+1,'classes cover',acc,'=%.1f%%'%(100*acc/tot))
print(cls.most_common(25))
sing=sum(1 for c in cls.values() if c==1); print('singleton classes',sing)
fwc=collections.Counter(h[4].lower() for h in hits)
print('-----')
art=[h for h in hits if h[4].lower() in ('the','a','an')]
DEM=re.compile(r"(^|\s)('di|de|gcig|cig|zhig|shig|ci|gang|'ga'|kha cig|la la|thams cad|de dag|'di dag|de nyid|'di nyid|de ltar|de ltar na)(\s|$|'i|s)")
dem=[h for h in art if any(DEM.search(t) for t in h[5])]
empty=[h for h in art if not h[5] or all(t=='' for t in h[5])]
print('articles',len(art),'with demonstrative/numeral/quantifier in Tibetan',len(dem),'empty tib',len(empty))
print('remaining article spans with no such licensor candidate', len(art)-len(dem)-len(empty))
other=[h for h in hits if h[4].lower() not in ('the','a','an')]
print('non-article hits', len(other), collections.Counter(h[4].lower() for h in other))
# examples of empty-tib
for h in empty[:5]: print(h[:5], h[5])
print('=====')
rest=[h for h in art if h not in dem and h not in empty]
hw=collections.Counter()
for h in rest:
    t=[x for x in h[5] if x]; hw[t[-1] if t else '']+=1
print('distinct Tibetan headwords among rule-governed article spans', len(hw))
acc=0;tot=len(rest)
for i,(k,c) in enumerate(hw.most_common()):
    acc+=c
    if i+1 in (25,50,100,200,400): print(' top',i+1,'headwords cover',acc,'of',tot,'=%.1f%%'%(100*acc/tot))
print(' singleton headwords', sum(1 for c in hw.values() if c==1))
