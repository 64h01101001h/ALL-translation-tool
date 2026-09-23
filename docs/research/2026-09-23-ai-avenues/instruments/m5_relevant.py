import json, collections, re, sqlite3
REPO='/Users/adamderickandrade/ALL-translation-tool/'
db=sqlite3.connect(REPO+'build/hgm_spine_v27_2.db')
bank=json.load(open(REPO+'data/alignment/alignment_full_v1.json'))
L=[x for x in bank['links'] if x['seg'] is not None]
ART=re.compile(r'^(the|a|an|of|to|in|for|that|this|his|her|their|its|our|your|my|and|is|are|be|been|will|would)\s+')
def nt(s): return (s or '').strip().strip(',.;:!?').lower()
def ne(s):
    if s is None: return None
    e=s.strip().lower().strip('".,;:!?()'); e=re.sub(r'\s+',' ',e); p=None
    while p!=e: p=e; e=ART.sub('',e)
    e=re.sub(r"(\w)'s$",r'\1',e)
    if len(e)>3 and e.endswith('s') and not e.endswith('ss'): e=e[:-1]
    return e
# master dictionary gloss index
gl=collections.defaultdict(set); has=set()
for wy,g,tier in db.execute("select wylie,hgm_gloss,tier from entries"):
    w=re.sub(r'^!+','',(wy or '').strip()).lower()
    if g:
        try: arr=json.loads(g)
        except: arr=[g]
        if arr:
            has.add(w)
            for a in arr:
                if isinstance(a,str): gl[w].add(ne(a))
print('dictionary headwords with an hgm_gloss: %d ; distinct normalised glosses %d'%(len(has), len(set().union(*gl.values()))))

D=[x for x in L if x['d'] in (5,7)]
occ=len(D)
nn=[x for x in D if x['eng']]
print('\n=== FUNNEL: from occurrence to dictionary-relevant evidence (landed 1,748 segments) ===')
print('1. all banked links                              %7d'%len(L))
print('2. word-layer occurrences (d=5 or d=7)           %7d'%occ)
print('3. ...with non-null English                      %7d  (%.1f%%)'%(len(nn),100*len(nn)/occ))
pairs=collections.Counter((nt(x['tib']),ne(x['eng'])) for x in nn)
print('4. distinct (tib, normalised eng) pairs          %7d  (%.1fx compression)'%(len(pairs),len(nn)/len(pairs)))
hw=collections.Counter(nt(x['tib']) for x in nn)
print('5. distinct Tibetan headwords                    %7d  (%.1fx compression)'%(len(hw),len(nn)/len(hw)))
known=sum(1 for (t,e) in pairs if e in gl.get(t,()))
print('6. pairs ALREADY in the master hgm_gloss         %7d  (%.1f%% of pairs)'%(known,100*known/len(pairs)))
nov=len(pairs)-known
print('7. pairs NOT already glossed = candidate new     %7d  (%.1f%% of pairs, %.2f%% of occurrences)'%(nov,100*nov/len(pairs),100*nov/occ))
inhw=sum(1 for (t,e) in pairs if t in has and e not in gl.get(t,()))
newhw=sum(1 for (t,e) in pairs if t not in has)
print('   ...of which: headword IS glossed, rendering is new (a NEW SENSE)  %6d'%inhw)
print('   ...           headword has no gloss at all (a NEW HEADWORD)       %6d'%newhw)
# how much does the corpus repeat itself at the pair level?
h=sum(1 for v in pairs.values() if v==1)
print('\npair-frequency: %d/%d (%.1f%%) attested exactly once; top-1%% of pairs carry %.1f%% of occurrences'%(
   h,len(pairs),100*h/len(pairs), 100*sum(sorted(pairs.values(),reverse=True)[:max(1,len(pairs)//100)])/len(nn)))
# per-segment dictionary-relevant yield
byseg=collections.defaultdict(list)
for x in nn: byseg[(x['course'],x['seg'])].append(x)
seen=set(); yields=[]
for k in sorted(byseg,key=lambda z:(z[0],z[1])):
    ps={(nt(x['tib']),ne(x['eng'])) for x in byseg[k]}
    new=[p for p in ps-seen if p[1] not in gl.get(p[0],())]
    yields.append(len(new)); seen|=ps
import statistics
print('\nNEW-AND-UNGLOSSED pairs per segment: mean %.2f median %.0f ; segments yielding ZERO: %d/%d (%.1f%%)'%(
   statistics.mean(yields), statistics.median(yields), sum(1 for y in yields if y==0), len(yields), 100*sum(1 for y in yields if y==0)/len(yields)))
N=len(yields)
print('by decile of reading order:', [round(sum(yields[int(N*d/10):int(N*(d+1)/10)])/max(1,int(N*(d+1)/10)-int(N*d/10)),2) for d in range(10)])
