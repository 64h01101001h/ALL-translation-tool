from math import sqrt
z=1.959963985
def wilson(k,n):
    p=k/n; d=1+z*z/n; c=(p+z*z/(2*n))/d
    h=z*sqrt(p*(1-p)/n+z*z/(4*n*n))/d
    return (c-h,c+h)

print("=== MEASURED ACCURACY OF THE CURRENT PIPELINE (per headword-occurrence) ===")
for k,n,lab in ((81,100,'majority rule (19 defects)'),(89,100,'unanimous rule (11 defects)')):
    lo,hi=wilson(k,n); print(f"  {lab}: point {k/n:.3f}, Wilson95 [{lo:.4f}, {hi:.4f}]")

print()
print("=== FEASIBILITY OF ADAM'S 4-TIER LADDER AT A POPULATION MEAN OF 0.81 ===")
print("  If the top bin covers fraction f at exactly .98, the remainder must average:")
for f in (0.2,0.3,0.4,0.5,0.6,0.602):
    rem=(0.81-0.98*f)/(1-f)
    print(f"    f={f:.3f} -> remainder mean {rem:.3f}" + ("   <-- below .75 floor" if rem<0.75 else ""))
print("  Two top bins (.98 at f1, .92 at f2); remainder must average:")
for f1,f2 in ((0.3,0.3),(0.4,0.3),(0.4,0.2),(0.5,0.2),(0.2,0.2)):
    rem=(0.81-0.98*f1-0.92*f2)/(1-f1-f2)
    print(f"    f1={f1}, f2={f2} -> remainder {rem:.3f}" + ("   <-- below .75" if rem<0.75 else ""))
print("  Same, taking the OPTIMISTIC unanimous-rule mean 0.89:")
for f1,f2 in ((0.3,0.3),(0.4,0.3),(0.5,0.2)):
    rem=(0.89-0.98*f1-0.92*f2)/(1-f1-f2)
    print(f"    f1={f1}, f2={f2} -> remainder {rem:.3f}" + ("   <-- below .75" if rem<0.75 else ""))

print()
print("=== AUC PRECISION (Hanley-McNeil) at 19% positive base rate ===")
def hm(auc,n1,n0):
    q1=auc/(2-auc); q2=2*auc*auc/(1+auc)
    return sqrt((auc*(1-auc)+(n1-1)*(q1-auc**2)+(n0-1)*(q2-auc**2))/(n1*n0))
for n in (300,600,1000,1500,2000):
    n1=round(0.19*n); n0=n-n1
    for auc in (0.75,0.85):
        se=hm(auc,n1,n0)
        print(f"  n={n:5d} (pos {n1:4d})  AUC {auc}: SE {se:.4f}, 95% CI +/-{1.96*se:.4f}")

print()
print("=== EXPERT HOURS ===")
for n in (400,600,1000,1200):
    for mins in (5,8):
        h=n*mins/60
        print(f"  n={n:5d} at {mins} min/item: {h:6.1f} h single-pass; +20% double-labelled: {h*1.2:6.1f} h; full double: {h*2:6.1f} h")
