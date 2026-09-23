"""Independent round-trip checker for span-id compiler attacks.

Proposals are built from CHARACTER OFFSETS chosen by me; token refs are
derived by my own mapping (not the compiler's express/_show); the emitted spec
is fed to the REAL generator CLI as a subprocess; its HTML is parsed by my own
parser; each label's offsets are compared to what I chose."""
import sys, os, re, json, html, subprocess, tempfile, io, hashlib, sqlite3
ROOT = "/Users/adamderickandrade/ALL-translation-tool"
sys.path.insert(0, os.path.join(ROOT, "tools"))
import span_id_compiler as S

def row(course, seq):
    return S.spine_row(course, seq)

def mytok(text, side):
    # an independent re-implementation of the documented rules, to cross-check
    toks=[]; i=0; n=len(text)
    import unicodedata
    def tc(c): return c.isalnum() or 'ༀ'<=c<='࿿' or unicodedata.category(c).startswith('M')
    while i<n:
        c=text[i]
        if c.isspace(): i+=1; continue
        j=i
        if side=='tib' and c=="'" and i+1<n and tc(text[i+1]): j=i+1
        if tc(text[j]):
            while j<n and tc(text[j]): j+=1
            if side=='eng':
                while j+1<n and text[j] in "'’" and tc(text[j+1]):
                    j+=1
                    while j<n and tc(text[j]): j+=1
            toks.append((i,j)); i=j
        else:
            toks.append((i,i+1)); i+=1
    return toks

def ref(text, toks, a, b, L):
    first=[k for k,(x,y) in enumerate(toks,1) if x<=a<y]
    last=[k for k,(x,y) in enumerate(toks,1) if x<b<=y]
    assert first and last, ("offsets not in tokens", a, b, text[a:b])
    f,l=first[0],last[0]
    r = "%s%d"%(L,f) if f==l else "%s%d-%s%d"%(L,f,L,l)
    if (toks[f-1][0],toks[l-1][1])!=(a,b):
        r += "[%s]"%text[a:b]
    return r

def nth(text, piece, n=1, word=True):
    """offset of the nth (1-based) occurrence of piece; word=True requires
    non-alnum on both sides."""
    k=-1; c=0
    while True:
        k=text.find(piece,k+1)
        if k<0: raise ValueError("no %d-th %r"%(n,piece))
        if word:
            if k>0 and text[k-1].isalnum(): continue
            e=k+len(piece)
            if e<len(text) and text[e].isalnum(): continue
        c+=1
        if c==n: return k

def parse(body):
    """my own reader: label -> (a,b) per side, plus nul labels."""
    m = re.search(r'<div class="tib"><div class="lab">[^<]*</div>\n(.*?)\n</div>\n<div class="eng"><div class="lab">[^<]*</div>\n(.*?)\n</div>\n</div>', body, re.S)
    out=[]
    for blk in (m.group(1), m.group(2)):
        pos=0; stack=[]; spans={}; nuls={}; plain=[]; dup=[]
        innul=0
        for t in re.findall(r'<[^>]+>|[^<]+', blk):
            if t.startswith('<'):
                if t.startswith('</'):
                    kind,lab,st=stack.pop()
                    if kind=='nul': innul-=1
                    elif kind=='u':
                        tgt = spans
                        if lab in spans or lab in nuls: dup.append(lab)
                        if st[1]:
                            nuls[lab]=True
                        else:
                            spans[lab]=(st[0],pos)
                elif 'class="nul"' in t:
                    stack.append(('nul',None,None)); innul+=1
                    # mark enclosing u as nul-holder
                    for i in range(len(stack)-2,-1,-1):
                        if stack[i][0]=='u':
                            k,l,st=stack[i]; stack[i]=(k,l,(st[0],True)); break
                else:
                    lab=re.search(r'data-l="([^"]*)"',t).group(1)
                    stack.append(('u',lab,(pos,False)))
            else:
                if innul: continue
                s=html.unescape(t); plain.append(s); pos+=len(s)
        out.append(("".join(plain), spans, nuls, dup))
    return out

