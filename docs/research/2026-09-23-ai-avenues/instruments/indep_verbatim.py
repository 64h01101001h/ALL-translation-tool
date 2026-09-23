"""Independent re-derivation: does every span of a duplicate's landed SOURCE
land verbatim in the TARGET's own spine row?  Own parser, own SQL."""
import io, os, re, sqlite3, sys, html, json, collections
sys.path.insert(0, '/Users/adamderickandrade/ALL-translation-tool/tools')
import build_alignment_layer as B

con = sqlite3.connect(B.SPINE)
courses = [c for c in sorted({r[0] for r in con.execute(
    "SELECT DISTINCT course FROM corpus_segments")})
    if re.fullmatch(r'C\d\d', c) and 1 <= int(c[1:]) <= 18 and c != 'C13']
rows = {}
for c in courses:
    for q, w, e in con.execute(
            "SELECT seq,wylie,english FROM corpus_segments WHERE course=?", (c,)):
        rows[(c, q)] = (w, e)
landed = {}
for course, cfg in B.COURSES.items():
    for pg, seqs in cfg["pages"].items():
        for q in seqs:
            if (course, q) in rows:
                landed[(course, q)] = (pg, cfg["dir"])

order = {c: i for i, c in enumerate(courses)}
g = collections.defaultdict(list)
for k, v in rows.items():
    g[v].append(k)
need_sources = set()      # source segments that a duplicate target would use
for v, mem in g.items():
    if len(mem) == 1:
        continue
    mem = sorted(mem, key=lambda k: (order[k[0]], k[1]))
    al = [k for k in mem if k in landed]
    rep = sorted(al, key=lambda k: (order[k[0]], k[1]))[0] if al else mem[0]
    tg = [k for k in mem if k != rep and k not in landed]
    if tg and rep in landed:
        need_sources.add(rep)
print("distinct landed source segments feeding a duplicate target:", len(need_sources))

# --- my own page reader -------------------------------------------------
SEG = re.compile(r'<h2>Segments? (\d+)[^<]*</h2>')
def spans_in(fragment):
    """[(label, text)] for every data-l span, own stack, no normalisation."""
    out, stack, buf, i = [], [], [], 0
    for m in re.finditer(r'<span\b([^>]*)>|</span>', fragment):
        buf.append(html.unescape(fragment[i:m.start()]))
        i = m.end()
        pos = len("".join(buf))
        if m.group(0).startswith('</'):
            if not stack:
                return None
            lab, st = stack.pop()
            out.append((lab, st, pos))
        else:
            lm = re.search(r'data-l="([^"]*)"', m.group(1))
            cm = re.search(r'class="([^"]*)"', m.group(1))
            stack.append(((lm.group(1) if lm else None,
                           cm.group(1) if cm else ''), pos))
    buf.append(html.unescape(fragment[i:]))
    if stack:
        return None
    text = "".join(buf)
    return text, out

bad_t = bad_e = tot_t = tot_e = 0
exceptions = []
unparsed = []
for src in sorted(need_sources, key=lambda k: (order[k[0]], k[1])):
    pg, pdir = landed[src]
    doc = io.open(os.path.join(pdir, pg + ".html"), encoding="utf-8").read()
    # cut the segment's own pair div, by heading
    parts = re.split(r'(?=<h2>Segments? )', doc)
    blk = None
    for p in parts:
        m = SEG.match(p)
        if m and int(m.group(1)) == src[1]:
            blk = p
            break
    if blk is None:
        unparsed.append((src, "no block"))
        continue
    j = blk.find('<div class="tib">')
    k = blk.find('<div class="eng">')
    n = blk.find('<div class="note">')
    if j < 0 or k < 0:
        unparsed.append((src, "no columns"))
        continue
    tibf = blk[j:k]
    engf = blk[k:n] if n > k else blk[k:]
    tibf = re.sub(r'^<div class="tib"><div class="lab">.*?</div>', '', tibf, flags=re.S)
    engf = re.sub(r'^<div class="eng"><div class="lab">.*?</div>', '', engf, flags=re.S)
    engf = re.sub(r'<span class="nul">.*?</span>', '', engf, flags=re.S)
    wyl, eng = rows[src]
    for side, frag, hay in (("tib", tibf, wyl), ("eng", engf, eng)):
        r = spans_in(frag)
        if r is None:
            unparsed.append((src, side + " unbalanced"))
            continue
        text, sp = r
        for (lab, cls), lo, hi in sp:
            if lab is None:
                continue
            piece = text[lo:hi]
            if not piece.strip():
                continue
            if side == "tib":
                tot_t += 1
            else:
                tot_e += 1
            if piece not in hay:
                if side == "tib":
                    bad_t += 1
                else:
                    bad_e += 1
                exceptions.append((src, side, lab, piece))
print("TIB spans: %d/%d verbatim in the row" % (tot_t - bad_t, tot_t))
print("ENG spans: %d/%d verbatim in the row" % (tot_e - bad_e, tot_e))
print("unparsed:", unparsed)
print("EXCEPTIONS (%d):" % len(exceptions))
for src, side, lab, piece in exceptions:
    print("  %s:%d %s %s %r" % (src[0], src[1], side, lab, piece))
