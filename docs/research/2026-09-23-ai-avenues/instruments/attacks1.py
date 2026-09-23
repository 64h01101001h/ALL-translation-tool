from harness import *
C,Q="C05",169
w,e=row(C,Q)
def T(piece,n=1): a=nth(w,piece,n); return (a,a+len(piece))
def E(piece,n=1,word=True): a=nth(e,piece,n,word); return (a,a+len(piece))
print("C05:169 E:",e[:120])
# 1 repeated English 'the'
attempt("A1 2nd 'the' alone (first 'the' unclaimed before it)",C,Q,[dict(id="x",d=5,tib=T("mu"),eng=E("the",2))])
attempt("A2 'Others' then 2nd 'the' (1st 'the' between)",C,Q,[dict(id="w4",d=5,tib=T("gzhan dag"),eng=E("Others")),dict(id="x",d=5,tib=T("mu"),eng=E("the",2))])
attempt("A3 'group' then 2nd 'the' (cursor past 1st)",C,Q,[dict(id="w3",d=5,tib=T("sde pa",2) if False else (nth(w,"sde pa",2),nth(w,"sde pa",2)+6),eng=E("group")),dict(id="x",d=5,tib=T("mu"),eng=E("the",2))])
attempt("A4 crossing: X->3rd 'the', Y->Others",C,Q,[dict(id="x",d=5,tib=T("dpe ston pa"),eng=E("the",3)),dict(id="y",d=5,tib=T("gzhan dag"),eng=E("Others"))])
attempt("A17 crossing chain the2/the3 behind 'group'",C,Q,[dict(id="x",d=5,tib=T("dpe ston pa"),eng=E("the",3)),dict(id="y",d=5,tib=T("gzhan dag"),eng=E("the",2)),dict(id="z",d=5,tib=T("mu"),eng=E("group"))])
attempt("A17b crossing: 3rd 'the' first in Tib, 1st 'the' later in Tib",C,Q,[dict(id="x",d=5,tib=T("dpe ston pa"),eng=E("the",3)),dict(id="y",d=5,tib=T("mu"),eng=E("the",1))])
attempt("A17c crossing: 1st 'the' first in Tib, 3rd 'the' later, nothing between",C,Q,[dict(id="x",d=5,tib=T("dpe ston pa"),eng=E("the",1)),dict(id="y",d=5,tib=T("mu"),eng=E("the",3))])
attempt("A17d both 'the' spans: 2nd then 3rd, 1st unclaimed",C,Q,[dict(id="x",d=5,tib=T("dpe ston pa"),eng=E("the",2)),dict(id="y",d=5,tib=T("mu"),eng=E("the",3))])
# 'in' inside 'definite' with derived subword
attempt("A5 'deeds' then 'in'(both respects) plain",C,Q,[dict(id="a",d=5,tib=T("gnyis ka"),eng=E("deeds",3)),dict(id="x",d=5,tib=T("nges",4),eng=E("in",3))])
attempt("A5b same, Tibetan bracket cut forces subword",C,Q,[dict(id="a",d=5,tib=T("gnyis ka"),eng=E("deeds",3)),dict(id="x",d=5,tib=(nth(w,"nges",4),nth(w,"nges",4)+3),eng=E("in",3))])
attempt("A5c 'in' after 'Exemplist' -> chosen E23 (1st in between)",C,Q,[dict(id="a",d=5,tib=T("dpe ston pa"),eng=E("Exemplist")),dict(id="x",d=5,tib=T("mu"),eng=E("in",2))])
# Tibetan repeated nges pa
attempt("A6 2nd 'nges pa' alone",C,Q,[dict(id="x",d=5,tib=T("nges pa",2),eng=None)])
attempt("A6b 'gnas skabs' then 2nd 'nges pa'",C,Q,[dict(id="g",d=5,tib=T("gnas skabs"),eng=None),dict(id="x",d=5,tib=T("nges pa",2),eng=None)])
attempt("A6c 'nges' of 'nges la' (3rd nges) after 'gnas skabs'(2nd) ",C,Q,[dict(id="g",d=5,tib=T("gnas skabs",2),eng=None),dict(id="x",d=5,tib=T("nges",3),eng=None)])
# la recurring
attempt("A7 2nd 'la' after rnam smin(1st)",C,Q,[dict(id="r",d=5,tib=T("rnam smin"),eng=None),dict(id="x",d=6,tib=T("la",2),eng=None)])
attempt("A7b 2nd 'la' after gnas skabs(2nd)",C,Q,[dict(id="r",d=5,tib=T("gnas skabs",2),eng=None),dict(id="x",d=6,tib=T("la",2),eng=None)])
attempt("A7c 'ni' (only one) alone d=6",C,Q,[dict(id="x",d=6,tib=T("ni"),eng=None)])
# shad
attempt("A11 2nd comma alone",C,Q,[dict(id="x",d=6,tib=(nth(w,",",2,False),nth(w,",",2,False)+1),eng=None)])
attempt("A11b 2nd comma after ma nges pa",C,Q,[dict(id="m",d=5,tib=T("ma nges pa"),eng=None),dict(id="x",d=6,tib=(nth(w,",",2,False),nth(w,",",2,False)+1),eng=None)])
# members with repeated English inside parent
par=(nth(w,"rnam smin"),nth(w,"nges pa",2)+7)   # rnam smin ... ma nges pa  (T19-T28)
peng=(nth(e,"deeds"),nth(e,"time")+4)
print("parent tib:",w[par[0]:par[1]]," | eng:",e[peng[0]:peng[1]])
attempt("A9 member 'ma'->2nd 'definite' in parent",C,Q,[dict(id="p",d=5,tib=par,eng=peng),dict(id="m1",d=7,tib=T("ma"),eng=E("definite",2)),dict(id="m2",d=7,tib=T("gnas skabs"),eng=E("time"))])
attempt("A9b member 'ma'->1st 'definite' in parent",C,Q,[dict(id="p",d=5,tib=par,eng=peng),dict(id="m1",d=7,tib=T("ma"),eng=E("definite",1)),dict(id="m2",d=7,tib=T("gnas skabs"),eng=E("time"))])
attempt("A9c members: rnam smin->2nd definite, gnas skabs->1st definite",C,Q,[dict(id="p",d=5,tib=par,eng=peng),dict(id="m1",d=7,tib=T("rnam smin"),eng=E("definite",2)),dict(id="m2",d=7,tib=T("gnas skabs"),eng=E("definite",1))])
attempt("A9d members crossing: rnam smin->ripen, gnas skabs->definite(1)",C,Q,[dict(id="p",d=5,tib=par,eng=peng),dict(id="m1",d=7,tib=T("rnam smin"),eng=E("ripen")),dict(id="m2",d=7,tib=T("gnas skabs"),eng=E("definite",1))])
attempt("A9e member 'la'->'at' inside parent",C,Q,[dict(id="p",d=5,tib=par,eng=peng),dict(id="m1",d=7,tib=T("la"),eng=E("at"))])
attempt("A9f member 'nges' (occurs twice in parent) ",C,Q,[dict(id="p",d=5,tib=par,eng=peng),dict(id="m1",d=7,tib=T("nges",2),eng=None)])
attempt("A9g member English sub-word 'de' of 1st definite",C,Q,[dict(id="p",d=5,tib=par,eng=peng),dict(id="m1",d=7,tib=T("ma"),eng=(nth(e,"definite",2),nth(e,"definite",2)+2))])
# genitive 'i
attempt("A15 member 'i in mdo sde pa'i",C,Q,[dict(id="p",d=5,tib=(nth(w,"mdo"),nth(w,"pa'i")+4),eng=E("Sutrist")),dict(id="g",d=7,tib=(nth(w,"'i",1,False),nth(w,"'i",1,False)+2),eng=None)])
attempt("A15b flat d=6 'i after mdo sde pa",C,Q,[dict(id="p",d=5,tib=(nth(w,"mdo"),nth(w,"pa'i")+2),eng=E("Sutrist")),dict(id="g",d=6,tib=(nth(w,"'i",1,False),nth(w,"'i",1,False)+2),eng=None)])
attempt("A15c final 'o flat after gnyis ka ma nges pa",C,Q,[dict(id="p",d=5,tib=T("nges pa",5),eng=None),dict(id="o",d=6,tib=(nth(w,"'o",1,False),nth(w,"'o",1,False)+2),eng=None)])
