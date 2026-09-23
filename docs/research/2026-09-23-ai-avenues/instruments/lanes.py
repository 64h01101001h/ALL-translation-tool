import json,sqlite3,collections,re,statistics
ROOT="/Users/adamderickandrade/ALL-translation-tool"
con=sqlite3.connect(ROOT+"/build/hgm_spine_v27_2.db")
C18=["C%02d"%i for i in range(1,19)]
rows=[(c,s,w or "",e or "") for c,s,w,e in con.execute("select course,seq,wylie,english from corpus_segments")]
bank=json.load(open(ROOT+"/data/alignment/alignment_full_v1.json"))["links"]
landed={(l["course"],l["seg"]) for l in bank if l.get("seg") is not None}
sub=[r for r in rows if r[0] in C18]
rem=[r for r in sub if (r[0],r[1]) not in landed and r[0]!="C13"]
seen={(w,e) for c,s,w,e in sub if (c,s) in landed}; loc=set(); uniq=[]
for c,s,w,e in rem:
    if (w,e) in seen or (w,e) in loc: continue
    loc.add((w,e)); uniq.append((c,s,w,e))
print("unique mandate:",len(uniq))
# E-071 marker rows: wylie field holds English running head -> almost no tibetan-legal syllables
TIBLIKE=re.compile(r"^[a-zA-Z'\-\.]+$")
ENGWORDS=set("the a an and or of to in for with is are was were be by on at from that this course reading class one two three four five six seven eight nine ten chapter part section review homework quiz answer key".split())
mark=0; short=0; phon=0; cap1500=0
for c,s,w,e in uniq:
    toks=w.split()
    engy=sum(1 for t in toks if t.lower().strip(",.;:") in ENGWORDS)
    if toks and engy/len(toks)>0.25: mark+=1
    if len(toks)<=3: short+=1
    if len(e)==1500: cap1500+=1
print("looks like an English running head in the wylie field (E-071 class, >25%% English stopwords): %d (%.1f%%)"%(mark,100*mark/len(uniq)))
print("<=3 wylie syllables (trivial rows): %d (%.1f%%)"%(short,100*short/len(uniq)))
print("english field exactly 1500 chars (E-107 ingest cap): %d (%.1f%%)"%(cap1500,100*cap1500/len(uniq)))
# how many flags per segment for the span-head queue
print()
# bank yield figures
d5=[l for l in bank if l.get("d")==5 and l.get("eng") and l.get("tib")]
pairs={(l["tib"],l["eng"]) for l in d5}; heads={l["tib"] for l in d5}
print("bank d5 links with English: %d over %d segments = %.2f/segment"%(len(d5),len(landed),len(d5)/len(landed)))
print("distinct (tib,eng) pairs: %d = %.2f/segment"%(len(pairs),len(pairs)/len(landed)))
print("distinct Tibetan headwords: %d = %.2f/segment"%(len(heads),len(heads)/len(landed)))
# overlap with dictionary
gloss=collections.defaultdict(set)
for w,raw in con.execute("select wylie, raw from entries"):
    try:o=json.loads(raw)
    except:continue
    g=o.get("hgm_gloss")
    if not g: continue
    if isinstance(g,str): g=[g]
    for x in g:
        if isinstance(x,str) and x.strip(): gloss[w].add(x.strip())
already={p for p in pairs if p[1] in gloss.get(p[0],())}
print("pairs already present verbatim in that headword's hgm_gloss: %d (%.1f%%)"%(len(already),100*len(already)/len(pairs)))
newheads=[h for h in heads if h not in gloss]
print("bank headwords with NO hgm_gloss in the dictionary: %d (%.1f%%)"%(len(newheads),100*len(newheads)/len(heads)))
