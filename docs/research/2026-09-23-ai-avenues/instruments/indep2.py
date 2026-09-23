import io, os, re, sqlite3, sys, html, collections
sys.path.insert(0, '/Users/adamderickandrade/ALL-translation-tool/tools')
import build_alignment_layer as B
con = sqlite3.connect(B.SPINE)
courses = [c for c in sorted({r[0] for r in con.execute("SELECT DISTINCT course FROM corpus_segments")})
    if re.fullmatch(r'C\d\d', c) and 1 <= int(c[1:]) <= 18 and c != 'C13']
rows = {}
for c in courses:
    for q, w, e in con.execute("SELECT seq,wylie,english FROM corpus_segments WHERE course=?", (c,)):
        rows[(c, q)] = (w, e)
landed = {}
for course, cfg in B.COURSES.items():
    for pg, seqs in cfg["pages"].items():
        for q in seqs:
            if (course, q) in rows: landed[(course, q)] = (pg, cfg["dir"])
order = {c: i for i, c in enumerate(courses)}
g = collections.defaultdict(list)
for k, v in rows.items(): g[v].append(k)
pairs = []
for v, mem in g.items():
    if len(mem) == 1: continue
    mem = sorted(mem, key=lambda k: (order[k[0]], k[1]))
    al = [k for k in mem if k in landed]
    rep = sorted(al, key=lambda k: (order[k[0]], k[1]))[0] if al else mem[0]
    for t in [k for k in mem if k != rep and k not in landed]:
        if rep in landed: pairs.append((t, rep))
print("pairs with landed source:", len(pairs), " distinct sources:", len({s for _t,s in pairs}))

def spans_in(fragment):
    out, stack, buf, i = [], [], [], 0
    for m in re.finditer(r'<span\b([^>]*)>|</span>', fragment):
        buf.append(html.unescape(fragment[i:m.start()])); i = m.end()
        pos = len("".join(buf))
        if m.group(0).startswith('</'):
            if not stack: return None
            lab, st = stack.pop(); out.append((lab, st, pos))
        else:
            lm = re.search(r'data-l="([^"]*)"', m.group(1))
            stack.append((lm.group(1) if lm else None, pos))
    buf.append(html.unescape(fragment[i:]))
    if stack: return None
    return "".join(buf), out

cache = {}
def source_spans(src):
    if src in cache: return cache[src]
    pg, pdir = landed[src]
    doc = io.open(os.path.join(pdir, pg + ".html"), encoding="utf-8").read()
    blk = None
    for p in re.split(r'(?=<h2>Segments? )', doc):
        m = re.match(r'<h2>Segments? (\d+)([^<]*)</h2>', p)
        if not m: continue
        lo = int(m.group(1))
        hi = lo
        m2 = re.search(r'(\d+)\s*$', html.unescape(m.group(2)).replace('–','-').strip().strip('-').strip())
        tail = re.findall(r'(\d+)', html.unescape(m.group(2)))
        if tail: hi = int(tail[-1])
        if lo <= src[1] <= hi: blk = p; break
    if blk is None: cache[src] = None; return None
    j = blk.find('<div class="tib">'); k = blk.find('<div class="eng">'); n = blk.find('<div class="note">')
    if j < 0 or k < 0: cache[src] = None; return None
    tibf = re.sub(r'^<div class="tib"><div class="lab">.*?</div>','',blk[j:k],flags=re.S)
    engf = re.sub(r'^<div class="eng"><div class="lab">.*?</div>','',(blk[k:n] if n>k else blk[k:]),flags=re.S)
    engf = re.sub(r'<span class="nul">.*?</span>','',engf,flags=re.S)
    pref = 's%d' % src[1]
    res = {}
    for side, frag in (("tib", tibf), ("eng", engf)):
        r = spans_in(frag)
        if r is None: cache[src] = None; return None
        text, sp = r
        keep = []
        for lab, lo2, hi2 in sp:
            if lab is None or not lab.startswith(pref): continue
            if len(lab) > len(pref) and lab[len(pref)].isdigit(): continue
            piece = text[lo2:hi2]
            if piece.strip(): keep.append((lab, piece))
        res[side] = keep
    cache[src] = res
    return res

for mode in ("per-pair", "per-source"):
    items = pairs if mode == "per-pair" else [(s, s) for s in sorted({s for _t,s in pairs}, key=lambda k:(order[k[0]],k[1]))]
    tt=te=bt=be=0; exc=collections.OrderedDict(); miss=[]
    for tgt, src in items:
        r = source_spans(src)
        if r is None: miss.append(src); continue
        wyl, eng = rows[tgt]
        for lab, piece in r["tib"]:
            tt += 1
            if piece not in wyl:
                bt += 1; exc[(src,'tib',lab)] = piece
        for lab, piece in r["eng"]:
            te += 1
            if piece not in eng:
                be += 1; exc[(src,'eng',lab)] = piece
    print("%s: TIB %d/%d verbatim, ENG %d/%d verbatim, unreadable sources %d (%s)"
          % (mode, tt-bt, tt, te-be, te, len(set(miss)), sorted(set(miss))[:12]))
    if mode == "per-pair":
        print("  DISTINCT exception spans (%d):" % len(exc))
        for (src,side,lab),piece in exc.items():
            print("   %s:%d %s %s %r" % (src[0],src[1],side,lab,piece))
