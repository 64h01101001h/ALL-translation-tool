import os, json, collections, subprocess, tempfile
SC="/private/tmp/claude-501/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2/scratchpad/c05"
ROOT="/Users/adamderickandrade/ALL-translation-tool"
def load(p):
    o=json.load(open(p))
    if "segments" in o: return o["segments"][0]
    return o
seqs=[d.split("-")[0] for d in sorted(os.listdir(SC))
      if d.endswith("-tibetan-first") and os.path.exists(f"{SC}/{d}/spec.json")
      and os.path.exists(f"{SC}/{d.split('-')[0]}-english-first/spec.json")
      and os.path.exists(f"{SC}/{d.split('-')[0]}-reconciled.json")]
k3=lambda x:(x.get("tib"),x.get("eng"),x.get("d"))
res=collections.Counter(); fails=[]
for s in seqs:
    A=load(f"{SC}/{s}-tibetan-first/spec.json"); B=load(f"{SC}/{s}-english-first/spec.json"); R=load(f"{SC}/{s}-reconciled.json")
    b3=collections.Counter(k3(x) for x in B.get("spans",[]))
    keep=[]; used=collections.Counter()
    for x in A.get("spans",[]):
        k=k3(x)
        if b3[k]>used[k]:
            used[k]+=1; keep.append(x)
    ids={x["id"] for x in keep}
    eo=[i for i in (A.get("eng_order") or []) if i in ids]
    seg={"seq":int(s),"title":A.get("title") or R.get("title") or "untitled","spans":keep}
    if eo: seg["eng_order"]=eo
    spec={"course":"C05","segments":[seg]}
    p=subprocess.run(["python3",f"{ROOT}/tools/gen_alignment_page.py"],input=json.dumps(spec),
                     capture_output=True,text=True,cwd=ROOT)
    if p.returncode==0: res["exit0"]+=1
    else:
        res["refused"]+=1
        fails.append((s,(p.stderr or p.stdout).strip().splitlines()[-1][:160] if (p.stderr or p.stdout).strip() else "?"))
print("AGREEMENT-ONLY merge (the %d spans both angles asserted identically), run through gen_alignment_page.py:"%1838)
print("  exit 0 :",res["exit0"])
print("  REFUSED:",res["refused"],"of",len(seqs),"segments  (%.0f%%)"%(100*res["refused"]/len(seqs)))
print()
print("refusal reasons (first 25):")
for f in fails[:25]: print("  ",f)
reasons=collections.Counter(f[1].split(":")[0][:60] for f in fails)
print()
print("reason histogram:")
for r,c in reasons.most_common(12): print("  %3d  %s"%(c,r))
