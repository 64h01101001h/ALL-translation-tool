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
res=collections.Counter(); fails=[]
for s in seqs:
    A=load(f"{SC}/{s}-tibetan-first/spec.json"); B=load(f"{SC}/{s}-english-first/spec.json"); R=load(f"{SC}/{s}-reconciled.json")
    pool=collections.Counter(k3(x) for x in A.get("spans",[])) | collections.Counter(k3(x) for x in B.get("spans",[]))
    used=collections.Counter(); keep=[]; drop=0
    for x in R.get("spans",[]):
        k=k3(x)
        if pool[k]>used[k]: used[k]+=1; keep.append(x)
        else: drop+=1
    res["authored_removed"]+=drop
    ids={x["id"] for x in keep}
    eo=[i for i in (R.get("eng_order") or []) if i in ids]
    seg={"seq":int(s),"title":R.get("title") or "untitled","spans":keep}
    if eo: seg["eng_order"]=eo
    p=subprocess.run(["python3",f"{ROOT}/tools/gen_alignment_page.py"],
        input=json.dumps({"course":"C05","segments":[seg]}),capture_output=True,text=True,cwd=ROOT)
    if p.returncode==0: res["exit0"]+=1
    else:
        res["refused"]+=1; fails.append((s,((p.stderr or p.stdout).strip().splitlines() or ["?"])[-1][:170]))
print("ORACLE-ARBITER page: the reconciler's own final choices, MINUS the %d spans it authored"%res["authored_removed"])
print("(this is the best a perfect chooser-over-supplied-candidates could ever produce)")
print("  exit 0 :",res["exit0"],"of",len(seqs))
print("  REFUSED:",res["refused"],"(%.0f%%)"%(100*res["refused"]/len(seqs)))
for f in fails[:20]: print("  ",f)
