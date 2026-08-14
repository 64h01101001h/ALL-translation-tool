wc = wcl386
flags = /c /cc /ms /W3 /3 /fpi /omax /zp1 /bt=NT

tcg.exe: t1.obj t2.obj t3.obj t4.obj utils.obj tests.obj bcd.obj
    wlink system nt name tcg file t1, t2, t3, t4, utils, tests, bcd

t1.obj: t1.c  tc.h tc.def bcd.h
        $(wc) $(flags) t1.c

t2.obj: t2.c  tc.h tc.def tc.ext bcd.h
        $(wc) $(flags) t2.c

t3.obj: t3.c tc.h tc.def tc.ext bcd.h
        $(wc) $(flags) t3.c

t4.obj: t4.c tc.h tc.def tc.ext bcd.h
        $(wc) $(flags) t4.c

utils.obj: utils.c tc.h tc.def tc.ext bcd.h
        $(wc) $(flags) utils.c

tests.obj: tests.c tc.h
        $(wc) $(flags) tests.c

bcd.obj: bcd.c bcd.h
        $(wc) $(flags) bcd.c
