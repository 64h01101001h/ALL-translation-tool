import json,os,collections,glob
A="/Users/adamderickandrade/Documents/ChatGPT/Geshe Michael Roach Tib _ Eng Alignment/campaign-artifacts"
D=json.load(open('banks_overlap.json'))
def flat(spans):
    for s in spans:
        if isinstance(s,dict) and 'd' in s and 'tib' in s:
            yield s
            for k in ('members','children','parts'):
                if isinstance(s.get(k),list): yield from flat(s[k])
def prop(seq,ang):
    b=(seq-166)//3*3+166
    p=os.path.join(A,f'C05-{b}-{b+2}',ang,str(seq),'spec.json')
    if not os.path.exists(p): return None
    d=json.load(open(p))
    out=collections.Counter()
    for sg in d['segments']:
        for s in flat(sg['spans']):
            if s.get('d')==5 and s.get('eng'):
                out[(sg['seq'],' '.join(s['tib'].split()),' '.join(s['eng'].split()))]+=1
    return out
def bank(which,seq):
    pg=(seq-166)//3+56
    return collections.Counter((x['seg'],x['tib'],x['eng']) for x in D[which][str(pg)] if x['d']==5 and x['eng'] and x['seg']==seq)
tot=collections.Counter()
for seq in range(196,367):
    T=prop(seq,'tibetan'); E=prop(seq,'english')
    if T is None or E is None: tot['missing']+=1; continue
    Fx=bank('codex',seq); Fc=bank('claude',seq)
    U=T&E; S=(T|E)-U   # single-angle
    tot['segs']+=1
    tot['T']+=sum(T.values()); tot['E']+=sum(E.values()); tot['U']+=sum(U.values()); tot['single']+=sum(S.values())
    tot['U_kept_codex']+=sum((U&Fx).values()); tot['U_dropped_codex']+=sum((U-Fx).values())
    tot['U_dropped_codex_in_claude']+=sum(((U-Fx)&Fc).values())
    tot['U_in_claude']+=sum((U&Fc).values()); tot['single_in_claude']+=sum((S&Fc).values())
    tot['single_kept_codex']+=sum((S&Fx).values())
    tot['Fx']+=sum(Fx.values()); tot['Fx_from_U']+=sum((Fx&U).values()); tot['Fx_from_single']+=sum((Fx&S).values())
    tot['Fx_new']+=sum((Fx-(T|E)).values())
    tot['Fc']+=sum(Fc.values())
    tot['Fx_and_Fc']+=sum((Fx&Fc).values())
    tot['UandFc_kept_codex']+=sum((U&Fc&Fx).values())
    tot['UandFc_dropped_codex']+=sum(((U&Fc)-Fx).values())
print(dict(tot))
t=tot
print('same-family unanimity (Codex T∩E) dropped by Codex final: %d/%d = %.2f%%'%(t['U_dropped_codex'],t['U'],100*t['U_dropped_codex']/t['U']))
print('  of those, present in Claude final: %d'%t['U_dropped_codex_in_claude'])
print('Codex-unanimous spans found in Claude final: %.1f%%; Codex single-angle spans in Claude final: %.1f%%'%(100*t['U_in_claude']/t['U'],100*t['single_in_claude']/t['single']))
print('single-angle kept by codex final: %.1f%%'%(100*t['single_kept_codex']/t['single']))
print('cross-family unanimous (U∩Fc) dropped by Codex final: %d/%d = %.2f%%'%(t['UandFc_dropped_codex'],t['UandFc_dropped_codex']+t['UandFc_kept_codex'],100*t['UandFc_dropped_codex']/max(1,t['UandFc_dropped_codex']+t['UandFc_kept_codex'])))

print('\nDROPPED same-family-unanimous (Codex), and whether Claude kept them:')
for seq in range(196,367):
    T=prop(seq,'tibetan'); E=prop(seq,'english')
    if T is None or E is None: continue
    Fx=bank('codex',seq); Fc=bank('claude',seq)
    U=T&E
    for k in (U-Fx):
        # did codex keep the tib with a different English?
        alt=[e for (s,t,e) in Fx if s==k[0] and t==k[1]]
        print(k, 'CLAUDE_KEPT' if k in Fc else 'claude_absent', 'codex_alt=',alt[:2])
