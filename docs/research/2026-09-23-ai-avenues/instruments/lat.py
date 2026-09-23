import json, os, glob, re, statistics as st
BASE=os.path.expanduser("~/.claude/projects/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2")
prop=[]; rec=[]; runw=[]
for f in glob.glob(BASE+"/workflows/wf_*.json"):
    w=json.load(open(f))
    if w.get("workflowName")!="c05-align" or w.get("status")!="completed": continue
    st_,en=w.get("startedAt"),w.get("completedAt") or w.get("endedAt")
    durs=[]
    for p in (w.get("workflowProgress") or []):
        if p.get("type")!="workflow_agent": continue
        lab=p.get("label") or ""
        d=p.get("durationMs") or p.get("elapsedMs")
        if d is None and p.get("startedAt") and p.get("completedAt"):
            try: d=(p["completedAt"]-p["startedAt"])
            except Exception: d=None
        if d is None: continue
        m=d/60000.0
        (prop if lab.startswith("propose") else rec).append(m)
    if st_ and en:
        try: runw.append((en-st_)/60000.0)
        except Exception: pass
print("keys sample:", sorted(set(k for f in glob.glob(BASE+"/workflows/wf_*.json")[:1] for k in json.load(open(f)))) )
w=json.load(open(glob.glob(BASE+"/workflows/wf_*.json")[0]))
pg=(w.get("workflowProgress") or [])
print("progress entry keys:", sorted(pg[0].keys()) if pg else None)
if prop: print("propose  agent minutes: n=%d mean %.1f median %.1f max %.1f"%(len(prop),st.mean(prop),st.median(prop),max(prop)))
if rec:  print("reconcile agent minutes: n=%d mean %.1f median %.1f max %.1f"%(len(rec),st.mean(rec),st.median(rec),max(rec)))
if runw: print("run wall minutes (9 segs): n=%d mean %.1f median %.1f"%(len(runw),st.mean(runw),st.median(runw)))
