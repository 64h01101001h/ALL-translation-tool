import json,collections
D=json.load(open('banks_overlap.json'))
A=D['claude'];B=D['codex']
def ms(L,f): return collections.Counter(f(x) for x in L)
tot=collections.Counter()
for n in A:
    a=[x for x in A[n] if x['d']==5]; b=[x for x in B[n] if x['d']==5]
    ae=ms([x for x in a if x['eng']],lambda x:(x['seg'],x['tib'],x['eng']))
    be=ms([x for x in b if x['eng']],lambda x:(x['seg'],x['tib'],x['eng']))
    tot['claude_pairs']+=sum(ae.values()); tot['codex_pairs']+=sum(be.values()); tot['exact_both']+=sum((ae&be).values())
    at=ms(a,lambda x:(x['seg'],x['tib'])); bt=ms(b,lambda x:(x['seg'],x['tib']))
    tot['claude_tib']+=sum(at.values()); tot['codex_tib']+=sum(bt.values()); tot['tib_both']+=sum((at&bt).values())
    # among tib both with eng in both: eng equal?
    ad=collections.defaultdict(list); bd=collections.defaultdict(list)
    for x in a: ad[(x['seg'],x['tib'])].append(x['eng'])
    for x in b: bd[(x['seg'],x['tib'])].append(x['eng'])
    for k in set(ad)&set(bd):
        ea=set(ad[k]); eb=set(bd[k])
        if None in ea and None in eb: tot['tibboth_null_both']+=1
        elif None in ea and not (None in eb): tot['tibboth_claude_null_codex_eng']+=1
        elif None in eb and not (None in ea): tot['tibboth_codex_null_claude_eng']+=1
        elif ea&eb: tot['tibboth_eng_same']+=1
        else:
            # containment?
            if any(x in y or y in x for x in ea for y in eb): tot['tibboth_eng_overlap']+=1
            else: tot['tibboth_eng_diff']+=1
print(dict(tot))
print('exact / claude pairs = %.3f; exact / codex pairs = %.3f'%(tot['exact_both']/tot['claude_pairs'], tot['exact_both']/tot['codex_pairs']))
print('tib seg agreement: both/claude %.3f both/codex %.3f'%(tot['tib_both']/tot['claude_tib'], tot['tib_both']/tot['codex_tib']))
