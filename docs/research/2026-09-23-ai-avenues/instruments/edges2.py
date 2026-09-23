exec(open('edges.py').read())
W=re.compile(r"[A-Za-z0-9ༀ-࿿]")
def toks(text, pat):
    return [(m.start(),m.end()) for m in re.finditer(pat,text)]
def classify(text, spans, pat, label):
    tk=toks(text,pat)
    starts={a for a,b in tk}; ends={b for a,b in tk}
    c=collections.Counter(); ex=collections.Counter()
    for (d,lab,cls,st,en,nul) in spans:
        if d==1 and lab.endswith("S"): continue
        if st==en: c['empty']+=1; continue
        okS = st in starts; okE = en in ends
        inone = any(a<=st and en<=b for a,b in tk)
        if okS and okE: c['range']+=1
        elif inone: c['subtoken']+=1; ex['sub:'+text[st:en]]+=1
        else:
            c['partial_edge']+=1
            # find token containing edges
            s0=[(a,b) for a,b in tk if a<st<b]; e0=[(a,b) for a,b in tk if a<en<b]
            key=''
            if s0: key+='S['+text[s0[0][0]:st]+'|'+text[st:s0[0][1]]+']'
            if e0: key+='E['+text[e0[0][0]:en]+'|'+text[en:e0[0][1]]+']'
            if not s0 and not okS: key+='Sgap'
            if not e0 and not okE: key+='Egap'
            ex['part:'+key]+=1
    return c,ex
for name,tpat,epat in [
  ("wordch", r"[A-Za-z0-9ༀ-࿿]+|[^\sA-Za-z0-9ༀ-࿿]", r"[A-Za-z0-9]+|[^\sA-Za-z0-9]"),
  ("syll+apos", r"[^\s,/;|]+|[,/;|]", r"[A-Za-z0-9]+(?:['’][A-Za-z0-9]+)*|[^\sA-Za-z0-9]"),
]:
    CT=collections.Counter(); ET=collections.Counter(); XT=collections.Counter(); XE=collections.Counter()
    for f,course,seq,w,e,ts,es in SEGS:
        c,x=classify(w,ts,tpat,'t'); CT+=c; XT+=x
        c,x=classify(e,es,epat,'e'); ET+=c; XE+=x
    print("==",name); print(" TIB",CT); print("  ",XT.most_common(40)); print(" ENG",ET); print("  ",XE.most_common(40))
