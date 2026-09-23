"""Independent recount of coverage and round trip. Does not import span_id_compiler."""
import sys, os, re, glob, html, json, io, sqlite3, collections, unicodedata
ROOT="/Users/adamderickandrade/ALL-translation-tool"
sys.path.insert(0, os.path.join(ROOT,"tools"))
import build_alignment_layer as B
import gen_alignment_page as G
db=sqlite3.connect(B.SPINE)
dirmap={os.path.basename(os.path.normpath(c["dir"])):k for k,c in B.COURSES.items()}
dirs=sorted(d for d in os.listdir(os.path.join(ROOT,"data","alignment")) if d.startswith("pages") and os.path.isdir(os.path.join(ROOT,"data","alignment",d)))
files=[f for d in dirs for f in sorted(glob.glob(os.path.join(ROOT,"data","alignment",d,"*.html")))]
print("dirs on disk:",dirs, "unregistered:",[d for d in dirs if d not in dirmap])
def tc(c): return c.isalnum() or 'ༀ'<=c<='࿿' or unicodedata.category(c).startswith('M')
def tok(text, side):
    out=[]; i=0; n=len(text)
    while i<n:
        if text[i].isspace(): i+=1; continue
        j=i
        if side=='tib' and text[i]=="'" and i+1<n and tc(text[i+1]): j=i+1
        if tc(text[j]):
            while j<n and tc(text[j]): j+=1
            if side=='eng':
                while j+1<n and text[j] in "'’" and tc(text[j+1]):
                    j+=1
                    while j<n and tc(text[j]): j+=1
            out.append((i,j)); i=j
        else: out.append((i,i+1)); i+=1
    return out
BLK=re.compile(r'<div class="pair">\n<div class="tib"><div class="lab">([^<]*)</div>\n(.*?)\n</div>\n<div class="eng"><div class="lab">([^<]*)</div>\n(.*?)\n</div>\n</div>',re.S)
def read(block):
    pos=0; st=[]; spans=[]; plain=[]
    for t in re.findall(r'<[^>]*>|[^<]+',block):
        if t[0]!='<':
            if st and st[-1]=='N':
                for f in reversed(st):
                    if isinstance(f,dict): f['nul']=(f['nul'] or '')+html.unescape(t); break
                continue
            s=html.unescape(t); plain.append(s); pos+=len(s); continue
        if t=='</span>':
            f=st.pop()
            if isinstance(f,dict): f['b']=pos; spans.append(f)
            continue
        if t=='<span class="nul">': st.append('N'); continue
        m=re.fullmatch(r'<span class="u( [^"]*)?" data-d="(\d)" data-l="([^"]*)">',t)
        if not m: raise ValueError(t)
        st.append({'cls':(m.group(1) or '')[1:] or None,'d':int(m.group(2)),'lab':m.group(3),'a':pos,'b':None,'nul':None})
    if st: raise ValueError('unclosed')
    return "".join(plain), spans
cnt=collections.Counter(); bydepth=collections.defaultdict(collections.Counter)
blocks=0; pages=0; unread_blocks=0; unread_tags=0; notspine_blocks=0; notspine_sides=0
tags_total=0
ws_blocks=set(); nu_ex=[]; empty_ex=[]
blockinfo={}
for f in files:
    pages+=1
    doc=io.open(f,encoding='utf-8').read()
    tags_total+=doc.count('<span class="u')
    d=os.path.basename(os.path.dirname(f)); course=dirmap[d]
    parts=doc.split('<div class="pair">')
    for k in range(1,len(parts)):
        blocks+=1
        ch='<div class="pair">'+parts[k]
        m=BLK.match(ch)
        key=(d,os.path.basename(f)[:-5],k)
        if not m:
            unread_blocks+=1; unread_tags+=ch.count('<span class="u'); blockinfo[key]=('UNREAD',None); continue
        tl,tb,el,eb=m.groups()
        try:
            tp,ts=read(tb); ep,es=read(eb)
        except ValueError:
            unread_blocks+=1; unread_tags+=tb.count('<span class="u')+eb.count('<span class="u'); blockinfo[key]=('UNREAD',None); continue
        mm=re.match(r'<span class="u" data-d="1" data-l="s(\d+)S">',tb)
        if mm: seq=int(mm.group(1))
        else:
            h=re.findall(r"<h2>Segment (\d+) &mdash;",parts[k-1]); seq=int(h[-1]) if h else None
        wrap="s%sS"%seq
        sides=[('tib',s) for s in ts if s['lab']!=wrap]+[('eng',s) for s in es if s['lab']!=wrap and s['nul'] is None]
        r=db.execute("select wylie,english from corpus_segments where course=? and seq=?",(course,seq)).fetchone()
        blockinfo[key]=('READ',(course,seq,tl,el,tb,eb,ts,es,r))
        if not r or tp!=r[0] or ep!=r[1]:
            notspine_blocks+=1; notspine_sides+=len(sides); continue
        w,e=r
        tt,et=tok(w,'tib'),tok(e,'eng')
        for side,s in sides:
            text,toks=(w,tt) if side=='tib' else (e,et)
            a,b=s['a'],s['b']
            if a==b: c='EMPTY_SPAN'; empty_ex.append((key,s['lab']))
            elif text[a].isspace() or text[b-1].isspace(): c='WHITESPACE_EDGE'; ws_blocks.add(key)
            else:
                cov=[(x,y) for x,y in toks if x<b and y>a]
                x,y=cov[0][0],cov[-1][1]
                if (x,y)==(a,b): c='plain'
                elif text[x:y].count(text[a:b])==1: c='bracket'
                else: c='SUBSTRING_NOT_UNIQUE'; nu_ex.append((key,s['lab'],text[a:b],text[x:y]))
            cnt[c]+=1; bydepth[c][s['d']]+=1
print("pages",pages,"blocks",blocks,"tags on pages",tags_total)
print("unreadable blocks",unread_blocks,"tags",unread_tags,"| not-spine blocks",notspine_blocks,"sides",notspine_sides)
tot=sum(cnt.values()); ex=cnt['plain']+cnt['bracket']
print("coverage: %d of %d (%.3f%%)"%(ex,tot,100*ex/tot), dict(cnt))
for c in bydepth: print("  ",c,dict(bydepth[c]))
print("NOT_UNIQUE:",nu_ex); print("EMPTY:",empty_ex)
import pickle; pickle.dump((blockinfo,ws_blocks),open("recount.pkl","wb"))
