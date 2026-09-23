import io, os, re, sys, json, html, subprocess, sqlite3
ROOT="/Users/adamderickandrade/ALL-translation-tool"
sys.path.insert(0, os.path.join(ROOT,"tools"))
import build_alignment_layer as B

WORDCH = re.compile(r"[A-Za-z0-9ༀ-࿿]")
TAG = re.compile(r'<span\b([^>]*)>|</span>')

def parse_side(block):
    """-> (plain_text, [ (d,lid,cls,start,end) in open-tag order ])"""
    block = re.sub(r'^\s*<div class="(?:tib|eng)"><div class="lab">.*?</div>\s*','',block,flags=re.S)
    block = re.sub(r'\s*</div>\s*$','',block)
    out=[]; stack=[]; text=[]; pos=0; i=0
    for m in TAG.finditer(block):
        text.append(html.unescape(block[i:m.start()])); pos=len("".join(text)); i=m.end()
        if m.group(0).startswith("</"):
            if not stack: return None,None
            d,lid,cls,st = stack.pop()
            out.append((d,lid,cls,st,pos))
        else:
            a=m.group(1)
            dm=re.search(r'data-d="(\d)"',a); lm=re.search(r'data-l="([^"]*)"',a)
            cm=re.search(r'class="([^"]*)"',a)
            cls=(cm.group(1) if cm else "u")
            if dm and lm: stack.append((int(dm.group(1)),lm.group(1),cls,pos))
            else: stack.append((None,None,cls,pos))
    text.append(html.unescape(block[i:]))
    if stack: return None,None
    plain="".join(text)
    out=[o for o in out if o[0] is not None]
    out.sort(key=lambda t:(t[3],-(t[4]-t[3])))
    return plain,out

def body_of(doc):
    a=doc.find('</div>\n<h2>')
    i=doc.find('\n<div class="legend">')
    if a<0 or i<0: return None
    return doc[a+len('</div>\n'):i]

def segblocks(body):
    segs=[]
    parts=re.split(r'(?=<h2>Segment )', body)
    for p in parts:
        m=re.match(r'<h2>Segment (\d+) &mdash; (.*?)</h2>\n<div class="pair">\n', p)
        if not m: continue
        seq=int(m.group(1)); title=html.unescape(m.group(2))
        rest=p[m.end():]
        nm=re.search(r'<div class="note"><b>AI NOTE</b> &mdash; (.*)</div>\n?$', rest, re.S)
        note=None
        if nm:
            note=nm.group(1); rest=rest[:nm.start()]
        rest=rest.rstrip()
        if rest.endswith('</div>'): rest=rest[:-len('</div>')].rstrip()
        j=rest.find('<div class="eng">')
        if j<0: continue
        segs.append((seq,title,rest[:j].rstrip(),rest[j:],note))
    return segs

def needs_subword(text,piece,lo,hi):
    if WORDCH.match(piece[0]) and lo>0 and WORDCH.match(text[lo-1]): return True
    if WORDCH.match(piece[-1]) and hi<len(text) and WORDCH.match(text[hi]): return True
    return False

def spec_from_block(seq,title,tib_block,eng_block,note):
    tp,ts=parse_side(tib_block)
    ep,es=parse_side(eng_block)
    if tp is None or ep is None: return None,"unbalanced span markup"
    pre="s%d"%seq
    if not ts or ts[0][0]!=1 or ts[0][1]!=pre+"S" or ts[0][3]!=0 or ts[0][4]!=len(tp):
        return None,"tibetan block is not the generator's d=1 wrapper"
    if not es or es[0][0]!=1 or es[0][1]!=pre+"S":
        return None,"english block is not the generator's d=1 wrapper"
    engmap={}; order=[]; nul={}
    for d,lid,cls,lo,hi in es[1:]:
        if lid==pre+"S": return None,"duplicate sentence wrapper"
        frag=ep[lo:hi]
        if frag=="" :
            continue
        if lid in engmap: return None,"english span %s is discontinuous"%lid
        engmap[lid]=(frag,lo,hi); order.append(lid)
    # null markers: eng spans whose content is a .nul marker -> zero-width after unescape? recover from raw
    nulraw=re.findall(r'<span class="u(?:[^"]*)" data-d="(\d)" data-l="([^"]*)"><span class="nul">(.*?)</span></span>', eng_block, re.S)
    for d,lid,body in nulraw:
        nul[lid]=html.unescape(body)
        engmap.pop(lid,None)
        if lid in order: order.remove(lid)
    spans=[]
    for d,lid,cls,lo,hi in ts[1:]:
        if not lid.startswith(pre): return None,"label %r not of this segment"%lid
        sid=lid[len(pre):]
        piece=tp[lo:hi]
        sp={"id":sid,"d":d,"tib":piece}
        c=cls.split()
        if c and c[0]=="u" and len(c)>1: sp["cls"]=" ".join(c[1:])
        if lid in engmap:
            sp["eng"]=engmap[lid][0]
        else:
            sp["eng"]=None
            if lid in nul: sp["nul"]=nul[lid]
        sw=needs_subword(tp,piece,lo,hi)
        if sp["eng"] is not None:
            f,elo,ehi=engmap[lid]
            sw = sw or needs_subword(ep,f,elo,ehi)
        if sw and d!=7: sp["subword"]=True
        spans.append(sp)
    eo=[l[len(pre):] for l in order if engmap.get(l)]
    # drop d=7 from eng_order
    dby={s["id"]:s["d"] for s in spans}
    eo=[x for x in eo if dby.get(x)!=7]
    seg={"seq":seq,"title":title,"spans":spans,"eng_order":eo}
    if note: seg["note"]=note
    return seg,None
