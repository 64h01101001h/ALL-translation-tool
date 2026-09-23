exec(open('cov.py').read().split("for label, sets in")[0])
st=incidence_stats(list(fam_h.values()))
T,Q1,Q0=st['T'],st['Q1'],st['Q0']; a=Q1/(T*Q0+Q1)
for ts in (0,1000,2000,2500,3000,3381):
    m=Q0*a*(1-a)**ts; cum=chao_extrap(st,ts)
    print('+%4d fams: cum new HW %5.0f (corr /1.07: %5.0f)  marginal %.2f/fam (corr %.2f)  $/new HW @6.69: %.2f  @3.00: %.2f  cov=%.3f'%(ts,cum,cum/1.07,m,m/1.07,6.69/(m/1.07),3/(m/1.07),chao_cov_extrap(st,ts)))
