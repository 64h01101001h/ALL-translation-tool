import json,glob,os,collections,statistics as st
root=os.path.expanduser('~/.claude/projects/-Users-adamderickandrade-ALL-translation-tool/')
bank=collections.defaultdict(set); bank5=collections.defaultdict(set)
d=json.load(open('/Users/adamderickandrade/ALL-translation-tool/data/alignment/alignment_full_v1.json'))
for x in d['links']:
    k=(x['course'],x['seg'])
    bank[k].add((x['d'],x['tib'].strip(),(x['eng'] or '').strip()))
    if x['d']==5 and x['eng']: bank5[k].add((x['tib'].strip(),x['eng'].strip()))
res=collections.defaultdict(list); n=0; nospec=0
for f in glob.glob(root+'*/subagents/**/*.jsonl',recursive=True):
    lines=open(f,errors='ignore').read().splitlines()
    if not lines: continue
    try: t=json.loads(lines[0])['message']['content']
    except: continue
    if isinstance(t,list): t=' '.join(x.get('text','') for x in t if isinstance(x,dict))
    if 'RECONCILER' in t[:600]: continue
    if not ('YOUR ANGLE IS' in t or 'PROPOSE_BRIEF' in t[:3000] or 'PROPOSE BRIEF' in t): continue
    angle='tib' if 'TIBETAN-FIRST' in t else ('eng' if 'ENGLISH-FIRST' in t else '?')
    spec=None
    for ln in lines:
        try: r=json.loads(ln)
        except: continue
        m=r.get('message') or {}
        if r.get('type')=='assistant' and isinstance(m.get('content'),list):
            for c in m['content']:
                if c.get('type')=='tool_use':
                    inp=c.get('input',{})
                    if str(inp.get('file_path','')).endswith('spec.json') and 'content' in inp:
                        spec=inp['content']
    if not spec: nospec+=1; continue
    try: sj=json.loads(spec)
    except: continue
    course=sj.get('course')
    for sg in sj.get('segments',[]):
        k=(course,sg.get('seq'))
        if k not in bank5: continue
        p5={(s['tib'].strip(),(s['eng'] or '').strip()) for s in sg.get('spans',[]) if s.get('d')==5 and s.get('eng')}
        if not p5 or not bank5[k]: continue
        inter=len(p5&bank5[k]); P=inter/len(p5); R=inter/len(bank5[k])
        F=0 if inter==0 else 2*P*R/(P+R)
        res[angle].append((P,R,F,course)); n+=1
print('proposal-segments matched to bank',n,'transcripts without Write spec',nospec)
for a,v in res.items():
    print(a,'n',len(v),'d5 exact (tib,eng) P median',round(st.median(x[0] for x in v),3),'R',round(st.median(x[1] for x in v),3),'F1 median',round(st.median(x[2] for x in v),3),'mean',round(st.mean(x[2] for x in v),3))
bc=collections.defaultdict(list)
for a,v in res.items():
    for x in v: bc[x[3]].append(x[2])
for c,v in sorted(bc.items()): print(c,len(v),'F1 mean',round(st.mean(v),3))
