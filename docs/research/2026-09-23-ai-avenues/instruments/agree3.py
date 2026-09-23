import os, json, collections
SC="/private/tmp/claude-501/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2/scratchpad/c05"
def spans_of(p):
    o=json.load(open(p))
    if "segments" in o:
        return [x for s in o["segments"] for x in s.get("spans",[])]
    return o.get("spans",[])
seqs=[d.split("-")[0] for d in sorted(os.listdir(SC))
      if d.endswith("-tibetan-first") and os.path.exists(f"{SC}/{d}/spec.json")
      and os.path.exists(f"{SC}/{d.split('-')[0]}-english-first/spec.json")
      and os.path.exists(f"{SC}/{d.split('-')[0]}-reconciled.json")]
k3=lambda x:(x.get("tib"),x.get("eng"),x.get("d"))
k2=lambda x:(x.get("tib"),x.get("eng"))
C=collections.Counter(); ex=[]
for s in seqs:
    A=spans_of(f"{SC}/{s}-tibetan-first/spec.json"); B=spans_of(f"{SC}/{s}-english-first/spec.json"); R=spans_of(f"{SC}/{s}-reconciled.json")
    a3=collections.Counter(k3(x) for x in A); b3=collections.Counter(k3(x) for x in B); r3=collections.Counter(k3(x) for x in R)
    a2=collections.Counter(k2(x) for x in A); b2=collections.Counter(k2(x) for x in B); r2=collections.Counter(k2(x) for x in R)
    agreed3 = a3 & b3            # multiset of spans both angles asserted identically
    for k,c in agreed3.items():
        kept3 = min(c, r3[k])
        C["agreed"]+=c
        C["agreed_kept_exact"]+=kept3
        lost = c-kept3
        if lost:
            k2k=(k[0],k[1])
            kept2 = min(c, r2[k2k])
            if kept2>kept3:
                C["agreed_kept_depth_changed"]+=min(lost, kept2-kept3)
                lost -= min(lost, kept2-kept3)
            if lost:
                # does the tib survive at all?
                rt=collections.Counter(x.get("tib") for x in R)
                if rt[k[0]]>0:
                    C["agreed_eng_overturned"]+=lost
                    if len(ex)<20: ex.append((s,"ENG-OVERTURNED",k, [x.get("eng") for x in R if x.get("tib")==k[0]]))
                else:
                    C["agreed_span_dropped"]+=lost
                    if len(ex)<20: ex.append((s,"DROPPED",k,None))
print("Spans BOTH angles asserted identically (tib,eng,d): %d over %d segments"%(C["agreed"],len(seqs)))
for lab,k in [("kept exactly by the reconciler","agreed_kept_exact"),
              ("kept, but depth changed","agreed_kept_depth_changed"),
              ("English overturned (tib survives, eng differs)","agreed_eng_overturned"),
              ("dropped entirely (tib gone from the page)","agreed_span_dropped")]:
    print("  %-46s %5d  %.2f%%"%(lab,C[k],100*C[k]/C["agreed"]))
overturn=C["agreed_kept_depth_changed"]+C["agreed_eng_overturned"]+C["agreed_span_dropped"]
print("  TOTAL agreements the reconciler did NOT rubber-stamp: %d  %.2f%%"%(overturn,100*overturn/C["agreed"]))
print()
for e in ex[:20]: print("  ",e)
