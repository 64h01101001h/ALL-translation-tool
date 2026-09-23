import json, re, collections, sqlite3, sys, itertools
BANK = sys.argv[1]
ROOT='/Users/adamderickandrade/ALL-translation-tool'
L=[l for l in json.load(open(BANK))['links'] if l['seg'] is not None]
db=sqlite3.connect(f'file:{ROOT}/build/hgm_spine_v27_2.db?mode=ro',uri=True)
seg={(c,s):(w or '',e or '') for c,s,w,e in db.execute('select course,seq,wylie,english from corpus_segments')}
uniq={}
for k,t in seg.items(): uniq.setdefault(t,k)
texts=list(uniq); idx={t:i for i,t in enumerate(texts)}
def words(s): return re.findall(r"[a-z0-9']+", s.lower())
def syl_norm(w):
    w=re.sub(r"[,/|;:!?\[\]{}()\"@#*]+",' ',w.lower()); return ' '+' '.join(w.split())+' '
W=[syl_norm(w) for w,e in texts]; E=[' '+' '.join(words(e))+' ' for w,e in texts]
sp=collections.defaultdict(set); wp=collections.defaultdict(set)
for i,w in enumerate(W):
    for s in set(w.split()): sp[s].add(i)
for i,e in enumerate(E):
    for s in set(e.split()): wp[s].add(i)
cT={}; cE={}
def ST(t):
    if t not in cT:
        tn=syl_norm(t); sy=tn.split()
        cT[t]=set() if not sy else {i for i in set.intersection(*[sp.get(s,set()) for s in sy]) if tn in W[i]}
    return cT[t]
STOP={'the','a','an','of','to','and','or','in','is','that','this','it','be','for','on','as','with','by','his','our','your','we','you','i','my','are','was','not','which','there'}
def SE(e):
    # content-word core of the English span (drop supplied function words at the edges)
    ws=words(e)
    while ws and ws[0] in STOP: ws=ws[1:]
    while ws and ws[-1] in STOP: ws=ws[:-1]
    key=' '.join(ws)
    if key not in cE:
        cE[key]=set() if not ws else {i for i in set.intersection(*[wp.get(s,set()) for s in ws]) if ' '+key+' ' in E[i]}
    return cE[key]
def dice(t,e,own):
    A=ST(t)-{own}; B=SE(e)-{own}
    if len(A)<5 or len(B)<5: return None
    return 2*len(A&B)/(len(A)+len(B))
bys=collections.defaultdict(list)
for l in L:
    if l['d']==5 and l['eng'] and l['tib'].strip()==l['tib']:
        bys[(l['course'],l['seg'])].append(l)
flags=[]; npairs=0
for k,ls in bys.items():
    own=idx[seg[k]]
    for a,b in itertools.combinations(ls,2):
        if a['tib']==b['tib'] or a['eng']==b['eng']: continue
        d11=dice(a['tib'],a['eng'],own); d22=dice(b['tib'],b['eng'],own)
        d12=dice(a['tib'],b['eng'],own); d21=dice(b['tib'],a['eng'],own)
        if None in (d11,d22,d12,d21): continue
        npairs+=1
        if d12+d21 > 2*(d11+d22) and d12+d21>0.2:
            flags.append((k,a['tib'],a['eng'],b['tib'],b['eng'],round(d11,3),round(d22,3),round(d12,3),round(d21,3)))
print('segments',len(bys),'scorable link pairs',npairs,'swap flags',len(flags))
fl=set()
for f in flags: fl.add((f[1],f[2],f'{f[0][0]}:{f[0][1]}')); fl.add((f[3],f[4],f'{f[0][0]}:{f[0][1]}'))
print('links implicated', len(fl))
import random; random.seed(11)
for f in random.sample(flags,min(20,len(flags))): print('  ',f)
json.dump(sorted(fl),open(BANK+'.swapflags.json','w'))
if len(sys.argv)>2:
    v=json.load(open(f'{ROOT}/data/alignment/audit_verdicts_20260904.json'))
    c=collections.Counter()
    for it in v['items']:
        kind=(it['first_read'].get('kind') or (it.get('second_reader') or {}).get('kind')) if it['final']=='CONFIRMED_DEFECT' else 'OK'
        c[(kind,(it['headword'],it['eng'],it['ref']) in fl)]+=1
    print(c)
