#!/usr/bin/env python3
"""build_alignment_layer.py — the ACI alignment evidence layer.

Converts the delivered full-depth alignment pages (span markup, the
source of truth for pass three) into a versioned evidence sidecar the
app can load: word-level (depth-5) Tibetan<->English pairs, each
citing the corpus segments that attest it.

GUARDRAILS (inviolable rule 1): every English string in this layer is
Geshe Michael's own corpus English, machine-MATCHED never composed.
The layer is evidence, tier TENTATIVE; it never writes hgm_gloss and
the card renders it amber, toggleable, unmistakably labeled.

Battery (refuses to write on any failure — rule 3):
  * every pair's Tibetan must appear letter-exact in the wylie of
    every segment it cites, straight from the spine;
  * every English fragment must appear letter-exact in that segment's
    English;
  * the pair count must not fall below the floor (a thin layer is a
    broken parse, not a small corpus).

Stage-then-swap. Usage: build_alignment_layer.py [--pages DIR]
"""
import hashlib
import html
import io
import json
import os
import re
import sqlite3
import sys
from datetime import date

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
PAGES = os.path.expanduser("~/Desktop/ACI2-full-depth-alignment")
SPINE = os.path.join(ROOT, "build", "hgm_spine_v27_2.db")
OUTDIR = os.path.join(ROOT, "data", "alignment")
OUT = os.path.join(OUTDIR, "alignment_c02_v1.json")
FLOOR = 120   # distinct word-level pairs; pass three carries ~200+

PAGE_SEGS = {
    "p01": [23], "p02": [24, 25], "p03": [26, 27, 28, 29, 30],
    "p04": [31, 32, 33, 34], "p05": [35, 36, 37], "p06": [38, 39, 40],
    "p07": [41, 42, 43, 44, 45], "p08": [46], "p09": [47, 48, 49],
    "p10": [50, 51, 52], "p11": [53, 54, 55, 56], "p12": [57, 58, 59],
    "p13": [60, 61], "p14": [62, 63, 64], "p15": [65, 66, 67, 68],
}

SPAN = re.compile(
    r'<span class="u" data-d="(\d)" data-l="([A-Za-z0-9_ ]+?)">')


def letters(s):
    return re.sub(r"[^a-z']", "", s.lower())


def strip_tags(s):
    return html.unescape(re.sub(r"<[^>]+>", " ", s))


def spans_of(doc, side):
    """Yield (depth, id, text) for every span inside the given side
    ('tib' or 'eng') of every .pair block, text tag-stripped."""
    out = []
    for chunk in doc.split('<div class="pair">')[1:]:
        for stop in ('<div class="note"', '<h2'):
            i = chunk.find(stop)
            if i >= 0:
                chunk = chunk[:i]
        j = chunk.find('<div class="eng">')
        if j < 0:
            continue
        block = chunk[:j] if side == "tib" else chunk[j:]
        # the machine's own notation is not evidence: drop nul-marker
        # annotations and bracketed editorial text before walking
        block = re.sub(r'<span class="nul">.*?</span>', "", block,
                       flags=re.S)
        block = re.sub(r"<i[^>]*>\[NO ENGLISH.*?</i>", "", block,
                       flags=re.S)
        # walk spans with a manual stack so nesting resolves
        pos, stack, hits = 0, [], []
        for tk in re.finditer(r'<span\b[^>]*>|</span>', block):
            if tk.group(0).startswith("</"):
                if stack:
                    d, l, start = stack.pop()
                    hits.append((d, l, block[start:tk.start()]))
            else:
                m2 = SPAN.match(tk.group(0))
                if m2:
                    stack.append((int(m2.group(1)),
                                  m2.group(2).split(" ")[0],
                                  tk.end()))
                else:
                    stack.append((0, "", tk.end()))
        for d, l, raw in hits:
            if d and l:
                out.append((d, l, " ".join(strip_tags(raw).split())))
    return out


def main():
    pages_dir = PAGES
    if "--pages" in sys.argv:
        pages_dir = sys.argv[sys.argv.index("--pages") + 1]
    con = sqlite3.connect(SPINE)
    segs = {r[0]: (r[1], r[2]) for r in con.execute(
        "SELECT seq, wylie, english FROM corpus_segments "
        "WHERE course='C02'")}
    con.close()

    pairs = {}   # tib_norm -> {eng_display -> set(segs)}
    sha = hashlib.sha256()
    for pg, seglist in sorted(PAGE_SEGS.items()):
        p = os.path.join(pages_dir, pg + ".html")
        doc = io.open(p, encoding="utf-8").read()
        sha.update(doc.encode())
        tib, eng = {}, {}
        for d, l, txt in spans_of(doc, "tib"):
            if d == 5 and txt:
                tib.setdefault(l, txt)
        for d, l, txt in spans_of(doc, "eng"):
            if d == 5 and txt:
                eng.setdefault(l, []).append(txt)
        for l, t in tib.items():
            if l not in eng:
                continue   # no-exponent word: not evidence, skip
            seg = None
            m = re.match(r"s(\d+)", l)
            if m and int(m.group(1)) in segs:
                seg = int(m.group(1))
            if seg is None:
                continue
            e = " … ".join(eng[l])   # fragmented exponent joined
            tn = " ".join(t.lower().split())
            # BATTERY line 1+2: letter-exact against the spine
            if letters(t) not in letters(segs[seg][0]):
                sys.exit("REFUSED: tib %r not in seg %d wylie" % (t, seg))
            for frag in eng[l]:
                if letters(frag) and \
                        letters(frag) not in letters(segs[seg][1]):
                    sys.exit("REFUSED: eng %r not in seg %d english"
                             % (frag, seg))
            pairs.setdefault(tn, {}).setdefault(e, set()).add(seg)

    n_pairs = sum(len(v) for v in pairs.values())
    if n_pairs < FLOOR:
        sys.exit("REFUSED: only %d pairs (< floor %d) — the parse is "
                 "broken, not the corpus small" % (n_pairs, FLOOR))

    doc = {
        "meta": {
            "layer": "alignment-evidence",
            "tier": "TENTATIVE (machine-matched, unreviewed)",
            "generator": "Claude (AI), full-depth alignment pass three",
            "date": str(date.today()),
            "source_pages": "ACI2-full-depth-alignment (sha256 %s)"
                            % sha.hexdigest()[:16],
            "source_corpus": "hgm_spine_v27_2 corpus_segments C02",
            "course": "C02",
            "rule": "English is HGM's corpus text verbatim, machine-"
                    "MATCHED never composed; never enters hgm_gloss",
        },
        "pairs": {
            tn: [{"eng": e, "segs": sorted(ss), "n": len(ss)}
                 for e, ss in sorted(evs.items())]
            for tn, evs in sorted(pairs.items())
        },
    }
    os.makedirs(OUTDIR, exist_ok=True)
    tmp = OUT + ".staging"
    with io.open(tmp, "w", encoding="utf-8") as f:
        json.dump(doc, f, ensure_ascii=False, indent=1)
    os.replace(tmp, OUT)
    print("alignment layer: %d headwords, %d pairs -> %s"
          % (len(pairs), n_pairs, os.path.relpath(OUT, ROOT)))


if __name__ == "__main__":
    main()
