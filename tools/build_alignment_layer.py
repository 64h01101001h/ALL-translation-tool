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
SPINE = os.path.join(ROOT, "build", "hgm_spine_v27_2.db")
OUTDIR = os.path.join(ROOT, "data", "alignment")
OUT = os.path.join(OUTDIR, "alignment_evidence_v1.json")
OUT_FULL = os.path.join(OUTDIR, "alignment_full_v1.json")
FLOOR = 150   # distinct word-level pairs across all courses

# course-aware (Adam's standing policy: every new reading scans at
# full depth and flows into the evidence layer the same way)
COURSES = {
    "C02": {
        "dir": os.path.join(OUTDIR, "pages"),
        "pages": {
            "p01": [23], "p02": [24, 25], "p03": [26, 27, 28, 29, 30],
            "p04": [31, 32, 33, 34], "p05": [35, 36, 37],
            "p06": [38, 39, 40], "p07": [41, 42, 43, 44, 45],
            "p08": [46], "p09": [47, 48, 49], "p10": [50, 51, 52],
            "p11": [53, 54, 55, 56], "p12": [57, 58, 59],
            "p13": [60, 61], "p14": [62, 63, 64],
            "p15": [65, 66, 67, 68],
        },
    },
    "C01": {
        "dir": os.path.join(OUTDIR, "pages_c01"),
        "pages": {
            "r1": [23, 24, 25, 26], "r2": [27, 28, 29],
            "r3": [30, 31], "r4": [32, 33, 34, 35, 36],
            "r5": [37, 38],
            "c1p01": [40, 41, 42],
            "c1p02": [44, 45],
            "c1p03": [46, 47],
            "c1p04": [48, 49, 50],
            "c1p05": [51, 52, 53],
            "c1p06": [54, 55, 56],
            "c1p07": [57, 58, 59],
            "c1p08": [60, 61, 62],
            "c1p09": [63, 64, 65],
            "c1p10": [66, 67, 68],
            "c1p11": [69, 70, 71],
            "c1p12": [72, 73, 74],
            "c1p13": [75, 76, 77],
            "c1p14": [78, 79, 80],
            "c1p15": [81, 82, 83],
            "c1p16": [84, 85, 86],
            "c1p17": [87, 88, 89],
            "c1p18": [90, 91, 92],
            "c1p19": [93, 94, 95],
            "c1p20": [96, 97, 98],
            "c1p21": [99, 100, 101],
            "c1p22": [102, 103, 104],
            "c1p23": [105, 106, 107],
            "c1p24": [108, 109, 110],
            "c1p25": [111, 112, 113],
            "c1p26": [114, 115, 116],
            "c1p27": [117, 118, 119],
            "c1p28": [120, 121, 122],
            "c1p29": [123, 124, 125],
            "c1p30": [126, 127, 128],
            "c1p31": [129, 130, 131],
            "c1p32": [132, 133, 134],
            "c1p33": [135, 136, 137],
            "c1p34": [138, 139, 140],
            "c1p35": [141, 142],
            "c1p36": [144, 145],
            "c1p37": [146, 147, 148],
            "c1p38": [149, 150, 151],
            "c1p39": [152, 153, 154],
            "c1p40": [155, 156, 157],
            "c1p41": [158, 159, 160],
            "c1p42": [161, 162, 163],
            "c1p43": [164, 165, 166],
            "c1p44": [167, 168, 169],
            "c1p45": [170, 171, 172],
            "c1p46": [173, 174, 175],
            "c1p47": [176, 177, 178],
            "c1p48": [179, 180, 181],
            "c1p49": [182, 183, 184],
            "c1p50": [185, 186, 187],
            "c1p51": [188, 189, 190],
            "c1p52": [191, 192, 193], "c1p53": [194, 195, 196], "c1p54": [197, 198, 199], "c1p55": [200, 201, 202], "c1p56": [203, 204, 205], "c1p57": [206, 207, 208], "c1p58": [209, 210, 211], "c1p59": [212, 213, 214], "c1p60": [215, 216, 217], "c1p61": [218, 219, 220], "c1p62": [221, 222, 223], "c1p63": [224, 225, 226], "c1p64": [227, 228, 229], "c1p65": [230, 231, 232], "c1p66": [233, 234, 235], "c1p67": [236, 237, 238], "c1p68": [239, 240, 241], "c1p69": [242, 243, 244], "c1p70": [245, 246, 247], "c1p71": [248, 249, 250], "c1p72": [251, 252, 253], "c1p73": [254, 255, 256], "c1p74": [257, 258, 259], "c1p75": [260, 261, 262], "c1p76": [263, 264, 265], "c1p77": [266, 267, 268], "c1p78": [269, 270, 271], "c1p79": [272, 273, 274], "c1p80": [275, 276, 277], "c1p81": [278, 279, 280], "c1p82": [281, 282, 283], "c1p83": [284, 285, 286], "c1p84": [287, 288, 289], "c1p85": [290, 291, 292], "c1p86": [293, 294, 295], "c1p87": [296, 297, 298], "c1p88": [299, 300, 301], "c1p89": [302, 303, 304], "c1p90": [305, 306, 307], "c1p91": [308, 309, 310], "c1p92": [311, 312, 313], "c1p93": [314, 315, 316], "c1p94": [317, 318, 319], "c1p95": [320, 321, 322], "c1p96": [323, 324, 325], "c1p97": [326, 327, 328], "c1p98": [329, 330, 331], "c1p99": [332, 333, 334], "c1p100": [335, 336, 337], "c1p101": [338, 339, 340], "c1p102": [341, 342, 343], "c1p103": [344, 345, 346], "c1p104": [347, 348, 349], "c1p105": [350, 351, 352], "c1p106": [353, 354, 355], "c1p107": [356, 357, 358], "c1p108": [359, 360, 361], "c1p109": [362, 363, 364], "c1p110": [365, 366, 367], "c1p111": [368, 369, 370], "c1p112": [371, 372, 373], "c1p113": [374, 375, 376], "c1p114": [377, 378, 379], "c1p115": [380, 381, 382], "c1p116": [383, 384, 385], "c1p117": [386, 387, 388], "c1p118": [389, 390, 391], "c1p119": [392, 393, 394], "c1p120": [395, 396, 397], "c1p121": [398, 399, 400], "c1p122": [401, 402, 403], "c1p123": [404, 405, 406], "c1p124": [407, 408, 409], "c1p125": [410, 411, 412], "c1p126": [413, 414, 415], "c1p127": [416, 417, 418], "c1p128": [419, 420, 421], "c1p129": [422, 423, 424], "c1p130": [425, 426, 427], "c1p131": [428, 429, 430], "c1p132": [431, 432, 433], "c1p133": [434, 435, 436], "c1p134": [437, 438, 439], "c1p135": [440, 441, 442], "c1p136": [443, 444, 445], "c1p137": [446, 447, 448], "c1p138": [449, 450, 451], "c1p139": [452, 453, 454], "c1p140": [455, 456, 457], "c1p141": [458, 459, 460], "c1p142": [461, 462, 463], "c1p143": [464, 465, 466], "c1p144": [467, 468, 469], "c1p145": [470, 471, 472], "c1p146": [473, 474, 475], "c1p147": [476, 477, 478], "c1p148": [479, 480, 481], "c1p149": [482, 483, 484], "c1p150": [485, 486, 487], "c1p151": [488, 489, 490],
        },
    },
}

