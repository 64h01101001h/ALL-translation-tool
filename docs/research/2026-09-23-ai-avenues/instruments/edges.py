import re,sys,collections
exec(open('survey.py').read().split('stats = collections.Counter()')[0])
def blocks(f):
    s=open(f,encoding='utf-8').read()
    for ch in s.split('<div class="pair">')[1:]:
        ti=ch.find('<div class="tib">'); ei=ch.find('<div class="eng">')
        if ti<0 or ei<0: continue
        tb=ch[ti:ei]; tb=tb[tb.find("</div>")+6:]; tb=tb.rsplit("</div>",1)[0]
        eb=ch[ei:]; eb=eb[eb.find("</div>")+6:]; k=eb.find("\n</div>\n</div>"); eb=eb[:k] if k>=0 else eb
        yield tb.strip("\n"), eb.strip("\n")
SEGS=[]
for d in page_dirs():
    course = "C02" if d=="pages" else "C"+d.split("_c")[1]
    for f in sorted(glob.glob(os.path.join(ROOT,"data/alignment",d,"*.html"))):
        for tb,eb in blocks(f):
            try:
                tp,ts=walk(tb); ep,es=walk(eb)
            except Exception: continue
            labs=[x[1] for x in ts if x[0]==1]
            if not labs: continue
            seq=int(re.match(r"s(\d+)",labs[0]).group(1))
            row=db.execute("select wylie,english from corpus_segments where course=? and seq=?",(course,seq)).fetchone()
            if not row or row[0]!=tp or row[1]!=ep: continue
            SEGS.append((f,course,seq,row[0],row[1],ts,es))
print("good segs",len(SEGS))
