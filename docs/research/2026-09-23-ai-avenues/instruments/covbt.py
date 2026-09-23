exec(open('cov.py').read().split("for label, sets in")[0])
for label,sets in (("HEADWORDS",fam_h),("PAIRS",fam_p)):
    print('##',label)
    for mode in ('forward','loco'):
        for i,tgt in enumerate(COURSES):
            if mode=='forward':
                if i==0: continue
                ref_c=COURSES[:i]
            else: ref_c=[c for c in COURSES if c!=tgt]
            ref=set(fams_in(ref_c)); tg=[f for f in fams_in([tgt]) if f not in ref]
            st=incidence_stats([sets[f] for f in ref]); seen=set(st['inc'])
            inc=[x for f in tg for x in sets[f]]
            act=sum(1 for x in inc if x in seen)/len(inc)
            # predicted coverage averaged over the target span (first unit C_ref -> after tstar units)
            tstar=len(tg)
            print('%-7s %s  predicted next-unit coverage=%.3f  realised share of target incidences already banked=%.3f   (pred after target=%.3f)'%(mode,tgt,st['C'],act,chao_cov_extrap(st,tstar)))
