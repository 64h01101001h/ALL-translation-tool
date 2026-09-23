import json,collections,re,sqlite3
D=json.load(open('banks_overlap.json')); A=D['claude'];B=D['codex']
meta={int(k):v for k,v in json.load(open('seg_meta.json')).items()}
con=sqlite3.connect('file:/Users/adamderickandrade/ALL-translation-tool/build/hgm_spine_v27_2.db?mode=ro',uri=True)
EN={r[0]:r[1] for r in con.execute("select seq,english from corpus_segments where course='C05' and seq between 166 and 366")}
HEAD={'the','a','an','and','or','his','our','your','i','you'}
allow=json.load(open('/Users/adamderickandrade/ALL-translation-tool/data/alignment/span_head_allow.json'))
def head_flag(e):
    w=e.split()[0].lower().strip('"\'([') if e else ''
    return w in HEAD
def midword(seg,e):
    en=EN[seg]
    for frag in e.split(' … '):
        frag=frag.strip()
        if not frag: continue
        ok=False
        for m in re.finditer(re.escape(frag),en):
            s,t=m.start(),m.end()
            if (s==0 or not en[s-1].isalnum()) and (t==len(en) or not en[t].isalnum()): ok=True;break
        if not ok: return True
    return False
strata=collections.defaultdict(collections.Counter)
cls=collections.Counter()
for n in [k for k in A if int(k)>=60]:
    a=collections.Counter((x['seg'],x['tib'],x['eng']) for x in A[n] if x['d']==5 and x['eng'])
    b=collections.Counter((x['seg'],x['tib'],x['eng']) for x in B[n] if x['d']==5 and x['eng'])
    both=a&b; ao=a-b; bo=b-a
    for name,C in (('both',both),('claude_only',ao),('codex_only',bo)):
        for (seg,t,e),k in C.items():
            m=meta[seg]
            for s in ('ALL','verse' if m['verse'] else 'prose','dupC16' if m['c16'] else 'uniq'):
                strata[s][name]+=k
            cls[(name,'head')]+=k*head_flag(e); cls[(name,'mid')]+=k*midword(seg,e); cls[(name,'n')]+=k
for s,c in strata.items():
    ca=c['both']+c['claude_only']; cb=c['both']+c['codex_only']; u=c['both']+c['claude_only']+c['codex_only']
    print('%-7s claude=%5d codex=%5d both=%5d  both/claude=%.3f both/codex=%.3f jaccard=%.3f'%(s,ca,cb,c['both'],c['both']/ca,c['both']/cb,c['both']/u))
for name in ('both','claude_only','codex_only'):
    n=cls[(name,'n')]; print(name,n,'supplied-head %d (%.2f%%)'%(cls[(name,'head')],100*cls[(name,'head')]/n),'mid-word %d (%.2f%%)'%(cls[(name,'mid')],100*cls[(name,'mid')]/n))