SPAN = re.compile(
    r'<span class="u" data-d="(\d)" data-l="([A-Za-z0-9_ ]+?)">')
NOTE = re.compile(r'<div class="note">(.*?)</div>', re.S)
TREE = re.compile(r'<div class="tree">(.*?)</div>', re.S)


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
                    toks = m2.group(2).split(" ")
                    stack.append((int(m2.group(1)),
                                  toks[0] +
                                  ("|case" if "case" in toks else ""),
                                  tk.end()))
                else:
                    stack.append((0, "", tk.end()))
        for d, l, raw in hits:
            if d and l:
                out.append((d, l, " ".join(strip_tags(raw).split())))
    return out


def main():
    con = sqlite3.connect(SPINE)
    pairs = {}   # tib_norm -> {eng_display -> set("COURSE:seq")}
    full = {"links": [], "notes": {}, "trees": {}}
    sha = hashlib.sha256()
    for course, cfg in sorted(COURSES.items()):
        segs = {r[0]: (r[1], r[2]) for r in con.execute(
            "SELECT seq, wylie, english FROM corpus_segments "
            "WHERE course=?", (course,))}
        for pg, _seglist in sorted(cfg["pages"].items()):
            path = os.path.join(cfg["dir"], pg + ".html")
            doc = io.open(path, encoding="utf-8").read()
            sha.update(doc.encode())
            tib, eng = {}, {}
            tibAll, engAll = {}, {}
            for d, l, txt in spans_of(doc, "tib"):
                lid, case = l.split("|")[0], l.endswith("|case")
                tibAll.setdefault((d, lid), {"t": txt, "case": case})
                if d == 5 and txt:
                    tib.setdefault(lid, txt)
            for d, l, txt in spans_of(doc, "eng"):
                lid = l.split("|")[0]
                engAll.setdefault((d, lid), []).append(txt)
                if d == 5 and txt:
                    eng.setdefault(lid, []).append(txt)
            # THE BANK: every span at every depth, both-sided or not.
            for (d, lid), rec in sorted(tibAll.items()):
                m0 = re.match(r"s(\d+)", lid)
                full["links"].append({
                    "course": course, "page": pg,
                    "seg": int(m0.group(1)) if m0 else None,
                    "id": lid, "d": d, "tib": rec["t"],
                    "eng": " … ".join(engAll.get((d, lid), [])) or None,
                    "case": rec["case"],
                })
            key = course + "/" + pg
            full["notes"][key] = [
                " ".join(strip_tags(n).split())
                for n in NOTE.findall(doc)]
            tr = TREE.findall(doc)
            if tr:
                full["trees"][key] = [
                    " ".join(strip_tags(t).split()) for t in tr]
            for l, t in tib.items():
                if l not in eng:
                    continue   # no-exponent word: not evidence
                m = re.match(r"s(\d+)", l)
                if not (m and int(m.group(1)) in segs):
                    continue
                seg = int(m.group(1))
                e = " … ".join(eng[l])
                tn = " ".join(t.lower().split())
                # BATTERY: letter-exact against the spine, per course
                if letters(t) not in letters(segs[seg][0]):
                    sys.exit("REFUSED: tib %r not in %s:%d wylie"
                             % (t, course, seg))
                for frag in eng[l]:
                    if letters(frag) and \
                            letters(frag) not in letters(segs[seg][1]):
                        sys.exit("REFUSED: eng %r not in %s:%d english"
                                 % (frag, course, seg))
                pairs.setdefault(tn, {}).setdefault(e, set()).add(
                    "%s:%d" % (course, seg))
    con.close()

    n_pairs = sum(len(v) for v in pairs.values())
    if n_pairs < FLOOR:
        sys.exit("REFUSED: only %d pairs (< floor %d) — the parse is "
                 "broken, not the corpus small" % (n_pairs, FLOOR))

    def refkey(r):
        c, n = r.split(":")
        return (c, int(n))

    doc = {
        "meta": {
            "layer": "alignment-evidence",
            "tier": "TENTATIVE (machine-matched, unreviewed)",
            "generator": "Claude (AI), full-depth alignment",
            "date": str(date.today()),
            "source_pages": "banked pages, %d course(s) (sha256 %s)"
                            % (len(COURSES), sha.hexdigest()[:16]),
            "source_corpus": "hgm_spine_v27_2 corpus_segments "
                             + "+".join(sorted(COURSES)),
            "rule": "English is HGM's corpus text verbatim, machine-"
                    "MATCHED never composed; never enters hgm_gloss",
        },
        "pairs": {
            tn: [{"eng": e, "refs": sorted(ss, key=refkey),
                  "n": len(ss)}
                 for e, ss in sorted(evs.items())]
            for tn, evs in sorted(pairs.items())
        },
    }
    os.makedirs(OUTDIR, exist_ok=True)
    tmpf = OUT_FULL + ".staging"
    with io.open(tmpf, "w", encoding="utf-8") as f:
        json.dump({"meta": {"what": "the COMPLETE seven-layer bank, "
                            "all courses: every span at every depth "
                            "incl. one-sided (no-exponent) records, "
                            "Wilson case flags, AI notes, tree blocks",
                            "consumer": "the Overlay grammar view; "
                            "the review workflow"},
                   "links": full["links"], "notes": full["notes"],
                   "trees": full["trees"]},
                  f, ensure_ascii=False, indent=None,
                  separators=(",", ":"))
    os.replace(tmpf, OUT_FULL)
    print("full bank: %d links (%d with no English exponent), "
          "%d note blocks, %d tree blocks"
          % (len(full["links"]),
             sum(1 for L in full["links"] if not L["eng"]),
             sum(len(v) for v in full["notes"].values()),
             sum(len(v) for v in full["trees"].values())))
    tmp = OUT + ".staging"
    with io.open(tmp, "w", encoding="utf-8") as f:
        json.dump(doc, f, ensure_ascii=False, indent=1)
    os.replace(tmp, OUT)
    print("alignment layer: %d headwords, %d pairs -> %s"
          % (len(pairs), n_pairs, os.path.relpath(OUT, ROOT)))


if __name__ == "__main__":
    main()
