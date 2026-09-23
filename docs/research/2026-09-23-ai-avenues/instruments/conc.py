import re,glob,collections,os,json
ROOT='/Users/adamderickandrade/ALL-translation-tool/'
FN = r'(?:the|The|a|A|an|An|and|And|or|Or|his|His|our|Our|your|Your|I|you|You)(?=[ <])'
PAT = re.compile(r'<span class="u[^"]*" data-d="([57])" data-l="(s\d+\w+)">')
flag=collections.Counter(); segflag=collections.Counter(); perseg=collections.Counter()
tot=0
for d in ['pages','pages_c01','pages_c03']:
    for f in sorted(glob.glob(ROOT+'data/alignment/%s/*.html'%d)):
        t=open(f,encoding='utf-8',errors='replace').read()
        for m in PAT.finditer(t):
            depth,lid=m.groups()
            tail=t[m.end():m.end()+40]
            body=tail.split('<')[0]
            if re.match(FN, body+' '):
                tot+=1
                seg=re.match(r's(\d+)',lid).group(1)
                key=(d,os.path.basename(f),seg)
                perseg[key]+=1
print('flagged spans (approx re-derivation):',tot)
print('distinct (dir,page,seg) carrying >=1 flag:',len(perseg))
v=sorted(perseg.values(),reverse=True)
print('flags per flagged segment: max',v[0],'median',v[len(v)//2],'mean',round(sum(v)/len(v),2))
import statistics
print('segments needed to cover 50%% of flags:', next(i+1 for i,_ in enumerate(v) if sum(v[:i+1])>=tot*0.5))
print('segments needed to cover 80%% of flags:', next(i+1 for i,_ in enumerate(v) if sum(v[:i+1])>=tot*0.8))
