from harness import *
C,Q="C05",169
w,e=row(C,Q)
def T(piece,n=1): a=nth(w,piece,n); return (a,a+len(piece))
def E(piece,n=1,word=True): a=nth(e,piece,n,word); return (a,a+len(piece))
g1=T("gnyis ka",1); n5=nth(w,"nges",5)
print(repr(w[g1[0]:n5+6]))
attempt("A18 subword(Tib cut) -> English 'in' raw-found inside 'definite'",C,Q,[dict(id="a",d=5,tib=g1,eng=E("deeds",3)),dict(id="x",d=5,tib=(n5,n5+3),eng=E("in",3))])
attempt("A18b same without the cut (no subword)",C,Q,[dict(id="a",d=5,tib=g1,eng=E("deeds",3)),dict(id="x",d=5,tib=(n5,n5+4),eng=E("in",3))])
attempt("A18c subword from ENGLISH cut 'defin' of definite(5) ",C,Q,[dict(id="a",d=5,tib=g1,eng=E("deeds",3)),dict(id="x",d=5,tib=(n5,n5+4),eng=(nth(e,"definite",5),nth(e,"definite",5)+5))])
ne=nth(e,"neither")
g2=T("gnyis ka",2)
attempt("A19 bracket 'the' inside 'neither', only English claimed",C,Q,[dict(id="a",d=5,tib=g1,eng=None),dict(id="x",d=5,tib=g2,eng=(ne+3,ne+6))])
attempt("A19b same with 'definite'(6) claimed first",C,Q,[dict(id="a",d=5,tib=g1,eng=(nth(e,"definite",6),nth(e,"definite",6)+8)),dict(id="x",d=5,tib=g2,eng=(ne+3,ne+6))])
# Tibetan subword relaxed: 'ka' inside 'skabs'? choose English cut to force subword and Tibetan 'ka' 2nd with cursor before skabs
attempt("A22 subword via English cut; Tib 'ka' raw-found inside 'skabs'",C,Q,[dict(id="a",d=5,tib=T("nges pa",1),eng=None),dict(id="x",d=5,tib=T("ka",1),eng=(ne,ne+3))])
attempt("A22b same without English cut",C,Q,[dict(id="a",d=5,tib=T("nges pa",1),eng=None),dict(id="x",d=5,tib=T("ka",1),eng=E("neither"))])
