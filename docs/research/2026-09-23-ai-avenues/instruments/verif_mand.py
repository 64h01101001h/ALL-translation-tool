exec(open('fin.py').read().split("for label,sets in")[0])
for lab,sets,m in (('HW',fam_h,3381),('PAIR',fam_p,4073)):
    st=incidence_stats(list(sets.values())); T=st['T']; t=m/T
    r=dict(chaolin=chao_lin_new(st,T+m), chao=chao_extrap(st,m), sgtp=sgt(st,t,'pois'), sgtb=sgt(st,t,'bin'))
    print(lab,'t=%.2f'%t, {k:round(v) for k,v in r.items()})
    corr={'chaolin':(0.86,0.87),'chao':(1.07,1.08),'sgtp':(1.09,1.11),'sgtb':(1.17,1.18)} if lab=='HW' else {'chaolin':(1.08,1.10),'chao':(1.22,1.25),'sgtp':(1.13,1.15),'sgtb':(1.22,1.24)}
    print('  corrected:',{k:(round(v/corr[k][1]),round(v/corr[k][0])) for k,v in r.items()})
