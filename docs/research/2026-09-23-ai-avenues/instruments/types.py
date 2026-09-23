import json,re,collections
exec(open('labelfree.py').read().split('# ---------- D.')[0].replace("print(","(lambda *a,**k:None)("))
FN={'the','a','an','and','or','his','our','your','i','you'}
first=lambda s:(re.findall(r"[A-Za-z']+",s) or [''])[0].lower()
sh=[l for l in L if l['d'] in (5,7) and l['eng'] and first(l['eng']) in FN]
print('approx span-head flags (d5/d7, FN-first):', len(sh), 'distinct (tib,eng) types:', len({(l['tib'],l['eng'].lower()) for l in sh}), 'distinct (first-word, tib) classes:', len({(first(l['eng']),l['tib']) for l in sh}))
print('containment flags', len(contain), 'types', len({(c[2],c[3].lower()) for c in contain}))
print('minority flags', len(minority), '(already type-level)')
u={(l['tib'],l['eng'].lower()) for l in sh}|{(c[2],c[3].lower()) for c in contain}|{(m[0],m[1]) for m in minority}
print('union of types', len(u))
# Pareto: tokens covered by top-k types for span-head
cnt=collections.Counter((l['tib'],l['eng'].lower()) for l in sh)
tot=sum(cnt.values()); acc=0
for k,(t,c) in enumerate(cnt.most_common(),1):
    acc+=c
    if k in (10,50,100,200,500): print(f'  top {k} span-head types cover {acc}/{tot} tokens ({acc/tot:.0%})')
cls=collections.Counter(first(l['eng']) for l in sh); print('  by first word', cls.most_common(10))
