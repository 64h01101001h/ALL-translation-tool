from math import sqrt
z=1.959963985

def wilson(k,n):
    if n==0: return (0,1)
    p=k/n; d=1+z*z/n
    c=(p+z*z/(2*n))/d
    h=z*sqrt(p*(1-p)/n+z*z/(4*n*n))/d
    return (c-h,c+h)

print("=== Q1: n needed so that a PERFECT run (0 errors) proves true acc >= T ===")
for T in (0.98,0.95,0.92,0.90,0.75):
    n=1
    while True:
        lo,hi=wilson(n,n)
        if lo>=T: break
        n+=1
        if n>200000: break
    print(f"  observed 100% correct, Wilson95 lower >= {T}: n = {n}")

print()
print("=== Q2: n needed so Wilson lower >= T when the OBSERVED rate is T+margin ===")
for T,obs in ((0.98,0.99),(0.98,0.995),(0.92,0.95),(0.92,0.96),(0.75,0.82),(0.75,0.85)):
    n=10
    while n<=300000:
        k=round(obs*n)
        lo,hi=wilson(k,n)
        if lo>=T: break
        n+=1
    print(f"  threshold {T}, observed {obs}: n = {n} (k={round(obs*n)}, CI lower {wilson(round(obs*n),n)[0]:.4f})")

print()
print("=== Q3: CI half-width achievable at given n, observed 0.98 ===")
for n in (50,100,200,300,500,1000,2000,5000):
    k=round(0.98*n); lo,hi=wilson(k,n)
    print(f"  n={n:5d}  Wilson95 = [{lo:.4f}, {hi:.4f}]  width {hi-lo:.4f}")

print()
print("=== Q4: the existing file, reproduced ===")
lo,hi=wilson(19,100); print(f"  19/100 majority rule: [{lo:.5f}, {hi:.5f}]  (file says 0.12514653992704258, 0.2777902407080786)")
lo,hi=wilson(11,100); print(f"  11/100 unanimous:     [{lo:.5f}, {hi:.5f}]  (file says 0.06254131955225124, 0.18631463027903025)")

print()
print("=== Q5: inter-model agreement (Opus first read vs Fable second read), n=100 ===")
a,b,c,dd=16,3,3,78   # both-def, opus-def/fable-sound, opus-sound/fable-def, both-sound
n=a+b+c+dd
po=(a+dd)/n
pe=((a+b)/n)*((a+c)/n)+((c+dd)/n)*((b+dd)/n)
print(f"  observed agreement {po:.3f}; expected {pe:.4f}; Cohen kappa = {(po-pe)/(1-pe):.3f}")
