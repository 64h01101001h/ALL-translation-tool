import json, collections
R='/Users/adamderickandrade/ALL-translation-tool/'
L=json.load(open(R+'data/alignment/alignment_full_v1.json'))['links']
prior={c:set() for c in ['C01','C02','C03','C04']}
bank_pairs=set(); bank_tib=set()
for l in L:
    if l['course'] in ('C01','C02','C03','C04') and l['d'] in (5,7) and l.get('eng'):
        bank_pairs.add((l['tib'],l['eng'])); bank_tib.add(l['tib'])
c5=[l for l in L if l['course']=='C05' and l['d'] in (5,7) and l.get('eng')]
seen_pair=sum(1 for l in c5 if (l['tib'],l['eng']) in bank_pairs)
seen_tib=sum(1 for l in c5 if l['tib'] in bank_tib)
print('C05 d5/7 links with eng:',len(c5))
print('  (tib,eng) pair ALREADY banked from C01-C04:', seen_pair, f'{100*seen_pair/len(c5):.1f}%')
print('  tib headword already banked, DIFFERENT eng :', seen_tib-seen_pair, f'{100*(seen_tib-seen_pair)/len(c5):.1f}%')
print('  tib headword entirely new              :', len(c5)-seen_tib, f'{100*(len(c5)-seen_tib)/len(c5):.1f}%')
# growing-bank version: walk C05 in seq order, bank grows
byseq=collections.defaultdict(list)
for l in c5: byseq[l['seg']].append(l)
pairs=set(bank_pairs); tib=set(bank_tib)
rep=new_eng=new_tib=0
for s in sorted(byseq):
    for l in byseq[s]:
        k=(l['tib'],l['eng'])
        if k in pairs: rep+=1
        elif l['tib'] in tib: new_eng+=1
        else: new_tib+=1
        pairs.add(k); tib.add(l['tib'])
tot=rep+new_eng+new_tib
print('\nwith the bank GROWING through C05 in seq order:')
print(f'  repeat of an already-banked pair : {rep:>6} ({100*rep/tot:.1f}%)')
print(f'  known Tibetan, new English       : {new_eng:>6} ({100*new_eng/tot:.1f}%)')
print(f'  new Tibetan headword             : {new_tib:>6} ({100*new_tib/tot:.1f}%)')
# also nulls: how often is a given tib nulled in prior courses
nullbank=collections.Counter()
for l in L:
    if l['course']!='C05' and l['d'] in (5,6,7):
        nullbank[(l['tib'], l.get('eng') is None)]+=1
c5n=[l for l in L if l['course']=='C05' and l['d'] in (5,6,7)]
known_null=sum(1 for l in c5n if l.get('eng') is None and nullbank[(l['tib'],True)]>0)
alln=sum(1 for l in c5n if l.get('eng') is None)
print(f'\nC05 nulls whose Tibetan was nulled before in C01-C04: {known_null}/{alln} ({100*known_null/alln:.1f}%)')
