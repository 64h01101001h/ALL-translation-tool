import json,glob,os,collections,re,itertools,statistics
base=os.path.expanduser('~/.claude/projects/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2/subagents/workflows')
files=sorted(glob.glob(base+'/*/agent-*.jsonl'))
cmds=[]
for f in files:
    for line in open(f,encoding='utf-8',errors='replace'):
        if 'alignment_full' not in line: continue
        try: o=json.loads(line)
        except Exception: continue
        m=o.get('message') or {}
        cl=m.get('content')
        if not isinstance(cl,list): continue
        for c in cl:
            if isinstance(c,dict) and c.get('type')=='tool_use' and c.get('name')=='Bash':
                cmd=(c.get('input') or {}).get('command','')
                if 'alignment_full' in cmd: cmds.append(cmd)
print('precedent-lookup Bash commands across ALL %d agent transcripts: %d'%(len(files),len(cmds)))
schema=sum(1 for c in cmds if re.search(r"list\(d\)|list\(d\.keys|links\[0\]|links\[:3\]|type\(d\)|\.keys\(\)",c))
mc=[int(m.group(1)) for c in cmds for m in [re.search(r'most_common\((\d+)\)',c)] if m]
print('  commands that re-discover the file schema: %d (%.1f%%)'%(schema,100*schema/len(cmds)))
print('  commands that truncate with most_common(N): %d ; N values: %s'%(len(mc),collections.Counter(mc).most_common(8)))
print('  commands loading the whole 10.9 MB JSON with json.load: %d (%.1f%%)'%(sum(1 for c in cmds if 'json.load' in c),100*sum(1 for c in cmds if 'json.load' in c)/len(cmds)))
# what does most_common(12) hide?
d=json.load(open('/Users/adamderickandrade/ALL-translation-tool/data/alignment/alignment_full_v1.json'))
rend=collections.defaultdict(collections.Counter)
for l in d['links']:
    if l.get('eng') and l.get('tib'): rend[l['tib']][(l['d'],l['eng'])]+=1
cov=[];hid=0;heads=0
for t,c in rend.items():
    if len(c)<=12: continue
    heads+=1
    N=sum(c.values()); top=sum(n for _,n in c.most_common(12))
    cov.append(top/N); hid+=len(c)-12
print('\nWHAT THE AGENTS\' OWN most_common(12) HIDES (all depths, as their code queries it)')
print('  headwords with >12 distinct (depth,rendering) pairs: %d'%heads)
print('  distinct renderings never shown: %d'%hid)
print('  median occurrence coverage of the top 12: %.1f%% (mean %.1f%%)'%(100*statistics.median(cov),100*statistics.mean(cov)))
worst=sorted(((sum(n for _,n in c.most_common(12))/sum(c.values()),t,len(c)) for t,c in rend.items() if len(c)>12))[:10]
print('  worst-covered headwords:', ', '.join('%s(K=%d, top12=%.0f%%)'%(t,k,100*f) for f,t,k in worst))
