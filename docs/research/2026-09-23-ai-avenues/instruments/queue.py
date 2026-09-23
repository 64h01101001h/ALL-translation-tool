import re,io,os,glob,json,collections,sys,time
sys.path.insert(0,"/Users/adamderickandrade/ALL-translation-tool/tools")
import importlib.util
spec=importlib.util.spec_from_file_location("g","/Users/adamderickandrade/ALL-translation-tool/tools/test_no_supplied_span_head.py")
g=importlib.util.module_from_spec(spec); spec.loader.exec_module.__self__ if False else None
# exec module body without running main()
src=open("/Users/adamderickandrade/ALL-translation-tool/tools/test_no_supplied_span_head.py").read()
ns={"__name__":"notmain","__file__":"/Users/adamderickandrade/ALL-translation-tool/tools/test_no_supplied_span_head.py"}
exec(compile(src,"g","exec"),ns)
ALIGN=ns["ALIGN"]; ID=ns["ID"]; PAT=ns["PAT"]; allow=json.load(open(ns["ALLOW"]))
lic=ns["licensed_by_class"]
t0=time.time()
rows=[]
for d in ["pages","pages_c01","pages_c03"]:
    for f in sorted(glob.glob(os.path.join(ALIGN,d,"*.html"))):
        page=os.path.basename(f)[:-5]; html=io.open(f,encoding="utf-8").read()
        texts={}
        for i,t in ID.findall(html): texts.setdefault(i,[]).append(t.strip())
        for m in PAT.finditer(html):
            key="%s/%s"%(page,m.group(2))
            if key in allow.get(d,{}): continue
            tibs=texts.get(m.group(2),[])
            engword=m.group(0).split(">")[-1]
            if lic(tibs,engword): continue
            # tibetan side = the text under this id that is NOT the english one
            tib=None
            for t in tibs:
                if t and t!=engword and not re.match(r'^[A-Za-z0-9 ,.\'"()\-;:!?]+$',t): tib=t; break
            if tib is None:
                # wylie pages: the tib column is latin too; take the first differing text
                cand=[t for t in tibs if t]
                tib=cand[0] if cand else ""
            rows.append((d,page,m.group(2),tib,engword))
print("flags collected: %d in %.1fs"%(len(rows),time.time()-t0))
tibs=collections.Counter(r[3] for r in rows)
print("distinct Tibetan span-heads carrying a flag: %d"%len(tibs))
cum=0
for i,(t,n) in enumerate(tibs.most_common(40)):
    cum+=n
    if i<25: print("   %5d  %-42s (cum %5d, %4.1f%%)"%(n,t[:42],cum,100*cum/len(rows)))
tot=len(rows)
for k in (10,25,50,100,200,400):
    c=sum(n for _,n in tibs.most_common(k))
    print("   top %4d distinct Tibetan heads cover %5d of %d flags = %.1f%%"%(k,c,tot,100*c/tot))
sing=sum(1 for t,n in tibs.items() if n==1)
print("   singleton heads (one flag only): %d (%.1f%% of heads, %.1f%% of flags)"%(sing,100*sing/len(tibs),100*sing/tot))
