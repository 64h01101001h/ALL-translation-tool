import gzip,json,re,os,collections,sys,time
segs=json.load(gzip.open('/Users/adamderickandrade/ALL-translation-tool/data/full_parallel_corpus_v32.json.gz'))
mand={f"C{i:02d}" for i in range(1,19)}-{"C13"}
K=6
def syl(s):
    s=s.upper()
    s=re.sub(r'\[[^\]]*\]','',s)      # ACIP comments
    s=re.sub(r'@\S*','',s)            # folio markers
    s=re.sub(r"[^A-Z'%\-\+]", ' ', s)
    return [t for t in s.split() if t]
probes=collections.defaultdict(set)   # shingle -> seg ids
segsyl={}
for i,s in enumerate(segs):
    if s['course'] not in mand: continue
    t=syl(s['acip']); segsyl[i]=t
    for j in range(0,max(1,len(t)-K+1)):
        sh=tuple(t[j:j+K])
        if len(sh)==K: probes[sh].add(i)
print('mandate segs',len(segsyl),'probe shingles',len(probes),file=sys.stderr)
hit=collections.defaultdict(lambda: collections.Counter())  # seg -> file -> nshingles
root='/Users/adamderickandrade/ALL-translation-tool/library'
t0=time.time(); nf=0
for coll in ['sungbum','kangyur','tengyur']:
    for dp,dn,fn in os.walk(os.path.join(root,coll)):
        for f in fn:
            if not f.upper().endswith('.TXT') or 'META' in f.upper(): continue
            p=os.path.join(dp,f)
            try: txt=open(p,encoding='latin-1').read()
            except Exception as e: continue
            t=syl(txt); nf+=1
            tag=coll+'/'+f[:8]
            for j in range(len(t)-K+1):
                sh=tuple(t[j:j+K])
                ids=probes.get(sh)
                if ids:
                    for i in ids: hit[i][tag]+=1
print('files',nf,'time',time.time()-t0,file=sys.stderr)
out={}
for i,t in segsyl.items():
    nsh=max(1,len(t)-K+1)
    best=hit[i].most_common(1)
    out[i]={'course':segs[i]['course'],'nsyl':len(t),'nsh':nsh,'best':best[0] if best else None}
json.dump(out,open('src_match.json','w'))
