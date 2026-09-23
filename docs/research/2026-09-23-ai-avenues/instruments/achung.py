from harness import *
import build_alignment_layer as B
db=sqlite3.connect(B.SPINE)
r=db.execute("select seq,wylie,english,acip from corpus_segments where course='C05' and wylie like '%grub mtha'' %' limit 1").fetchone()
seq,w,e,acip=r
print("C05:%d"%seq, w[:140])
tt=S.tokenize(w,'tib')
a=w.find("grub mtha'")
toks=[(n,w[x:y]) for n,(x,y) in enumerate(tt,1) if a<=x<a+12]
print("packet tokens:",toks)
n1=toks[0][0]; n2=toks[1][0]
res=attempt("A-CHUNG: plain ref T%d-T%d on \"grub mtha'\" (the packet's own cut)"%(n1,n2),"C05",seq,[dict(id="w1",d=5,tib=(a,a+9),eng=None)])
spec=S.compile_proposal({"course":"C05","segments":[{"seq":seq,"src":S.source_hash(w,e)[:16],"title":"t","spans":[{"id":"w1","d":5,"t":"T%d-T%d"%(n1,n2),"e":None}]}]})
sp=spec["segments"][0]["spans"][0]
print("emitted tib:",repr(sp["tib"]),"subword:",sp.get("subword"))
rc,body,err=gen_cli(spec); print("generator exit",rc)
print("builder would bank tib:",[x for x in B.spans_of(body,"tib") if x[1]=="s%dw1"%seq])
print("ACIP recovered for it:",repr(B.acip_span(acip,w,sp["tib"])))
# no_split_syllables logic
import re
LET=re.compile(r"[A-Za-z]")
SPAN=re.compile(r'<span class="u[^"]*" data-d="(\d)" data-l="(s\d+\w+)"[^>]*>([^<]*)</span>')
chunk=body.split('<div class="tib">')[1].split('<div class="eng">')[0]
for m in SPAN.finditer(chunk):
    nxt=chunk[m.end():m.end()+8]
    print("no_split_syllables sees next chars %r -> flagged=%s"%(nxt, bool(nxt and LET.match(nxt[0]))))
