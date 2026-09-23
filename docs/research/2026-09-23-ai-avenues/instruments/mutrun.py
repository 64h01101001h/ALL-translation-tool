import sys, os, subprocess, hashlib, json, re
SB = os.path.join(os.path.dirname(os.path.abspath(__file__)), "sbx")
def sha(p): return hashlib.sha256(open(p,'rb').read()).hexdigest()
BATTERY_ONLY = r'''
import sys; sys.path.insert(0,"tools")
import span_id_compiler as S
res=S.battery(); bad=S.judge(res,S.load_baseline())
print("segments_ok",res["segments_ok"],"sides",json.dumps(res["sides"]) if False else res["sides"])
print("BATTERY_BAD",len(bad))
for b in bad[:6]: print("  ",b[:230])
sys.exit(1 if bad else 0)
'''
def run(name, rel, old, new, battery=True):
    p = os.path.join(SB, rel)
    src = open(p, encoding="utf-8").read()
    h0 = sha(p)
    n = src.count(old)
    if n != 1:
        print("### %s: old string occurs %d times -- NOT RUN" % (name, n)); return
    open(p, "w", encoding="utf-8").write(src.replace(old, new, 1))
    try:
        g = subprocess.run([sys.executable, "tools/test_span_id_compiler.py"], cwd=SB, capture_output=True, text=True, timeout=900)
        out = g.stdout + g.stderr
        fl = [l for l in out.splitlines() if l.startswith("  ") and ("FAILED" in l or "expected" in l or "NEW FAILURE" in l or "REGRESSED" in l or "CLASS CHANGED" in l or "NO LONGER" in l or "CENSUS" in l or "BLIND" in l or "TOKENIZER" in l or "NOTHING" in l or "refused" in l or "tokenize" in l)]
        sect = out[out.find("\nFAILED"):] if "\nFAILED" in out else ""
        print("### %s\n  GATE exit=%d" % (name, g.returncode))
        for l in sect.strip().splitlines()[:10]: print("   | " + l[:240])
        if battery:
            b = subprocess.run([sys.executable, "-c", BATTERY_ONLY], cwd=SB, capture_output=True, text=True, timeout=900)
            print("  BATTERY-ONLY exit=%d" % b.returncode)
            for l in (b.stdout+b.stderr).strip().splitlines()[:8]: print("   > " + l[:240])
    finally:
        open(p, "w", encoding="utf-8").write(src)
        assert sha(p) == h0, "RESTORE FAILED"
        print("  restored, sha256 %s..." % h0[:12])
if __name__ == "__main__":
    muts = json.load(open(sys.argv[1]))
    only = sys.argv[2:] 
    for m in muts:
        if only and m["name"] not in only: continue
        run(m["name"], m.get("file","tools/span_id_compiler.py"), m["old"], m["new"], m.get("battery", True))
