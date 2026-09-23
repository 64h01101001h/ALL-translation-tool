import json, subprocess, sys, hashlib, io, os
ROOT="/Users/adamderickandrade/ALL-translation-tool"
M=json.load(open(sys.argv[1]))
only=sys.argv[2:] or None
def sha(p): return hashlib.sha256(open(p,'rb').read()).hexdigest()
for mid,f,old,new,exp in M:
    if only and mid not in only: continue
    path=os.path.join(ROOT,f); before=sha(path); src=io.open(path,encoding='utf-8').read()
    assert src.count(old)==1, (mid, src.count(old))
    print("=== %s  (%s)" % (mid, f)); sys.stdout.flush()
    # 1. the gate's own words, verbatim
    try:
        io.open(path,'w',encoding='utf-8').write(src.replace(old,new,1))
        r=subprocess.run([sys.executable,"tools/test_span_id_compiler.py"],capture_output=True,text=True,cwd=ROOT)
        out=r.stdout+r.stderr
        lines=out.splitlines()
        i=next((k for k,l in enumerate(lines) if l.startswith("FAILED")),None)
        print("  gate exit %d; its FAILED section:" % r.returncode)
        for l in (lines[i:i+6] if i is not None else lines[-6:]): print("   |", l[:230])
    finally:
        io.open(path,'w',encoding='utf-8').write(src)
    assert sha(path)==before, "RESTORE FAILED"
    # 2. the harness's verdict (plants again, builds, ctest, restores, re-proves green)
    r=subprocess.run([sys.executable,"tools/mutate.py","--file",f,"--old",old,"--new",new,"--test","^span_id_compiler$","--expect-fail-contains",exp],capture_output=True,text=True,cwd=ROOT)
    for l in r.stdout.splitlines():
        if "MUTANT" in l or "restored" in l or "battery went red" in l or "SURVIVED" in l or "ANCHOR" in l: print("  harness:", l.strip())
    print("  harness exit %d; file sha256 restored: %s" % (r.returncode, sha(path)==before)); sys.stdout.flush()
