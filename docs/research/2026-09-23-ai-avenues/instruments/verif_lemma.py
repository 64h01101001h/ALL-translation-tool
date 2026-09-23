exec(open('cov.py').read().split("for label, sets in")[0])
PART={'gyi','kyi','gi','yi','kyis','gyis','gis','yis','la','su','ru','tu','du','nas','cing','zhing','shing','ste','te','de','ni','kyang','yang',"'am",'dang','dag','rnams','no','do'}
def fold(h):
    w=h.split()
    while len(w)>1 and w[-1] in PART: w=w[:-1]
    if w:
        for suf in ("'i","'is","'o","'am","'ang"):
            if w[-1].endswith(suf) and len(w[-1])>len(suf): w[-1]=w[-1][:-len(suf)]; break
    return ' '.join(w)
fam_f={f:{fold(h) for h in s} for f,s in fam_h.items()}
for lab,sets in (('surface',fam_h),('folded',fam_f)):
    st=incidence_stats(list(sets.values()))
    print(lab,'S=%d Q1=%d Q2=%d Chao2=%.0f C=%.3f'%(st['S'],st['Q1'],st['Q2'],st['chao2'],st['C']),
          ' +3381 chao_extrap=%.0f'%chao_extrap(st,3381))
# single-syllable and 2-syllable only (lexical core)
core={f:{h for h in s if len(h.split())<=2} for f,s in fam_f.items()}
st=incidence_stats(list(core.values()))
print('folded<=2syl S=%d Q1=%d Q2=%d Chao2=%.0f C=%.3f +3381=%.0f'%(st['S'],st['Q1'],st['Q2'],st['chao2'],st['C'],chao_extrap(st,3381)))
