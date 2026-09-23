exec(open('fin.py').read().split("for label,sets in")[0])
for label,sets in (("HEADWORDS",fam_h),("PAIRS",fam_p)):
    for ref_c,tgt_c in ((['C01'],['C02','C03','C04','C05']),(['C01','C02'],['C03','C04','C05']),(['C01','C02','C03'],['C04','C05'])):
        ref=set(fams_in(ref_c)); tg=[f for f in fams_in(tgt_c) if f not in ref]
        st=incidence_stats([sets[f] for f in ref])
        actual=len(set().union(*(sets[f] for f in tg))-set(st['inc']))
        t=len(tg)/len(ref)
        pc=chao_extrap(st,len(tg)); pb=sgt(st,t,'bin'); pp=sgt(st,t,'pois'); pl=chao_lin_new(st,len(ref)+len(tg))
        inc=[x for f in tg for x in sets[f]]; act_cov=sum(1 for x in inc if x in st['inc'])/len(inc)
        print('%-9s %s->%s t=%.2f actual=%5d chao=%.2fx sgt_bin=%.2fx sgt_pois=%.2fx chaoLin=%.2fx | cov pred(next)=%.3f pred(avg over target)~%.3f realised=%.3f'%(label,'+'.join(ref_c),'+'.join(tgt_c),t,actual,pc/actual,pb/actual,pp/actual,pl/actual,st['C'],(st['C']+chao_cov_extrap(st,len(tg)))/2,act_cov))
