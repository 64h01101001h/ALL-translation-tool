import math
def lbinom_pmf(x,k,p):
    if p<=0: return 0.0 if x>0 else 1.0
    if p>=1: return 1.0 if x==k else 0.0
    return math.exp(math.lgamma(k+1)-math.lgamma(x+1)-math.lgamma(k-x+1)+x*math.log(p)+(k-x)*math.log1p(-p))
def cdf(x,k,p): return sum(lbinom_pmf(i,k,p) for i in range(x+1))
def cp_upper(x,k,delta):
    if x>=k: return 1.0
    lo,hi=x/k,1.0
    for _ in range(60):
        mid=(lo+hi)/2
        if cdf(x,k,mid)>delta: lo=mid
        else: hi=mid
    return hi
def max_x_certifiable(k,alpha,delta):
    # largest x such that cp_upper(x,k,delta)<=alpha
    x=-1
    while x+1<k and cp_upper(x+1,k,delta)<=alpha: x+=1
    return x
def power(k,e,alpha,delta):
    xm=max_x_certifiable(k,alpha,delta)
    return cdf(xm,k,e) if xm>=0 else 0.0
res=[]
for alpha in (0.02,0.05):
  for delta in (0.05,0.10):
    for e in (0.0,0.005,0.01,0.015):
      k=None
      step=10
      for kk in range(20,30001,step):
        if power(kk,e,alpha,delta)>=0.8: k=kk;break
      print(f"alpha={alpha:.2f} delta={delta:.2f} true_err={e:.3f}: labelled spans needed in accepted region (80% power) = {k}")
