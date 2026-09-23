"""Independent round trip: my own spec builder (no span_id_compiler import),
the generator's own main(), byte comparison with the landed block."""
import sys, os, re, io, json, pickle, collections, copy
ROOT="/Users/adamderickandrade/ALL-translation-tool"
sys.path.insert(0, os.path.join(ROOT,"tools"))
import gen_alignment_page as G
blockinfo, _ = pickle.load(open("recount.pkl","rb"))
res = pickle.load(open("res.pkl","rb"))
W=re.compile(r"[A-Za-z0-9ༀ-࿿]")
def cut(t,i): return 0<i<len(t) and bool(W.match(t[i-1])) and bool(W.match(t[i]))
PAIR=re.compile(r'<div class="pair">\n<div class="tib"><div class="lab">[^<]*</div>\n(.*?)\n</div>\n<div class="eng"><div class="lab">[^<]*</div>\n(.*?)\n</div>\n</div>',re.S)
def gen(course, seg):
    oi,oo=sys.stdin,sys.stdout
    sys.stdin=io.StringIO(json.dumps({"course":course,"segments":[seg]},ensure_ascii=False)); sys.stdout=buf=io.StringIO()
    try: G.main()
    except SystemExit as e: return None, str(e.code)
    except Exception as e: return None, "CRASH %r"%e
    finally: sys.stdin,sys.stdout=oi,oo
    m=PAIR.search(buf.getvalue()); return (m.group(1),m.group(2)), None
ok=set(); why=collections.Counter()
bypage=collections.defaultdict(list)
for key in res["blocks"]: bypage[key.rsplit(":",1)[0]].append(key)
for k,(st,info) in blockinfo.items():
    ckey=bypage["%s/%s"%(k[0],k[1])][k[2]-1]
    if st!='READ': why['unread']+=1; continue
    course,seq,tl,el,tb,eb,ts,es,r=info
    if not r: why['nospine']+=1; continue
    w,e=r
    wrap="s%dS"%seq
    ts=[s for s in ts if s['lab']!=wrap]; es=[s for s in es if s['lab']!=wrap]
    engl={s['lab']:s for s in es if s['nul'] is None}; nul={s['lab']:s for s in es if s['nul'] is not None}
    flat=sorted([s for s in ts if s['d']!=7],key=lambda s:(s['a'],-(s['b']-s['a'])))
    if any(y['a']<x['b'] for x,y in zip(flat,flat[1:])): why['flat-nested']+=1; continue
    order=[]
    bad=False
    for p in flat:
        order.append(p)
        ms=sorted([m for m in ts if m['d']==7 and p['d']==5 and p['a']<=m['a'] and m['b']<=p['b']],key=lambda s:s['a'])
        order+=ms
    if len(order)!=len(ts): why['orphan-member']+=1; continue
    spans=[]
    for s in order:
        lab=s['lab']; sid=lab[len("s%d"%seq):]
        o={"id":sid,"d":s['d'],"tib":w[s['a']:s['b']]}
        if lab in engl: o["eng"]=e[engl[lab]['a']:engl[lab]['b']]
        elif lab in nul:
            o["eng"]=None
            if nul[lab]['nul']!=o["tib"]+"→∅": o["nul"]=nul[lab]['nul']
        else: bad=True
        if s['cls']: o["cls"]=s['cls']
        if s['d']!=7 and (cut(w,s['a']) or cut(w,s['b']) or (lab in engl and (cut(e,engl[lab]['a']) or cut(e,engl[lab]['b'])))): o["subword"]=True
        spans.append(o)
    if bad: why['no-eng-no-marker']+=1; continue
    fe=sorted([s for s in order if s['d']!=7 and s['lab'] in engl],key=lambda s:engl[s['lab']]['a'])
    seg={"seq":seq,"title":"t","spans":spans}
    if fe: seg["eng_order"]=[s['lab'][len("s%d"%seq):] for s in fe]
    out,err=gen(course,seg)
    if err: why['gen-refused']+=1; continue
    if out==(tb,eb): ok.add(ckey)
    else: why['bytes-differ']+=1
cok={k for k,b in res["blocks"].items() if b["class"]=="OK"}
print("independent round trip:",len(ok),"blocks;  compiler:",len(cok))
print("reasons:",dict(why))
print("in mine not compiler:",sorted(ok-cok)[:20])
print("in compiler not mine:",sorted(cok-ok)[:20])
