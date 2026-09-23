import json,sqlite3,collections,re,math
R='/Users/adamderickandrade/ALL-translation-tool/'
full=json.load(open(R+'data/alignment/alignment_full_v1.json')); L=full['links']
ev=json.load(open(R+'data/alignment/alignment_evidence_v1.json'))
# --- what share of the bank has ever been inside an audit frame?
# audit frames: evidence layer (d=5 ACIP-keyed, non-null English)
evkeys=set()
for h,rs in ev['pairs'].items():
    for r in rs:
        for ref in r['refs']:
            c,s=ref.split(':'); evkeys.add((h,r['eng'],c,int(s)))
inframe=sum(1 for x in L if (x['tib'],x['eng'],x['course'],x['seg']) in evkeys)
print('bank links:',len(L))
print('links inside the evidence layer (the only audit frame ever used):',inframe, '= %.1f%%'%(100*inframe/len(L)))
print('links NEVER in any audit population:', len(L)-inframe, '= %.1f%%'%(100*(len(L)-inframe)/len(L)))
byd=collections.Counter()
for x in L:
    if (x['tib'],x['eng'],x['course'],x['seg']) not in evkeys: byd[(x['d'], x['eng'] is None)]+=1
print('never-audited by (depth, is_null):', dict(sorted(byd.items())))
# --- tier / sanskrit proxies for technical terminology
con=sqlite3.connect(R+'build/hgm_spine_v27_2.db');cur=con.cursor()
cur.execute("select wylie,tier,sanskrit from entries")
tier={}; skt={}
for w,t,s in cur.fetchall():
    tier.setdefault(w,t); 
    if s: skt.setdefault(w,s)
hw=list(ev['pairs'])
print('evidence headwords:',len(hw))
print('  tier mix:', collections.Counter(tier.get(h,'ABSENT') for h in hw).most_common())
print('  with a Sanskrit equivalent in the dictionary:', sum(1 for h in hw if h in skt))
# --- segments carrying a CONFIRMED erratum, landed
er=json.load(open(R+'docs/errata_register.json'))
landed={(x['course'],x['seg']) for x in L if x['seg'] is not None}
def parse(s):
    m=re.match(r'^([A-Z0-9]+):(\d+)$', s or ''); return (m.group(1),int(m.group(2))) if m else None
conf_segs={parse(x['segment']) for x in er if x.get('confidence')=='CONFIRMED' and parse(x.get('segment')) in landed}
print('landed segments carrying >=1 CONFIRMED erratum:', len(conf_segs))
# --- per-segment span counts in the BANK (for effort model)
per=collections.Counter()
for x in L:
    if x['seg'] is not None: per[(x['course'],x['seg'])]+=1
v=sorted(per.values())
print('bank spans per landed segment: mean %.1f median %d p90 %d max %d'%(sum(v)/len(v), v[len(v)//2], v[int(.9*len(v))], v[-1]))
# --- expert-hours table
import math
print()
print('=== expert hours for a gold book, by segments and minutes/segment ===')
print('%8s'%'G'+''.join('%9s'%('%dmin'%m) for m in [10,20,30,45,60,90]))
for G in [20,50,100,150,200,300]:
    print('%8d'%G+''.join('%9.1f'%(G*m/60) for m in [10,20,30,45,60,90]))
print('(double-labelling a fraction f multiplies by (1+f))')
