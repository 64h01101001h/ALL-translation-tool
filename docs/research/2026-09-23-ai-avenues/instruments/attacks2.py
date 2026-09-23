from harness import *
C,Q="C05",169
w,e=row(C,Q)
def T(piece,n=1): a=nth(w,piece,n); return (a,a+len(piece))
def E(piece,n=1,word=True): a=nth(e,piece,n,word); return (a,a+len(piece))
g1=T("gnyis ka",1); n3=nth(w,"nges",4)
print(repr(w[g1[0]:n3+6]))
attempt("A18 subword(Tib cut) -> English 'in' raw-found inside 'definite'",C,Q,[dict(id="a",d=5,tib=g1,eng=E("deeds",3)),dict(id="x",d=5,tib=(n3,n3+3),eng=E("in",3))])
attempt("A18b same without the cut (no subword)",C,Q,[dict(id="a",d=5,tib=g1,eng=E("deeds",3)),dict(id="x",d=5,tib=(n3,n3+4),eng=E("in",3))])
attempt("A18c subword from ENGLISH cut 'defin' of definite(3) ",C,Q,[dict(id="a",d=5,tib=g1,eng=E("deeds",3)),dict(id="x",d=5,tib=(n3,n3+4),eng=(nth(e,"definite",5),nth(e,"definite",5)+5))])
# English 'the' inside 'neither' via subword: choose word 'neither' part 'the'
ne=nth(e,"neither"); 
attempt("A19 bracket 'the' inside 'neither' (only chosen, others unclaimed)",C,Q,[dict(id="x",d=5,tib=T("gnyis ka",2),eng=(ne+3,ne+6))])
attempt("A19b bracket 'the' of neither after 'definite'(6th) claimed",C,Q,[dict(id="a",d=5,tib=T("gnyis ka",2),eng=(nth(e,"definite",6),nth(e,"definite",6)+8)),dict(id="x",d=5,tib=T("nges pa",5),eng=(ne+3,ne+6))])
# Unicode / punctuation English: parenthesis numbers
attempt("A20 '(2)' English, chosen after deeds(1)",C,Q,[dict(id="a",d=5,tib=T("rnam smin"),eng=E("deeds",1)),dict(id="x",d=6,tib=T("la"),eng=(nth(e,"(2)",1,False),nth(e,"(2)",1,False)+3))])
attempt("A20b ')' 2nd occurrence alone",C,Q,[dict(id="x",d=6,tib=T("la"),eng=(nth(e,")",2,False),nth(e,")",2,False)+1))])
# hyphen double
attempt("A21 second '-' of '--' after Others",C,Q,[dict(id="a",d=5,tib=T("gzhan dag"),eng=E("Others")),dict(id="x",d=6,tib=T("mu"),eng=(nth(e,"-",2,False),nth(e,"-",2,False)+1))])
attempt("A21b first '-' of 2nd '--' (after 'school')",C,Q,[dict(id="a",d=5,tib=T("gzhan dag"),eng=E("school")),dict(id="x",d=6,tib=T("mu"),eng=(nth(e,"-",3,False),nth(e,"-",3,False)+1))])
