import re, glob, os, sys, html, sqlite3, collections, json
ROOT = "/Users/adamderickandrade/ALL-translation-tool"
sys.path.insert(0, os.path.join(ROOT, "tools"))
from alignment_page_dirs import page_dirs

DIRCOURSE = {"pages": "C02"}
db = sqlite3.connect(os.path.join(ROOT, "build/hgm_spine_v27_2.db"))

TAG = re.compile(r"<[^>]+>")
SPANOPEN = re.compile(r'<span class="([^"]*)" data-d="(\d)" data-l="([^"]*)"\s*>')


def walk(block):
    """return plain text, list of spans (d,label,cls,start,end,nul)"""
    pos = 0
    plain = []
    stack = []
    spans = []
    i = 0
    nuldepth = 0
    for m in re.finditer(r"<[^>]+>|[^<]+", block):
        t = m.group(0)
        if t.startswith("<"):
            if t.startswith("</span"):
                top = stack.pop()
                if top[0] == "nul":
                    nuldepth -= 1
                elif top[0] == "u":
                    _, d, lab, cls, st, nul = top
                    spans.append((d, lab, cls, st, pos, nul))
            elif t.startswith("<span"):
                mm = SPANOPEN.match(t)
                if t.startswith('<span class="nul"'):
                    stack.append(("nul",))
                    nuldepth += 1
                    # record nul text into parent
                elif mm:
                    stack.append(["u", int(mm.group(2)), mm.group(3), mm.group(1), pos, None])
                else:
                    stack.append(("other", t))
            else:
                # other tag (br, i, b...)
                if not t.startswith("</") and not t.endswith("/>"):
                    pass
        else:
            txt = html.unescape(t)
            if nuldepth:
                # attach to nearest u
                for fr in reversed(stack):
                    if fr[0] == "u":
                        fr[5] = (fr[5] or "") + txt
                        break
                continue
            plain.append(txt)
            pos += len(txt)
    return "".join(plain), spans


stats = collections.Counter()
mism = []
for d in page_dirs():
    course = DIRCOURSE.get(d) or ("C" + d.split("_c")[1].upper()) if d != "pages" else "C02"
    for f in sorted(glob.glob(os.path.join(ROOT, "data/alignment", d, "*.html"))):
        s = open(f, encoding="utf-8").read()
        for m in re.finditer(r'<h2>Segment (\d+)', s):
            stats["h2"] += 1
        chunks = s.split('<div class="pair">')[1:]
        for ch in chunks:
            stats["pairs"] += 1
            ti = ch.find('<div class="tib">')
            ei = ch.find('<div class="eng">')
            if ti < 0 or ei < 0:
                stats["pair_no_tib_or_eng"] += 1
                continue
            tb = ch[ti:ei]
            tb = tb[tb.find("</div>") + 6:]
            tb = tb.rsplit("</div>", 1)[0]
            eb = ch[ei:]
            eb = eb[eb.find("</div>") + 6:]
            # eng block ends at "\n</div>\n</div>"
            k = eb.find("\n</div>\n</div>")
            eb = eb[:k] if k >= 0 else eb
            try:
                tp, ts = walk(tb.strip("\n"))
                ep, es = walk(eb.strip("\n"))
            except Exception as ex:
                stats["walk_error"] += 1
                mism.append((f, "walkerr", repr(ex)[:60], eb[:300]))
                continue
            labs = [x[1] for x in ts if x[0] == 1] + [x[1] for x in es if x[0] == 1]
            seqm = re.match(r"s(\d+)", labs[0]) if labs else None
            if not seqm:
                stats["no_seq"] += 1
                continue
            seq = int(seqm.group(1))
            row = db.execute("select wylie, english from corpus_segments where course=? and seq=?", (course, seq)).fetchone()
            if not row:
                stats["not_in_spine"] += 1; continue
            w, e = row
            okt = tp == w
            oke = ep == e
            stats["tib_ok" if okt else "tib_bad"] += 1
            stats["eng_ok" if oke else "eng_bad"] += 1
            if not (okt and oke):
                mism.append((f, seq, okt, oke))
            stats["tib_spans"] += len(ts)
            stats["eng_spans"] += len(es)
print(stats)
for x in mism[:30]:
    print(x)
print(len(mism))

import difflib
def show(fpath, seq):
    pass
