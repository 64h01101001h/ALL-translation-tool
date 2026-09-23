import re,html,json,collections,sqlite3,sys
SPAN = re.compile(r'<span class="u(?:[^"]*)" data-d="(\d)" data-l="([A-Za-z0-9_ ]+?)">')
def strip_tags(s): return html.unescape(re.sub(r"<[^>]+>", "", s))
def spans_of(doc, side):
    out = []
    for chunk in doc.split('<div class="pair">')[1:]:
        for stop in ('<div class="note"', '<h2'):
            i = chunk.find(stop)
            if i >= 0: chunk = chunk[:i]
        j = chunk.find('<div class="eng">')
        if j < 0: continue
        block = chunk[:j] if side == "tib" else chunk[j:]
        block = re.sub(r'<span class="nul">.*?</span>', "", block, flags=re.S)
        block = re.sub(r"<i[^>]*>\[NO ENGLISH.*?</i>", "", block, flags=re.S)
        pos, stack, hits = 0, [], []
        for tk in re.finditer(r'<span\b[^>]*>|</span>', block):
            if tk.group(0).startswith("</"):
                if stack:
                    d, l, start = stack.pop(); hits.append((d, l, block[start:tk.start()]))
            else:
                m2 = SPAN.match(tk.group(0))
                if m2:
                    toks = m2.group(2).split(" ")
                    stack.append((int(m2.group(1)), toks[0] + ("|case" if "case" in toks else ""), tk.end()))
                else: stack.append((0, "", tk.end()))
        for d, l, raw in hits:
            if d and l: out.append((d, l, " ".join(strip_tags(raw).split())))
    return out
def links(path):
    doc=open(path,encoding='utf-8').read()
    tibAll={}; engAll={}
    for d,l,txt in spans_of(doc,'tib'):
        lid=l.split('|')[0]; tibAll.setdefault((d,lid),txt)
    for d,l,txt in spans_of(doc,'eng'):
        lid=l.split('|')[0]; engAll.setdefault((d,lid),[]).append(txt)
    out=[]
    for (d,lid),t in tibAll.items():
        m=re.match(r's(\d+)',lid); seg=int(m.group(1)) if m else None
        e=" … ".join(engAll.get((d,lid),[])) or None
        out.append(dict(seg=seg,d=d,lid=lid,tib=t,eng=e))
    return out
A={}; B={}
for n in range(56,123):
    A[n]=links(f'claude_pages/c5p{n}.html'); B[n]=links(f'codex_pages/c5p{n}.html')
json.dump({'claude':A,'codex':B},open('banks_overlap.json','w'))
def summ(L):
    c=collections.Counter()
    for x in L:
        c['all']+=1; c['d%d'%x['d']]+=1
        if x['d']==5: c['d5_eng' if x['eng'] else 'd5_null']+=1
    return c
ca=collections.Counter(); cb=collections.Counter()
for n in A: ca.update(summ(A[n])); cb.update(summ(B[n]))
print('claude',dict(ca)); print('codex',dict(cb))
