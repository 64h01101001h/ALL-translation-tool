from harness import *
import copy
C,Q="C05",169
w,e=row(C,Q); src=S.source_hash(w,e)[:16]
def comp(name, prop):
    try:
        out=S.compile_proposal(prop); print("%-60s COMPILED"%name); return out
    except S.Refusal as x:
        print("%-60s REFUSED %s: %s"%(name,x.code,str(x)[:170])); return None
    except Exception as x:
        print("%-60s *** CRASH %r"%(name,x)); return None
base={"id":"w1","d":5,"t":"T1-T3","e":"E7"}
def P(spans=None, **kw):
    s={"seq":Q,"src":src,"title":"t","spans":[base] if spans is None else spans}; s.update(kw)
    return {"course":C,"segments":[s]}
comp("empty ref ''", P([dict(base,t="")]))
comp("whitespace ref ' '", P([dict(base,t=" ")]))
comp("bracket of a space 'T3[ ]'", P([dict(base,t="T3[ ]")]))
comp("bracket spanning the gap 'T3-T4[ ]'", P([dict(base,t="T3-T4[ ]")]))
comp("bracket with leading space 'T3-T4[ mdo]'", P([dict(base,t="T3-T4[ mdo]")]))
comp("bracket with trailing space 'T3-T4[pa ]'", P([dict(base,t="T3-T4[pa ]")]))
comp("empty brackets 'T3[]'", P([dict(base,t="T3[]")]))
comp("T0", P([dict(base,t="T0")]))
comp("lowercase t3", P([dict(base,t="t3")]))
comp("int t", P([dict(base,t=3)]))
comp("list e", P([dict(base,e=["E7"])]))
comp("e = '' ", P([dict(base,e="")]))
comp("e = false", P([dict(base,e=False)]))
comp("d = '5'", P([dict(base,d="5")]))
comp("d = true", P([dict(base,d=True)]))
comp("id None", P([dict(base,id=None)]))
comp("cls = '' (empty string)", P([dict(base,cls="")]))
comp("cls = 'nul'", P([dict(base,cls="nul")]))
comp("cls with newline", P([dict(base,cls="a\nb")]))
comp("nul = 5", P([dict(base,e=None,nul=5)]))
comp("nul null with English", P([dict(base,nul=None)]))
comp("src int", P(src=12345))
comp("src 64-hex full", P(src=S.source_hash(w,e)))
comp("src uppercase", P(src=src.upper()))
comp("src 15 chars", P(src=src[:15]))
comp("title int", P(title=5))
comp("seq str", {"course":C,"segments":[{"seq":"169","src":src,"title":"t","spans":[base]}]})
comp("segment not dict", {"course":C,"segments":["x"]})
comp("segments empty", {"course":C,"segments":[]})
comp("course lowercase", {"course":"c05","segments":[{"seq":Q,"src":src,"title":"t","spans":[base]}]})
comp("top-level list", [1])
comp("spans not list", P(spans={"a":1}))
comp("span not dict", P(spans=["x"]))
comp("three-part range T1-T2-T3", P([dict(base,t="T1-T2-T3")]))
comp("unicode digit T٣", P([dict(base,t="T٣", e=None)]))
comp("huge token T99999999999999999999", P([dict(base,t="T99999999999999999999")]))
comp("mixed side range T1-E3", P([dict(base,t="T1-E3")]))
# duplicate seq in one proposal
out=comp("DUPLICATE SEQ: same segment twice, different English", {"course":C,"segments":[
   {"seq":Q,"src":src,"title":"a","spans":[{"id":"w1","d":5,"t":"T1-T3","e":"E7"}]},
   {"seq":Q,"src":src,"title":"b","spans":[{"id":"w1","d":5,"t":"T1-T3","e":"E8"}]}]})
if out:
    rc,body,err=gen_cli(out)
    import build_alignment_layer as B
    doc="<html>"+body
    print("   builder spans_of eng on that page:",[x for x in B.spans_of(doc,"eng") if x[1].startswith("s169w1")])
# note injection
note='see <div class="pair"><div class="eng"><span class="u" data-d="5" data-l="s169w1">group</span></div>'
out=comp("NOTE carrying a pair-div and a span", P(note=note))
if out:
    rc,body,err=gen_cli(out)
    import build_alignment_layer as B
    print("   generator exit",rc,"; builder spans_of eng:",[x for x in B.spans_of(body,"eng") if x[1].startswith("s169w1")])
title='x <div class="pair"><div class="eng"><span class="u" data-d="5" data-l="s169w1">school</span></div>'
out=comp("TITLE carrying a pair-div and a span", P(title=title))
if out:
    rc,body,err=gen_cli(out)
    print("   generator exit",rc,"; builder spans_of eng:",[x for x in B.spans_of(body,"eng") if x[1].startswith("s169w1")])
