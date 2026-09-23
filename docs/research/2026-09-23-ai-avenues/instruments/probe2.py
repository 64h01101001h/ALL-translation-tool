import json, re, collections, random, sqlite3
exec(open('labelfree.py').read().split('# ---------- D.')[0].replace("print(","(lambda *a,**k:None)("))
FN = {'the','a','an','and','or','his','our','your','i','you'}
first = lambda s: (re.findall(r"[A-Za-z']+", s) or [''])[0].lower()
gate = [c for c in contain if first(c[3]) in FN]
res = [c for c in contain if first(c[3]) not in FN]
print('containment flags', len(contain), 'of which span-head gate words first:', len(gate), 'residue:', len(res))
print('residue first-word histogram:', collections.Counter(first(c[3]) for c in res).most_common(25))
random.seed(3)
for c in random.sample(res, 25): print('   ', c)
# minority vs gate
mg = [m for m in minority if first(m[1]) in FN]
print('minority flags', len(minority), 'gate-word-first', len(mg))
# duplicate pairs: depth mix and phonetic ids
dd = collections.Counter()
for a,b in pairs:
    for l in banked[a]:
        if l['d']>=5: dd[(l['d'], bool(re.match(r'^s\d+y\d+', l['id'])))]+=1
print('dup-pair first-member d>=5 spans by (depth, y-id):', dd)
texts = sorted({seg[a] for a,b in pairs}, key=lambda t: len(t[0]))
for t in texts[:6]+texts[-4:]: print('   DUPTEXT', t[0][:70], '|', t[1][:70])
# audit defects: do the (headword, ref) still exist with other eng?
v = json.load(open(f'{ROOT}/data/alignment/audit_verdicts_20260904.json'))
byref = collections.defaultdict(list)
for l in L: byref[(l['tib'], f"{l['course']}:{l['seg']}")].append(l['eng'])
for it in v['items']:
    if it['final']!='CONFIRMED_DEFECT': continue
    cur = byref.get((it['headword'], it['ref']))
    print('   DEF', it['headword'], '|', it['eng'], '|', it['ref'], '-> now', cur)
