import os, json, collections, re
SC="/private/tmp/claude-501/-Users-adamderickandrade-ALL-translation-tool/463f1ae3-dff6-4666-a672-f84264aa4eb2/scratchpad/c05"
bank=json.load(open('/Users/adamderickandrade/ALL-translation-tool/data/alignment/alignment_full_v1.json'))
notes=bank['notes']
# map C05 seq -> note text, via links (page key per seg)
seg2page={}
for l in bank['links']:
    if l['course']=='C05': seg2page[l['seg']]=l['page']
def load(p):
    o=json.load(open(p)); return o["segments"][0] if "segments" in o else o
seqs=[d.split("-")[0] for d in sorted(os.listdir(SC))
      if d.endswith("-tibetan-first") and os.path.exists(f"{SC}/{d}/spec.json")
      and os.path.exists(f"{SC}/{d.split('-')[0]}-english-first/spec.json")
      and os.path.exists(f"{SC}/{d.split('-')[0]}-reconciled.json")]
k3=lambda x:(x.get("tib"),x.get("eng"),x.get("d"))
C=collections.Counter(); nseg=0
for s in seqs:
    seq=int(s)
    page=seg2page.get(seq)
    if not page: continue
    body=" ".join(re.sub('<[^>]+>','',n) for n in notes.get("C05/"+page,[]))
    if not body: continue
    nseg+=1
    A=load(f"{SC}/{s}-tibetan-first/spec.json"); B=load(f"{SC}/{s}-english-first/spec.json"); R=load(f"{SC}/{s}-reconciled.json")
    a3=collections.Counter(k3(x) for x in A.get("spans",[])); b3=collections.Counter(k3(x) for x in B.get("spans",[]))
    agreedtib=set(); contestedtib=set()
    rs=R.get("spans",[])
    seen=collections.Counter()
    for x in rs:
        k=k3(x); i=seen[k]; seen[k]+=1
        t=x.get("tib")
        if a3[k]>i and b3[k]>i: agreedtib.add(t)
        else: contestedtib.add(t)
    agreedtib -= contestedtib
    for t in agreedtib:
        C["agreed_tib"]+=1
        if t and t in body: C["agreed_in_note"]+=1
    for t in contestedtib:
        C["contested_tib"]+=1
        if t and t in body: C["contested_in_note"]+=1
print("segments with a landed note:",nseg)
print("distinct AGREED tib strings   : %d, of which the note discusses %d (%.1f%%)"%(
    C["agreed_tib"],C["agreed_in_note"],100*C["agreed_in_note"]/C["agreed_tib"]))
print("distinct CONTESTED tib strings: %d, of which the note discusses %d (%.1f%%)"%(
    C["contested_tib"],C["contested_in_note"],100*C["contested_in_note"]/C["contested_tib"]))
