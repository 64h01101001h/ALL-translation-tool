from math import comb, sqrt
def binom_cdf(c,n,p): return sum(comb(n,k)*p**k*(1-p)**(n-k) for k in range(c+1))
def lqas(p0,p1,alpha,beta,nmax=600):
    for n in range(1,nmax):
        for c in range(0,n):
            if 1-binom_cdf(c,n,p0)<=alpha and binom_cdf(c,n,p1)<=beta:
                return n,c
    return None
for p0,p1 in [(0.05,0.15),(0.05,0.10),(0.02,0.08),(0.10,0.20)]:
    print('LQAS p0',p0,'p1',p1,'a=b=0.10', lqas(p0,p1,0.10,0.10), 'a=b=0.05', lqas(p0,p1,0.05,0.05))
# Wald SPRT expected sample number approx at p0 and p1
from math import log
def sprt_asn(p0,p1,a,b):
    A=log((1-b)/a); B=log(b/(1-a))
    g1=log(p1/p0); g0=log((1-p1)/(1-p0))
    def asn(p):
        mu=p*g1+(1-p)*g0
        # OC approx
        if p==p0: L=1-a
        else: L=b
        return (L*B+(1-L)*A)/mu
    return asn(p0),asn(p1)
print('SPRT ASN 0.05 vs 0.15 a=b=.10', sprt_asn(0.05,0.15,0.1,0.1))
# rule of three
for n in [100,300,600]: print('0 defects in',n,'-> 95% upper', 3/n)
# SRS sample sizes
z=1.96
for p,E in [(0.19,0.05),(0.10,0.03),(0.05,0.02),(0.04,0.02)]:
    n=z*z*p*(1-p)/E/E; print('SRS p',p,'E',E,'n',round(n))
# DEFF with m spans per segment
for icc in [0.01,0.05,0.10,0.14]:
    for m in [5,16.4]:
        print('ICC',icc,'m',m,'DEFF',round(1+(m-1)*icc,2))
# PPI variance factor with free detector as predictor: prec 1.0 recall 0.58 base 0.19 (Claude-labelled)
p=0.19; r=0.58
TP=r*p; FN=p-TP; FP=0; TN=1-p
phi=(TP*TN-FP*FN)/sqrt((TP+FP)*(FN+TN)*(TP+FN)*(FP+TN))
print('phi',phi,'var factor 1-phi^2',1-phi**2, 'effective multiplier', 1/(1-phi**2))
