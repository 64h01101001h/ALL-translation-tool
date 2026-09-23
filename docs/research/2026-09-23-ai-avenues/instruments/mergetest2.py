import os, json, collections, subprocess
SC="/private/tmp/claude-501/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2/scratchpad/c05"
ROOT="/Users/adamderickandrade/ALL-translation-tool"
def load(p):
    o=json.load(open(p)); return o["segments"][0] if "segments" in o else o
seqs=[d.split("-")[0] for d in sorted(os.listdir(SC))
      if d.endswith("-tibetan-first") and os.path.exists(f"{SC}/{d}/spec.json")
      and os.path.exists(f"{SC}/{d.split('-')[0]}-english-first/spec.json")
      and os.path.exists(f"{SC}/{d.split('-')[0]}-reconciled.json")]
k3=lambda x:(x.get("tib"),x.get("eng"),x.get("d"))
res=collections.Counter(); fails=[]; kept_tot=0; recon_tot=0
for s in seqs:
    A=load(f"{SC}/{s}-tibetan-first/spec.json"); B=load(f"{SC}/{s}-english-first/spec.json"); R=load(f"{SC}/{s}-reconciled.json")
    b3=collections.Counter(k3(x) for x in B.get("spans",[])); used=collections.Counter(); keep=[]
    for x in A.get("spans",[]):
        k=k3(x)
        if b3[k]>used[k]: used[k]+=1; keep.append(x)
    # deterministic repair: drop a d=7 whose immediately-preceding kept span is not a d=5 containing it
    rep=[]
    for i,x in enumerate(keep):
        if x.get("d")==7:
            prev=None
            for y in reversed(rep):
                prev=y; break
            if not prev or prev.get("d") not in (5,7) or (x.get("tib") or "") not in (prev.get("tib") or ""):
                # try the nearest preceding d=5
                par=None
                for y in reversed(rep):
                    if y.get("d")==5: par=y; break
                if not par or (x.get("tib") or "") not in (par.get("tib") or ""):
                    res["orphans_dropped"]+=1; continue
        rep.append(x)
    ids={x["id"] for x in rep}
    eo=[i for i in (A.get("eng_order") or []) if i in ids]
    seg={"seq":int(s),"title":A.get("title") or "untitled","spans":rep}
    if eo: seg["eng_order"]=eo
    p=subprocess.run(["python3",f"{ROOT}/tools/gen_alignment_page.py"],
        input=json.dumps({"course":"C05","segments":[seg]}),capture_output=True,text=True,cwd=ROOT)
    if p.returncode==0: res["exit0"]+=1
    else:
        res["refused"]+=1; fails.append((s,((p.stderr or p.stdout).strip().splitlines() or ["?"])[-1][:150]))
    kept_tot+=len(rep); recon_tot+=len(R.get("spans",[]))
print("AGREEMENT-ONLY + deterministic orphan-repair:")
print("  exit 0 :",res["exit0"],"of",len(seqs))
print("  REFUSED:",res["refused"])
print("  orphan d=7 spans dropped by the repair:",res["orphans_dropped"])
print("  spans in the repaired agreement-only page: %d  vs %d in the reconciled pages (%.1f%%)"%(kept_tot,recon_tot,100*kept_tot/recon_tot))
for f in fails: print("  ",f)
