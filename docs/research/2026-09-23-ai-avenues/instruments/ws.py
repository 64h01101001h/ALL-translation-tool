exec(open('edges.py').read())
import unicodedata
ws=collections.Counter(); na=collections.Counter(); nulls=0
for f,course,seq,w,e,ts,es in SEGS:
    for side,t in (('t',w),('e',e)):
        for m in re.finditer(r"\s{2,}|[^\S ]",t): ws[(side,repr(m.group(0)))]+=1
        for ch in t:
            if ord(ch)>127: na[(side,ch,unicodedata.category(ch))]+=1
print(ws.most_common(10)); print(na.most_common(40))
r=db.execute("select count(*), sum(wylie is null), sum(english is null) from corpus_segments").fetchone(); print(r)
