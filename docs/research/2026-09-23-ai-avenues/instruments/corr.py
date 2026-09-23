import sqlite3, json, collections, statistics, math
c=sqlite3.connect('/Users/adamderickandrade/ALL-translation-tool/build/hgm_spine_v27_2.db')
gl={}
for w,t,g in c.execute("SELECT wylie,tier,hgm_gloss FROM entries WHERE hgm_gloss IS NOT NULL AND hgm_gloss NOT IN ('[]','null')"):
    try: arr=json.loads(g)
    except Exception: arr=[g]
    gl.setdefault(w,{})[t]=arr
d=json.load(open('/Users/adamderickandrade/ALL-translation-tool/data/alignment/alignment_full_v1.json'))
rend=collections.defaultdict(collections.Counter)
for l in d['links']:
    if l['d']==5 and l.get('eng') and l.get('tib'): rend[l['tib']][l['eng']]+=1
def pearson(xs,ys):
    mx=statistics.mean(xs); my=statistics.mean(ys)
    num=sum((x-mx)*(y-my) for x,y in zip(xs,ys))
    den=math.sqrt(sum((x-mx)**2 for x in xs)*sum((y-my)**2 for y in ys))
    return num/den if den else float('nan')
cur={w:v['curated'] for w,v in gl.items() if 'curated' in v and w in rend}
N=[sum(rend[w].values()) for w in cur]; KB=[len(rend[w]) for w in cur]; KC=[len(cur[w]) for w in cur]
print('curated headwords in bank: %d'%len(cur))
print('r(occurrences, K_bank)    = %+.3f'%pearson(N,KB))
print('r(occurrences, K_curated) = %+.3f'%pearson(N,KC))
print('r(log occ, K_bank)        = %+.3f'%pearson([math.log(x) for x in N],KB))
print('r(log occ, K_curated)     = %+.3f'%pearson([math.log(x) for x in N],KC))
# binned
print('\n%-14s %4s %9s %11s'%('occ band','n','mean K_bank','mean K_cur'))
bands=[(1,2),(3,5),(6,10),(11,20),(21,60),(61,10**6)]
for lo,hi in bands:
    sel=[w for w in cur if lo<=sum(rend[w].values())<=hi]
    if sel: print('%-14s %4d %9.2f %11.2f'%('%d-%d'%(lo,hi if hi<10**6 else 999),len(sel),statistics.mean(len(rend[w]) for w in sel),statistics.mean(len(cur[w]) for w in sel)))
# same for glossary tier (machine-assembled from HGM glossaries)
gls={w:v['glossary'] for w,v in gl.items() if 'glossary' in v and w in rend}
print('\nglossary-tier headwords in bank: %d'%len(gls))
N2=[sum(rend[w].values()) for w in gls]; K2=[len(gls[w]) for w in gls]
print('r(occurrences, K_glossary) = %+.3f  mean K %.2f'%(pearson(N2,K2),statistics.mean(K2)))
print('\n%-14s %4s %11s'%('occ band','n','mean K_gloss'))
for lo,hi in bands:
    sel=[w for w in gls if lo<=sum(rend[w].values())<=hi]
    if sel: print('%-14s %4d %11.2f'%('%d-%d'%(lo,hi if hi<10**6 else 999),len(sel),statistics.mean(len(gls[w]) for w in sel)))
