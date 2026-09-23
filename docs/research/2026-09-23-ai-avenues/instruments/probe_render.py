import io, json, os, sys, subprocess, tempfile
ROOT="/Users/adamderickandrade/ALL-translation-tool"
sys.path.insert(0, os.path.join(ROOT,"tools"))
import test_view_matches_layer as VM
import build_dictionary_view as V
import test_transfer_provenance as TP

FULL=os.path.join(ROOT,"data","alignment","alignment_full_v1.json")
bank=json.load(io.open(FULL,encoding="utf-8"))
sref="C03:501"; tco,tseq="C07",85; page="c7x1"
twin=[]
for L in bank["links"]:
    if "%s:%s"%(L["course"],L["seg"])==sref:
        t=dict(L); t["course"],t["seg"],t["page"]=tco,tseq,page; t["xfer"]=sref; twin.append(t)
print("synthesised twin links:", len(twin))
bank["links"].extend(twin)
tmp=tempfile.mkdtemp(prefix="render-probe-")
full=os.path.join(tmp,"alignment_full_v1.json"); out=os.path.join(tmp,"view.html")
json.dump(bank, io.open(full,"w",encoding="utf-8"), ensure_ascii=False)
env=dict(os.environ, ALL_VIEW_FULL=full, ALL_VIEW_OUT=out, ALL_VIEW_DOCS=tmp)
r=subprocess.run([sys.executable, os.path.join(ROOT,"tools","build_dictionary_view.py")],
                 capture_output=True, text=True, cwd=ROOT, env=env)
assert r.returncode==0, r.stderr[-400:]
doc=io.open(out,encoding="utf-8").read()
P=VM.payload(doc)
total=vis=hid=0
hidden_examples=[]
for bn,band in list(P["depths"].items())+[("phon",P["phonetics"])]:
    for hw,_a,rl in band:
        for e,refs in rl:
            for i,ref in enumerate(refs):
                if V.XFER_MARK in ref:
                    total+=1
                    # dictionary_view_shell.html line 231: refs.slice(0,3).join(' ') + ' +N'
                    if i<3: vis+=1
                    else:
                        hid+=1
                        if len(hidden_examples)<5:
                            hidden_examples.append((bn,hw,e,len(refs),refs[:3]))
print("carried citations in the payload: %d" % total)
print("  VISIBLE to a reader (inside the first three shown): %d" % vis)
print("  HIDDEN behind the '+N' truncation at dictionary_view_shell.html:231: %d" % hid)
for bn,hw,e,n,first3 in hidden_examples:
    print("    band %s  %-12s -> %-22s %d refs; a reader sees: %s +%d" % (bn,hw[:12],e[:22],n," ".join(first3),n-3))
# gate on the clean scratch page
probs=[]
n,_segs,_er,_em=TP.check_view(out, bank["links"], probs)
print("GATE on this page: marked=%d problems=%d  (EXIT would be %d)" % (n,len(probs),1 if probs else 0))
print("SCRATCH:",tmp)
