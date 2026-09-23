import json, glob, re, collections
P='/Users/adamderickandrade/.claude/projects/-Users-adamderickandrade-ALL-translation-tool'
pat=re.compile(r"(hit your [a-z ]*limit|reached your [a-z ]*limit|weekly limit|session limit|spend limit|usage limit)[^\"\\]{0,90}",re.I)
hits=collections.OrderedDict()
for f in glob.glob(P+'/**/*.jsonl',recursive=True):
    for line in open(f,errors='ignore'):
        if 'limit' not in line: continue
        try: j=json.loads(line)
        except: continue
        if j.get('type')!='assistant': continue
        m=j.get('message') or {}
        if m.get('model')!='<synthetic>' and not j.get('isApiErrorMessage') and not j.get('error'): continue
        txt=json.dumps(m.get('content'))[:400]
        mm=pat.search(txt)
        if not mm: continue
        ts=j.get('timestamp','')[:16]
        key=(ts[:13], mm.group(0)[:110])
        hits[key]=hits.get(key,0)+1
for (t,s),n in sorted(hits.items()): print(t,n,s)
