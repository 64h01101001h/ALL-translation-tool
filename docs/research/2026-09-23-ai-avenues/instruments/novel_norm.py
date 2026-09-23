import json, re, collections
R='/Users/adamderickandrade/ALL-translation-tool/'
L=json.load(open(R+'data/alignment/alignment_full_v1.json'))['links']
def norm(e):
    e=e.lower().strip(); e=re.sub(r"^(the|a|an|to|of|in)\s+","",e); e=re.sub(r"[^a-z' ]","",e); e=re.sub(r"(ing|ed|es|s)\b","",e); return e.strip()
order=['C01','C02','C03','C04','C05']
for test in ['C03','C04','C05']:
    prior=order[:order.index(test)]
    bank=collections.defaultdict(set); bankn=collections.defaultdict(set)
    for l in L:
        if l['course'] in prior and l['d']==5 and l['eng']:
            bank[l['tib'].strip()].add(l['eng'].strip()); bankn[l['tib'].strip()].add(norm(l['eng']))
    k=ex=nm=0
    for l in L:
        if l['course']==test and l['d']==5 and l['eng'] and l['tib'].strip() in bank:
            k+=1; t=l['tib'].strip()
            if l['eng'].strip() in bank[t]: ex+=1
            elif norm(l['eng']) in bankn[t]: nm+=1
    print(test,'known-headword d5 renderings',k,'exact repeat',f'{ex/k:.1%}','repeat only after normalising',f'{nm/k:.1%}','genuinely new',f'{(k-ex-nm)/k:.1%}')
