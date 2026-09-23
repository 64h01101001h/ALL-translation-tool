import re, io, os, glob, collections, json
ROOT='/Users/adamderickandrade/ALL-translation-tool'
ALIGN=ROOT+'/data/alignment'
bank=json.load(open(ALIGN+'/alignment_full_v1.json'))
idx={}
for l in bank['links']:
    idx[(l['page'],l['id'])]=l
FN = r'(?:the|The|a|A|an|An|and|And|or|Or|his|His|our|Our|your|Your|I|you|You)(?=[ <])'
PAT = re.compile(r'<span class="u[^"]*" data-d="([57])" data-l="(s\d+\w+)">'
                 r'(?:<span class="u[^"]*" data-d="7" data-l="s\d+\w+">)?' + FN)
rows=[]; miss=0
for d in sorted(x for x in os.listdir(ALIGN) if os.path.isdir(os.path.join(ALIGN,x)) and x.startswith('pages')):
    for f in sorted(glob.glob(os.path.join(ALIGN,d,'*.html'))):
        page=os.path.basename(f)[:-5]; html=io.open(f,encoding='utf-8').read()
        for m in PAT.finditer(html):
            l=idx.get((page,m.group(2)))
            if l is None: miss+=1; continue
            rows.append((d,page,m.group(2),l.get('tib'),l.get('eng')))
print('flagged spans joined to bank: %d (unjoined %d)'%(len(rows),miss))
lead=lambda e:(e or '').split()[0] if (e or '').split() else ''
cls=collections.Counter((r[3],lead(r[4])) for r in rows)
print('distinct (Tibetan, leading word) classes: %d  -> collapse %.2fx'%(len(cls),len(rows)/len(cls)))
print('singleton classes: %d (%.1f%% of classes; %d spans = %.1f%% of backlog)'%(
   sum(1 for v in cls.values() if v==1),100*sum(1 for v in cls.values() if v==1)/len(cls),
   sum(1 for v in cls.values() if v==1),100*sum(1 for v in cls.values() if v==1)/len(rows)))
# collapse on Tibetan alone (the licensor is the Tibetan word, per the gate's own argument)
t=collections.Counter(r[3] for r in rows)
print('\ndistinct TIBETAN licensor candidates: %d -> collapse %.2fx'%(len(t),len(rows)/len(t)))
cum=0
for n,(k,v) in enumerate(t.most_common(),1):
    cum+=v
    if n in (10,25,50,100,200,400): print('  top %3d Tibetan words cover %5d of %d spans (%.1f%%)'%(n,cum,len(rows),100*cum/len(rows)))
print('  singletons: %d Tibetan words appear once (%.1f%% of spans)'%(sum(1 for v in t.values() if v==1),100*sum(1 for v in t.values() if v==1)/len(rows)))
print('\nlargest Tibetan classes and the English heads they carry:')
byt=collections.defaultdict(collections.Counter)
for d,p,i,tb,e in rows: byt[tb][lead(e)]+=1
for k,v in t.most_common(18):
    print('  %4d  %-22s  heads: %s'%(v,(k or 'NULL')[:22],', '.join('%s(%d)'%(a,b) for a,b in byt[k].most_common(6))))
