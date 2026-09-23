exec(open('fin.py').read().split("for label,sets in")[0])
st=incidence_stats(list(fam_h.values()))
m=3381; t=m/st['T']
print('HEADWORDS at +%d Tibetan-unique families t=%.2f: chao_extrap=%.0f sgt_bin=%.0f sgt_pois=%.0f chao_lin_finite=%.0f cov_after=%.3f'%(m,t,chao_extrap(st,m),sgt(st,t,'bin'),sgt(st,t,'pois'),chao_lin_new(st,st['T']+m),chao_cov_extrap(st,m)))
