import sys, os, re, io, glob, json, collections
sys.path.insert(0, 'tools')
import test_no_supplied_span_head as g
ALIGN='data/alignment'
allow=json.load(io.open(g.ALLOW,encoding='utf-8'))
DEM=re.compile(r"(^|\s)('di|de|gcig|cig|zhig|shig|ci|gang|kha cig|la la|thams cad|de dag|'di dag|de nyid|'di nyid)(\s|$|'i|s)")
n=collections.Counter()
ex=collections.defaultdict(list); ALL=[]
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
                if any(DEM.search(t) for t in tibs): cat='article+tib demonstrative/numeral (judge)'
                elif full.lower()==fw: cat='bare article (judge)'
                else:
                    nxt=tail.strip().split(' ')[0] if tail.strip() else ''
                    cat='article + Capitalised next word (epithet/title? judge)' if nxt[:1].isupper() else 'article + lowercase (Rule 7 trim candidate)'
            else:
                cat='pronoun/conjunction (judge)'
            n[cat]+=1; ALL.append((cat,full.lower()))
            if len(ex[cat])<4: ex[cat].append((key, full, [t for t in tibs if t][:1]))
tot=sum(n.values()); print('total',tot)
for k,v in n.most_common(): print('%5d %5.1f%%  %s'%(v,100*v/tot,k)); [print('        ',e) for e in ex[k]]
print('#####')
for c in set(a for a,_ in ALL):
    items=[e for a,e in ALL if a==c]
    print('%-55s items %4d distinct English %4d'%(c,len(items),len(set(items))))
