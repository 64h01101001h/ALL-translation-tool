import sys, os, re, io, glob, json, collections, random
sys.path.insert(0, 'tools')
import test_no_supplied_span_head as g
ALIGN='data/alignment'
allow=json.load(io.open(g.ALLOW,encoding='utf-8'))
DEM=re.compile(r"(^|\s)('di|de|gcig|cig|zhig|shig|ci|gang|kha cig|la la|thams cad|de dag|'di dag|de nyid|'di nyid)(\s|$|'i|s)")
rows=[]
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
            tail=html[m.end():m.end()+80].split('<')[0]
            full=(eng+tail).strip()
            fw=eng.lower()
            if fw in ('the','a','an'):
                if any(DEM.search(t) for t in tibs): cat='d'
                elif full.lower()==fw: cat='c'
                else:
                    nxt=tail.strip().split(' ')[0] if tail.strip() else ''
                    cat='b' if nxt[:1].isupper() else 'a'
            else: cat='e'
            rows.append((cat,key,full,tibs[:1],eng))
c=collections.Counter(r[0] for r in rows); print(c)
# kind c: tib equal to english text (gate FP on Tibetan column)?
kc=[r for r in rows if r[0]=='c']
print('kind c tib==the:', sum(1 for r in kc if r[3] and r[3][0].strip().lower().startswith('the')), 'of', len(kc))
print(collections.Counter(r[2] for r in kc))
# any flags where the tib text begins with 'the ' (tshom)?
print('all kinds, tib starts the:', sum(1 for r in rows if r[3] and r[3][0].lower().startswith('the')))
# kind a: first-word is 'a'/'an' vs 'the'
ka=[r for r in rows if r[0]=='a']
print('kind a by article', collections.Counter(r[4].lower() for r in ka))
# kind a with genitive/possessive (rang gi, bdag gi, kyi/gyi/gi/'i) in tib
GEN=re.compile(r"(rang gi|bdag gi|rang re'i|khyod kyi|\bkyi\b|\bgyi\b|\bgi\b|'i\b)")
print('kind a with genitive/possessive in tib', sum(1 for r in ka if any(GEN.search(t) for t in r[3])))
# kind a with numeral words not in DEM
NUM=re.compile(r"\b(gnyis|gsum|bzhi|lnga|drug|bdun|brgyad|dgu|bcu)")
print('kind a with numeral gnyis..bcu', sum(1 for r in ka if any(NUM.search(t) for t in r[3])))
# kind b sentence-initial 'The' + capitalised (heading/title) share where tib looks English
kb=[r for r in rows if r[0]=='b']
print('kind b where tib contains uppercase english-like', sum(1 for r in kb if r[3] and re.search(r'[A-Z][a-z]+ [A-Za-z]',r[3][0])))
random.seed(20260923)
for r in random.sample(ka,25): print(r[1], '|', r[2][:60], '|', r[3])
print('=== entity-decoding check')
import html as H
ka_ent=[r for r in ka if r[3] and '&#' in r[3][0]]
print('kind a with entity-encoded tib', len(ka_ent))
moved=[r for r in ka if any(DEM.search(H.unescape(t)) for t in r[3])]
print('kind a that DEM matches after unescape (should be kind d):', len(moved))
for r in moved[:12]: print('   ', r[1], '|', r[2][:50], '|', H.unescape(r[3][0]))
allent=sum(1 for r in rows if r[3] and '&#' in r[3][0]); print('all flags with entity tib', allent)
IDI=re.compile(r"^(a|an) (number|great|kind|few|little|lot|single|bit|while|certain|whole|couple|sort|type|variety|total|good deal|hundred|thousand|million)\b", re.I)
idi=[r for r in ka if IDI.search(r[2])]
print('kind a starting with quantifier/idiom a/an X:', len(idi), collections.Counter(' '.join(r[2].split()[:2]).lower() for r in idi).most_common(15))
