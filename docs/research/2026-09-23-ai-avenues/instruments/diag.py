import json, sqlite3, collections, statistics
ROOT="/Users/adamderickandrade/ALL-translation-tool"; MAXN=6
bank=json.load(open(ROOT+"/data/alignment/alignment_full_v1.json"))["links"]
prec=collections.defaultdict(lambda: collections.defaultdict(set))
tibsrc=collections.defaultdict(set)
for l in bank:
    if l.get("d")!=5: continue
    e,t=l.get("eng"),l.get("tib")
    if not t: continue
    tibsrc[t].add((l["course"],l["seg"]))
    if e: prec[t][e].add((l["course"],l["seg"]))
con=sqlite3.connect(ROOT+"/build/hgm_spine_v27_2.db")
gloss=collections.defaultdict(set)
for w,raw in con.execute("select wylie, raw from entries"):
    try: o=json.loads(raw)
    except: continue
    g=o.get("hgm_gloss")
    if not g: continue
    if isinstance(g,str): g=[g]
    for s in g:
        if isinstance(s,str) and s.strip(): gloss[w].add(s.strip())
seg_text={}
for c,s,w,e in con.execute("select course, seq, wylie, english from corpus_segments"):
    seg_text[(c,s)]=(w or "", e or "")
def ngrams(w):
    t=w.split(); out=set()
    for i in range(len(t)):
        for n in range(1,MAXN+1):
            if i+n>len(t): break
            g=" ".join(t[i:i+n]); out.add(g); g2=g.strip(",.;:!?")
            if g2: out.add(g2)
    return out
by=collections.defaultdict(list)
for l in bank:
    if l.get("d")==5 and l.get("eng") and l.get("tib") and l.get("seg") is not None:
        by[(l["course"],l["seg"])].append((l["tib"],l["eng"]))
C=collections.Counter(); n=0
for key,spans in by.items():
    if key not in seg_text: continue
    w,eng=seg_text[key]
    if not w or not eng: continue
    grams=ngrams(w)
    for t,e in spans:
        n+=1
        in_gram = t in grams
        in_eng  = e in eng
        tib_known_bank = bool(tibsrc.get(t,set()) - {key})
        tib_known_dict = t in gloss
        pair_bank = bool(prec.get(t,{}).get(e,set()) - {key})
        pair_dict = e in gloss.get(t,())
        C['total']+=1
        if not in_gram: C['tib NOT an n-gram of the wylie']+=1
        if not in_eng:  C['eng NOT a verbatim substring']+=1
        if in_gram and in_eng:
            if pair_bank or pair_dict: C['HIT (pair attested elsewhere)']+=1
            elif tib_known_bank or tib_known_dict: C['MISS: tib known, this English never seen for it']+=1
            else: C['MISS: tib itself unknown to bank+dictionary']+=1
print("banked d5 spans examined:",n)
for k,v in C.most_common(): print("  %-52s %6d  %5.1f%%"%(k,v,100*v/n))
# how much of the miss is novel-English-for-known-Tibetan
