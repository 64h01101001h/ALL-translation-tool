exec(open('edges.py').read())
pat=r"[A-Za-z0-9ༀ-࿿]+|[^\sA-Za-z0-9ༀ-࿿]"
c=collections.Counter(); ex=[]
dd=collections.Counter()
for f,course,seq,w,e,ts,es in SEGS:
    for side,text,spans in (('t',w,ts),('e',e,es)):
        tk=[(m.start(),m.end()) for m in re.finditer(pat,text)]
        ends={b for a,b in tk}; starts={a for a,b in tk}
        for (d,lab,cls,st,en,nul) in spans:
            if st==en: continue
            if en not in ends and not any(a<en<b for a,b in tk):
                c[(side,repr(text[en-1:en+1]))]+=1; dd[(side,d,lab[-1] if lab[-1]=='S' else 'x')]+=1
                if len(ex)<8: ex.append((os.path.basename(f),seq,side,d,lab,repr(text[st:en][-30:])))
            if st not in starts and not any(a<st<b for a,b in tk):
                c[('S'+side,repr(text[st-1:st+1]))]+=1
print(c.most_common(20)); print(dd); print(*ex,sep="\n")
# also nonword chars in wylie
cc=collections.Counter()
for f,course,seq,w,e,ts,es in SEGS:
    for ch in w:
        if not re.match(r"[A-Za-z0-9\s]",ch): cc[ch]+=1
print(cc.most_common())
