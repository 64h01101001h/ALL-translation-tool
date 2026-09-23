exec(open('cov.py').read().split("for label, sets in")[0])
def chao_lin_new(st, N):
    T,Q1,Q2=st['T'],st['Q1'],st['Q2']; q=T/N
    return Q1*Q1/(2*Q2*T/(T-1) + q/(1-q)*Q1)
for label,sets in (("HEADWORDS",fam_h),("PAIRS",fam_p)):
    print('##',label)
    for mode in ('forward','loco'):
        for i,tgt in enumerate(COURSES):
            if mode=='forward':
                if i==0: continue
                ref_c=COURSES[:i]
            else: ref_c=[c for c in COURSES if c!=tgt]
            ref=set(fams_in(ref_c)); tg=[f for f in fams_in([tgt]) if f not in ref]
            st=incidence_stats([sets[f] for f in ref])
            actual=len(set().union(*(sets[f] for f in tg))-set(st['inc']))
            p=chao_lin_new(st,len(ref)+len(tg))
            print(mode,tgt,'actual',actual,'chao-lin finite-pop Q0=%.0f (%.2fx)'%(p,p/actual))
    st=incidence_stats(list(sets.values()))
    print('MANDATE finite population N=1552+4073: new = %.0f'%chao_lin_new(st,1552+4073))
# yield table in reading order
print('\ncourse fams newHW newHW/fam newPairs newPairs/fam cumHW Good_inc_cov_after')
seenh=set();seenp=set();done=[]
for c in COURSES:
    fs=[f for f in fams_in([c]) if f not in set(done)]
    nh=set().union(*(fam_h[f] for f in fs))-seenh; npair=set().union(*(fam_p[f] for f in fs))-seenp
    seenh|=nh; seenp|=npair; done+=fs
    st=incidence_stats([fam_h[f] for f in done])
    print(c,len(fs),len(nh),'%.2f'%(len(nh)/len(fs)),len(npair),'%.2f'%(len(npair)/len(fs)),len(seenh),'%.3f'%st['C'])
