import sys,io,os,glob,json,collections
sys.path.insert(0,'tools')
import test_no_supplied_span_head as g
allow=json.load(io.open(g.ALLOW,encoding='utf-8'))
tot=collections.Counter()
for d in ["pages_c01","pages","pages_c03"]:
    c=collections.Counter()
    for f in sorted(glob.glob(os.path.join(g.ALIGN,d,"*.html"))):
        page=os.path.basename(f)[:-5]; html=io.open(f,encoding='utf-8').read()
        texts={}
        for i,t in g.ID.findall(html): texts.setdefault(i,[]).append(t.strip())
        for m in g.PAT.finditer(html):
            key="%s/%s"%(page,m.group(2))
            if key in allow.get(d,{}): continue
            eng=m.group(0).split(">")[-1]
            if g.licensed_by_class(texts.get(m.group(2),[]),eng): continue
            c[eng.lower()]+=1
    print(d,sum(c.values()),dict(c.most_common()))
    tot.update(c)
print("TOTAL",sum(tot.values()),"articles",tot['the']+tot['a']+tot['an'],dict(tot.most_common()))
