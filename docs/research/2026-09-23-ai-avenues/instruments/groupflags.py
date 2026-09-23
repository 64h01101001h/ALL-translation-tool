import sys,os,io,glob,json,collections,re
ROOT="/Users/adamderickandrade/ALL-translation-tool"
sys.path.insert(0,ROOT+"/tools")
import test_no_supplied_span_head as g
ALIGN=g.ALIGN
allow=json.load(io.open(g.ALLOW,encoding="utf-8"))
hits=[]
for d in ("pages_c01","pages","pages_c03"):
    for f in sorted(glob.glob(os.path.join(ALIGN,d,"*.html"))):
        page=os.path.basename(f)[:-5]; html=io.open(f,encoding="utf-8").read()
        texts={}
        for i,t in g.ID.findall(html): texts.setdefault(i,[]).append(t.strip())
        for m in g.PAT.finditer(html):
            key="%s/%s"%(page,m.group(2))
            if key in allow.get(d,{}): continue
            tibs=texts.get(m.group(2),[])
            if g.licensed_by_class(tibs,m.group(0).split(">")[-1]): continue
            fw=m.group(0).split(">")[-1]
            # tibetan side = the text that isn't english-looking: take first text
            hits.append((d,page,m.group(2),fw.lower(),tuple(tibs)))
print("flagged",len(hits))
# bank lookup for tib per span id
L=json.load(open(ROOT+"/data/alignment/alignment_full_v1.json"))["links"]
byid=collections.defaultdict(list)
for l in L: byid[(l["page"],l["id"])].append(l)
grp=collections.Counter(); tibonly=collections.Counter(); miss=0
for d,page,i,fw,tibs in hits:
    ls=byid.get((page,i))
    if not ls: miss+=1; continue
    t=ls[0]["tib"]
    grp[(t,fw)]+=1; tibonly[t]+=1
print("unmatched to bank",miss)
print("distinct (tib, leading function word) groups:",len(grp))
print("distinct tib headwords:",len(tibonly))
cum=0; tot=sum(grp.values())
for n,(k,v) in enumerate(grp.most_common()):
    cum+=v
    if n+1 in (10,25,50,100,200):
        print("  top %d groups cover %d of %d flagged (%.1f%%)"%(n+1,cum,tot,100*cum/tot))
print("singleton groups:",sum(1 for v in grp.values() if v==1))
print(grp.most_common(12))
