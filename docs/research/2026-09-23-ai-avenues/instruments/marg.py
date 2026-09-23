exec(open('cov.py').read().split("for label, sets in")[0])
for label,sets in (("HEADWORDS",fam_h),("PAIRS",fam_p)):
    st=incidence_stats(list(sets.values()))
    T,Q1,Q0=st['T'],st['Q1'],st['Q0']
    a=Q1/(T*Q0+Q1)
    print('##',label,'mean incidences/family=%.1f'%(st['U']/T))
    for ts in (0,500,1000,2000,3000,4073):
        m=Q0*a*(1-a)**ts
        cum=chao_extrap(st,ts)
        print('  after +%4d families: cumulative new=%6.0f  marginal new per family=%.2f  -> $/new at $6.69/seg: %.2f ; at $3.00: %.2f ; at $1.00: %.2f   (deflated /1.06-1.36: $%.2f-%.2f at $6.69)'%(ts,cum,m,6.69/m,3/m,1/m,6.69/(m/1.06),6.69/(m/1.36)))
