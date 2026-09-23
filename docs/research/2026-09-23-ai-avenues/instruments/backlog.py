import re, io, os, glob, collections, json
ALIGN='/Users/adamderickandrade/ALL-translation-tool/data/alignment'
ID = re.compile(r'data-l="(s\d+\w+)"[^>]*>([^<]*)')
FN = r'(?:the|The|a|A|an|An|and|And|or|Or|his|His|our|Our|your|Your|I|you|You)(?=[ <])'
PAT = re.compile(r'<span class="u[^"]*" data-d="([57])" data-l="(s\d+\w+)">'
                 r'(?:<span class="u[^"]*" data-d="7" data-l="s\d+\w+">)?' + FN)
TIB=re.compile(r"^[a-zA-Z' /.\-]+$")
rows=[]
for d in sorted(x for x in os.listdir(ALIGN) if os.path.isdir(os.path.join(ALIGN,x)) and x.startswith('pages')):
    for f in sorted(glob.glob(os.path.join(ALIGN,d,'*.html'))):
        page=os.path.basename(f)[:-5]; html=io.open(f,encoding='utf-8').read()
        texts={}
        for i,t in ID.findall(html): texts.setdefault(i,[]).append(t.strip())
        for m in PAT.finditer(html):
            sid=m.group(2); ts=texts.get(sid,[])
            # heuristic split: the wylie side is the one matching the wylie charset
            tib=[t for t in ts if t and TIB.match(t) and not any(c.isupper() for c in t[:1])]
            eng=[t for t in ts if t not in tib]
            rows.append((d,page,sid,(tib[0] if tib else '?'),(eng[0] if eng else '?')))
print('flagged spans harvested: %d'%len(rows))
print('by directory:', dict(collections.Counter(r[0] for r in rows)))
lead=lambda e: e.split()[0] if e.split() else e
cls=collections.Counter((r[3],lead(r[4])) for r in rows)
print('\ndistinct (Tibetan headword, leading function word) classes: %d'%len(cls))
cum=0; tot=len(rows)
for n,(k,v) in enumerate(cls.most_common(),1):
    cum+=v
    if n in (1,5,10,20,50,100,200,500,len(cls)):
        print('  top %4d classes cover %5d of %d flagged spans (%.1f%%)'%(n,cum,tot,100*cum/tot))
print('\nclasses appearing once: %d (%.1f%% of classes, %d spans)'%(
    sum(1 for v in cls.values() if v==1),100*sum(1 for v in cls.values() if v==1)/len(cls),sum(1 for v in cls.values() if v==1)))
print('\nlargest classes (one ruling would dispose of each):')
for (t,e),v in cls.most_common(25):
    print('  %5d spans   %-24s -> "%s ..."'%(v,t[:24],e))