def gen_cli(spec):
    p=subprocess.run([sys.executable, os.path.join(ROOT,"tools","gen_alignment_page.py")],
                     input=json.dumps(spec,ensure_ascii=False),capture_output=True,text=True,cwd=ROOT)
    return p.returncode, p.stdout, p.stderr

LOG=[]
def attempt(name, course, seq, spans, expect=None, title="attack", extra=None):
    """spans: list of dicts with id,d, tib=(a,b), eng=(a,b)|None, cls,nul"""
    wyl, eng = row(course, seq)
    tt, et = mytok(wyl,'tib'), mytok(eng,'eng')
    assert tt==S.tokenize(wyl,'tib') and et==S.tokenize(eng,'eng')
    src=hashlib.sha256(("%s\0%s\0%s"%(S.TOKENIZER,wyl,eng)).encode()).hexdigest()[:16]
    psp=[]
    for s in spans:
        o={"id":s["id"],"d":s["d"],"t":ref(wyl,tt,*s["tib"],"T") if not isinstance(s["tib"],str) else s["tib"]}
        if s.get("eng") is None: o["e"]=None
        else: o["e"]=ref(eng,et,*s["eng"],"E") if not isinstance(s["eng"],str) else s["eng"]
        for k in ("cls","nul"):
            if k in s: o[k]=s[k]
        psp.append(o)
    seg={"seq":seq,"src":src,"title":title,"spans":psp}
    if extra: seg.update(extra)
    prop={"course":course,"segments":[seg]}
    try:
        spec=S.compile_proposal(prop)
    except S.Refusal as e:
        line="%-58s REFUSED %s :: %s" % (name, e.code, str(e)[:260])
        print(line); LOG.append((name,"REFUSED",e.code,str(e)))
        return ("REFUSED", e.code, prop)
    rc,out,err=gen_cli(spec)
    if rc!=0:
        line="%-58s COMPILED BUT GENERATOR CLI REFUSED: %s"%(name,err.strip()[:200])
        print(line); LOG.append((name,"CLI_REFUSED",err)); return ("CLI_REFUSED",err,prop)
    (tp,tsp,tnul,tdup),(ep,esp,enul,edup)=parse(out)
    problems=[]
    if tp!=wyl: problems.append("tib plain != spine")
    if ep!=eng: problems.append("eng plain != spine")
    if tdup or edup: problems.append("dup labels %s %s"%(tdup,edup))
    for s in spans:
        lab="s%d%s"%(seq,s["id"])
        want_t = s["tib"] if not isinstance(s["tib"],str) else None
        if want_t and tsp.get(lab)!=tuple(want_t): problems.append("%s tib %r got %r (%r vs %r)"%(s["id"],want_t,tsp.get(lab), wyl[want_t[0]:want_t[1]], wyl[tsp[lab][0]:tsp[lab][1]] if lab in tsp else None))
        if s.get("eng") is None:
            if lab not in enul or lab in esp: problems.append("%s null marker missing"%s["id"])
        elif not isinstance(s["eng"],str):
            if esp.get(lab)!=tuple(s["eng"]): problems.append("%s eng %r got %r"%(s["id"],s["eng"],esp.get(lab)))
    want={"s%d%s"%(seq,s["id"]) for s in spans}|{"s%dS"%seq}
    if set(tsp)|set(tnul) != want: problems.append("tib labels %s"%sorted(set(tsp)^want))
    if (set(esp)|set(enul)) != want: problems.append("eng labels %s"%sorted((set(esp)|set(enul))^want))
    status = "COMPILED, RESOLVES AS PROPOSED" if not problems else "COMPILED, *** RESOLVES DIFFERENTLY *** "+"; ".join(problems)
    print("%-58s %s  eng_order=%s subword=%s" % (name, status, spec["segments"][0].get("eng_order"), [x["id"] for x in spec["segments"][0]["spans"] if x.get("subword")]))
    LOG.append((name,"OK" if not problems else "MISMATCH",problems))
    return ("OK" if not problems else "MISMATCH", problems, prop)
