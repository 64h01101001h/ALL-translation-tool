import os, json, collections
SC="/private/tmp/claude-501/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2/scratchpad/c05"
def spans_of(p):
    o=json.load(open(p))
    if "segments" in o:
        out=[]
        for s in o["segments"]: out.extend(s.get("spans",[]))
        return out
    return o.get("spans",[])
seqs=[d.split("-")[0] for d in sorted(os.listdir(SC))
      if d.endswith("-tibetan-first") and os.path.exists(f"{SC}/{d}/spec.json")
      and os.path.exists(f"{SC}/{d.split('-')[0]}-english-first/spec.json")
      and os.path.exists(f"{SC}/{d.split('-')[0]}-reconciled.json")]

C=collections.Counter()
ex_modified=[]; ex_new=[]; ex_dropped=[]; ex_engdiff=[]; ex_ddiff=[]
for s in seqs:
    A=spans_of(f"{SC}/{s}-tibetan-first/spec.json"); B=spans_of(f"{SC}/{s}-english-first/spec.json"); R=spans_of(f"{SC}/{s}-reconciled.json")
    k3=lambda x:(x.get("tib"),x.get("eng"),x.get("d"))
    a3=collections.Counter(k3(x) for x in A); b3=collections.Counter(k3(x) for x in B)
    atib=collections.Counter(x.get("tib") for x in A); btib=collections.Counter(x.get("tib") for x in B)
    r3=collections.Counter(k3(x) for x in R); rtib=collections.Counter(x.get("tib") for x in R)
    # reconciled classification
    seen=collections.Counter()
    for x in R:
        k=k3(x); i=seen[k]; seen[k]+=1
        ina=a3[k]>i; inb=b3[k]>i
        if ina and inb: C["confirm"]+=1
        elif ina or inb:
            C["pick_one"]+=1
            # was the tib contested? i.e. other angle had the tib with different eng/d
            other = btib if ina else atib
            if other[x.get("tib")]>0: C["pick_one_contested"]+=1
            else: C["pick_one_uncontested"]+=1
        else:
            if atib[x.get("tib")]>0 or btib[x.get("tib")]>0:
                C["modified"]+=1
                if len(ex_modified)<12: ex_modified.append((s,x.get("tib"),x.get("eng"),x.get("d")))
            else:
                C["new_tib"]+=1
                if len(ex_new)<12: ex_new.append((s,x.get("tib"),x.get("eng"),x.get("d")))
    # dropped: proposed tib strings absent from reconciled
    for t,c in (atib|btib).items():
        if rtib[t]==0:
            C["dropped_tib"]+=1
            if len(ex_dropped)<15: ex_dropped.append((s,t))
    # contested tib: present in both, eng differs / d differs
    for t in set(atib)&set(btib):
        ae={x.get("eng") for x in A if x.get("tib")==t}; be={x.get("eng") for x in B if x.get("tib")==t}
        ad={x.get("d") for x in A if x.get("tib")==t}; bd={x.get("d") for x in B if x.get("tib")==t}
        C["shared_tib"]+=1
        if ae!=be:
            C["shared_tib_eng_differs"]+=1
            if len(ex_engdiff)<12: ex_engdiff.append((s,t,sorted(map(str,ae)),sorted(map(str,be))))
        if ad!=bd:
            C["shared_tib_d_differs"]+=1
            if len(ex_ddiff)<10: ex_ddiff.append((s,t,sorted(ad),sorted(bd)))
    # null disagreement: one angle banks eng, other nulls same tib
    for t in set(atib)&set(btib):
        an=any(x.get("eng") is None for x in A if x.get("tib")==t); bn=any(x.get("eng") is None for x in B if x.get("tib")==t)
        if an!=bn: C["null_vs_bank"]+=1
    C["tibA_only"]+=len(set(atib)-set(btib)); C["tibB_only"]+=len(set(btib)-set(atib))
tot=C["confirm"]+C["pick_one"]+C["modified"]+C["new_tib"]
print("reconciled spans classified:",tot)
for k in ["confirm","pick_one","pick_one_contested","pick_one_uncontested","modified","new_tib"]:
    print("  %-22s %5d  %.1f%%"%(k,C[k],100*C[k]/tot))
print()
print("proposal-level disagreement:")
print("  shared tib strings            ",C["shared_tib"])
print("   ...eng set differs           ",C["shared_tib_eng_differs"], "(%.1f%% of shared)"%(100*C["shared_tib_eng_differs"]/C["shared_tib"]))
print("   ...depth set differs         ",C["shared_tib_d_differs"], "(%.1f%%)"%(100*C["shared_tib_d_differs"]/C["shared_tib"]))
print("   ...one nulls, other banks    ",C["null_vs_bank"])
print("  tib only in tibetan-first     ",C["tibA_only"])
print("  tib only in english-first     ",C["tibB_only"])
print("  proposed tib dropped entirely ",C["dropped_tib"])
print()
print("EXAMPLES modified (tib in a proposal, eng/d changed by reconciler):")
for e in ex_modified: print("  ",e)
print("EXAMPLES new tib (neither angle proposed this Tibetan):")
for e in ex_new: print("  ",e)
print("EXAMPLES eng disagreement on shared tib:")
for e in ex_engdiff: print("  ",e)
print("EXAMPLES depth disagreement:")
for e in ex_ddiff: print("  ",e)
print("EXAMPLES dropped:")
for e in ex_dropped: print("  ",e)
