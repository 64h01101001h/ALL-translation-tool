exec(open('cov.py').read().split("for label, sets in")[0])
import random
st=incidence_stats(list(fam_h.values()))
inc=st['inc']
def nsyl(h): return len(h.split())
dist_all=collections.Counter(min(nsyl(h),6) for h in inc)
dist_q1=collections.Counter(min(nsyl(h),6) for h,c in inc.items() if c==1)
print('syllable-length dist, all headwords:',sorted(dist_all.items()))
print('syllable-length dist, singletons  :',sorted(dist_q1.items()))
# particles at end
PART={'gyi','kyi','gi','yi','kyis','gyis','gis','yis','la','su','ru','tu','du','na','nas','las','dang','ni','ste','de','te','cing','zhing','shing','pa','ba','po','bo','ma','mo','yang','kyang','the','dag','rnams'}
end_part=sum(1 for h,c in inc.items() if c==1 and h.split()[-1] in {'gyi','kyi','gi','yi','kyis','gyis','gis','yis','la','su','ru','tu','du','nas','cing','zhing','shing','ste','te','de'})
print('singletons ending in case/connective particle:',end_part,'of',st['Q1'])
random.seed(1)
q1=[h for h,c in inc.items() if c==1]
print('sample singletons:',random.sample(q1,40))
