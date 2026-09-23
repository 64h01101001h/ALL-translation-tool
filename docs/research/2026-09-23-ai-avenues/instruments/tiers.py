import sqlite3, json, collections, statistics
DB='/Users/adamderickandrade/ALL-translation-tool/build/hgm_spine_v27_2.db'
c=sqlite3.connect(DB)
rows=c.execute("SELECT wylie,tier,hgm_gloss FROM entries WHERE hgm_gloss IS NOT NULL AND hgm_gloss NOT IN ('[]','null')").fetchall()
byt=collections.defaultdict(list)
gl={}
for w,t,g in rows:
    try: arr=json.loads(g)
    except Exception: arr=[g]
    byt[t].append(len(arr)); gl.setdefault(w,{})[t]=arr
print('%-14s %6s %8s %8s %6s %8s'%('tier','n','mean K','median','max','%K>1'))
for t,v in sorted(byt.items(), key=lambda x:-len(x[1])):
    print('%-14s %6d %8.2f %8d %6d %7.1f%%'%(t,len(v),statistics.mean(v),statistics.median(v),max(v),100*sum(1 for k in v if k>1)/len(v)))

# bank vs curated
B='/Users/adamderickandrade/ALL-translation-tool/data/alignment/alignment_full_v1.json'
d=json.load(open(B)); L=d['links']
sub=[l for l in L if l['d']==5 and l.get('eng') and l.get('tib')]
rend=collections.defaultdict(collections.Counter)
for l in sub: rend[l['tib']][l['eng']]+=1
cur={w:v['curated'] for w,v in gl.items() if 'curated' in v}
print('\ncurated entries: %d ; of these present in the d=5 bank: %d'%(len(cur),sum(1 for w in cur if w in rend)))
print('\n%-18s %5s %7s %8s %9s'%('wylie','occ','K_bank','K_curated','ratio'))
rs=[]
for w in sorted(cur, key=lambda w:-sum(rend[w].values()) if w in rend else 0):
    if w not in rend: continue
    N=sum(rend[w].values()); kb=len(rend[w]); kc=len(cur[w])
    rs.append((N,kb,kc))
    if N>=8: print('%-18s %5d %7d %8d %9.1f'%(w,N,kb,kc,kb/kc))
if rs:
    print('\nover %d curated headwords present in the bank:'%len(rs))
    print('  total occurrences %d ; total bank renderings %d ; total curated senses %d'%(sum(r[0] for r in rs),sum(r[1] for r in rs),sum(r[2] for r in rs)))
    print('  mean K_bank %.2f vs mean K_curated %.2f ; median ratio %.2f'%(statistics.mean(r[1] for r in rs),statistics.mean(r[2] for r in rs),statistics.median(r[1]/r[2] for r in rs)))
    big=[r for r in rs if r[0]>=20]
    if big: print('  on the %d curated headwords with >=20 occurrences: mean K_bank %.1f, mean K_curated %.1f, mean ratio %.1f'%(len(big),statistics.mean(r[1] for r in big),statistics.mean(r[2] for r in big),statistics.mean(r[1]/r[2] for r in big)))
