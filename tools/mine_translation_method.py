#!/usr/bin/env python3
"""Find where Geshe Michael teaches TRANSLATION METHOD, in his own classes.

Backlog #32. He has taught the craft for decades in passing — how to
render a term, when a word shifts register, what to do with Sanskrit,
what he tells students about the work — and almost none of it is
collected. The dictionary records the RESULTS of that method; the
classes record the method itself.

What this produces and what it does not
---------------------------------------
CANDIDATES. Every passage is a machine-located span of auto-generated
captions, and the RECORDING is the authority — captions mishear, and a
sentence pulled out of a class can lose the case it was answering.
Nothing here is a rule. A passage becomes a rule only if Geshe Michael
rules it one through the approval channel (inviolable rule 1: machine
work may MATCH his English from evidence, never COMPOSE it).

Patterns are weighted by how specifically they indicate METHOD rather
than mere subject matter. "we translate this as" is method; "in
Sanskrit" is a topic marker that appears constantly and is kept only
as supporting context around a stronger hit.

Usage:  python3 tools/mine_translation_method.py [--limit N]
Writes: data/teaching/translation_method.json
        docs/research/GMR_TRANSLATION_METHOD.md   (readable digest)
"""
import collections
import glob
import json
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT_JSON = os.path.join(ROOT, "data/teaching/translation_method.json")
OUT_MD = os.path.join(ROOT, "docs/research/GMR_TRANSLATION_METHOD.md")
TS = re.compile(r"(\d+):(\d\d):(\d\d)\.(\d\d\d) -->")
TAG = re.compile(r"<[^>]+>")

# (name, weight, regex). Weight 3 = says something ABOUT rendering.
# Weight 2 = defines a term. Weight 1 = topical, kept as context only.
PATTERNS = [
    ("renders-as", 3, re.compile(
        r"\b(?:we|I|you)\s+(?:usually |normally |always |generally )?"
        r"translate\s+(?:it|this|that|the\s+\w+)?\s*(?:as|to be|with)\b", re.I)),
    ("better-rendering", 3, re.compile(
        r"\b(?:a\s+)?(?:better|closer|more accurate|the correct)\s+"
        r"translation\b", re.I)),
    ("mistranslation", 3, re.compile(r"\bmis-?translat\w*", re.I)),
    ("refuses-rendering", 3, re.compile(
        r"\b(?:don'?t|do not|never|shouldn'?t|should not)\s+translate\b", re.I)),
    ("hard-to-render", 3, re.compile(
        r"\b(?:hard|difficult|tricky|impossible|no way)\s+to\s+translate\b", re.I)),
    ("no-english-word", 3, re.compile(
        r"\bno\s+(?:real\s+|good\s+|single\s+)?(?:word|equivalent)\s+in\s+English\b", re.I)),
    ("literally-means", 2, re.compile(r"\bliterally\s+(?:it\s+|this\s+)?means\b", re.I)),
    ("word-means", 2, re.compile(
        r"\bthe\s+(?:Tibetan\s+|Sanskrit\s+)?word\s+[\w'’-]+\s+means\b", re.I)),
    ("term-is-called", 2, re.compile(
        r"\bin\s+Tibetan\s+(?:it'?s|it is|this is|that'?s)\s+called\b", re.I)),
    ("context-sanskrit", 1, re.compile(r"\bin\s+Sanskrit\b", re.I)),
    ("context-tibetan-word", 1, re.compile(r"\bthe\s+Tibetan\s+(?:word|term)\b", re.I)),
]
STRONG = {n for n, w, _ in PATTERNS if w >= 2}


def raw_cues(path):
    t, buf, last = None, [], ""
    for line in open(path, encoding="utf-8", errors="replace"):
        line = line.strip()
        m = TS.match(line)
        if m:
            if t is not None and buf:
                txt = " ".join(buf).strip()
                if txt and txt != last:
                    yield t, txt
                    last = txt
            t = int(m[1]) * 3600 + int(m[2]) * 60 + int(m[3])
            buf = []
        elif (line and "-->" not in line and not line.isdigit()
              and line != "WEBVTT"
              and not line.startswith(("Kind:", "Language:"))):
            buf.append(TAG.sub("", line))
    if t is not None and buf:
        txt = " ".join(buf).strip()
        if txt and txt != last:
            yield t, txt


def cues(path):
    """(seconds, NEW words only) — YouTube rolling captions un-rolled.

    Auto-captions roll: each cue repeats the tail of the previous one
    and adds a few words. Collapsing only EXACTLY equal neighbours (what
    the term indexer does, which is right for its job) leaves passages
    like "difficult to when you these are difficult to when you these
    are difficult to". For reading a passage back, the overlap has to
    go: keep the longest suffix of what we already have that prefixes
    the new cue, and append only the remainder.
    """
    tail = []
    for t, txt in raw_cues(path):
        w = txt.split()
        if not w:
            continue
        k = 0
        for n in range(min(len(tail), len(w)), 0, -1):
            if tail[-n:] == w[:n]:
                k = n
                break
        new = w[k:]
        if not new:
            continue
        tail = (tail + new)[-40:]
        yield t, " ".join(new)


def titles():
    out = {}
    for p in ("data/teaching/dcc_videos.json", "data/teaching/tkb_videos.json"):
        try:
            d = json.load(open(os.path.join(ROOT, p)))
        except Exception:
            continue
        items = d.values() if isinstance(d, dict) else d
        for v in items:
            if isinstance(v, dict) and v.get("id"):
                out[v["id"]] = v.get("title", "")
    return out


def main():
    limit = None
    if "--limit" in sys.argv:
        limit = int(sys.argv[sys.argv.index("--limit") + 1])
    files = sorted(f for f in glob.glob(
        os.path.join(ROOT, "data/teaching/*_captions/*.en.vtt"))
        if "-orig" not in f)
    if limit:
        files = files[:limit]
    tmap = titles()
    found, bypat = [], collections.Counter()
    seen_text = set()
    seen_bucket = set()

    for path in files:
        vid = os.path.basename(path).split(".")[0]
        chan = "TKB" if "tkb_captions" in path else "DCC"
        cs = list(cues(path))
        if not cs:
            continue
        for i, (sec, txt) in enumerate(cs):
            hitnames = [n for n, w, p in PATTERNS if w >= 2 and p.search(txt)]
            if not hitnames:
                continue
            # a passage is the hit plus its neighbours, so the sentence
            # is not decapitated by a caption break
            lo, hi = max(0, i - 2), min(len(cs), i + 4)
            passage = " ".join(c[1] for c in cs[lo:hi])
            passage = re.sub(r"\s+", " ", passage).strip()
            # one passage per MOMENT: consecutive cues each trip the
            # same pattern, which produced three copies of one sentence
            # at t=2094/2096/2096. Collapse by video + 45s bucket, and
            # keep the text key as a second net for repeats elsewhere.
            bucket = (vid, sec // 45)
            if bucket in seen_bucket:
                continue
            key = re.sub(r"[^a-z ]", "", passage.lower())[:180]
            if key in seen_text:
                continue
            seen_bucket.add(bucket)
            seen_text.add(key)
            start = max(0, cs[lo][0] - 2)
            weight = max(w for n, w, p in PATTERNS if n in hitnames)
            found.append({
                "video": vid, "channel": chan,
                "title": tmap.get(vid, ""),
                "t": start,
                "url": "https://www.youtube.com/watch?v=%s&t=%ds" % (vid, start),
                "patterns": sorted(hitnames),
                "weight": weight,
                "passage": passage[:600],
            })
            for n in hitnames:
                bypat[n] += 1

    found.sort(key=lambda r: (-r["weight"], r["video"], r["t"]))
    doc = {
        "_meta": {
            "what": "machine-located passages where Geshe Michael appears "
                    "to teach translation METHOD",
            "authority": "THE RECORDING. These are auto-generated captions; "
                         "they mishear, and a sentence lifted out of a class "
                         "can lose the question it was answering.",
            "status": "CANDIDATES. Nothing here is a rule. A passage becomes "
                      "one only when Geshe Michael rules it through the "
                      "approval channel.",
            "files_read": len(files),
            "passages": len(found),
            "by_pattern": dict(bypat),
        },
        "passages": found,
    }
    os.makedirs(os.path.dirname(OUT_MD), exist_ok=True)
    with open(OUT_JSON, "w") as f:
        json.dump(doc, f, indent=1)
        f.write("\n")
    print("caption files read : %d" % len(files))
    print("candidate passages : %d" % len(found))
    for n, c in bypat.most_common():
        print("   %-22s %d" % (n, c))
    print("wrote %s" % os.path.relpath(OUT_JSON, ROOT))


KIND_HEADS = [
    ("refuses-rendering", "He rules a rendering OUT",
     "The strongest kind of instruction here: not what a word means, "
     "but what it must not be called."),
    ("renders-as", "He gives a rendering",
     "\"we translate this as…\" — his own English, spoken in class."),
    ("better-rendering", "He prefers one rendering over another", ""),
    ("mistranslation", "He names a mistranslation", ""),
    ("hard-to-render", "He says a thing is hard or impossible to render",
     "Often the most useful passages for a working translator: they "
     "name the problem rather than paper over it."),
    ("no-english-word", "He says English has no word for it", ""),
    ("word-means", "He defines a term", ""),
    ("literally-means", "He gives a literal sense",
     "Usually a step toward a rendering rather than the rendering."),
    ("term-is-called", "He names the Tibetan for something", ""),
]


def digest():
    doc = json.load(open(OUT_JSON))
    ps = doc["passages"]
    meta = doc["_meta"]
    L = []
    L.append("# Where Geshe Michael teaches translation method\n")
    L.append("*Machine-located passages from %d caption files of his own "
             "recorded classes (Diamond Cutter Classics and The Knowledge "
             "Base). Backlog #32.*\n" % meta["files_read"])
    L.append("> **These are candidates, and the recording is the "
             "authority.**\n>\n"
             "> They are auto-generated captions: they mishear. In one "
             "passage below he says a term must not be rendered "
             "\"sutra\" and the caption writes \"citrus\". A sentence "
             "lifted out of a class can also lose the question it was "
             "answering.\n>\n"
             "> **Nothing here is a rule.** A passage becomes one only "
             "when Geshe Michael rules it one through the approval "
             "channel. Machine work may MATCH his English from evidence; "
             "it may never COMPOSE it.\n")
    L.append("**%d passages** across %d files. Every timecode links to "
             "the moment in the recording.\n" % (len(ps), meta["files_read"]))
    by = collections.defaultdict(list)
    for r in ps:
        for n in r["patterns"]:
            by[n].append(r)
    for name, head, note in KIND_HEADS:
        rows = by.get(name, [])
        if not rows:
            continue
        L.append("\n## %s  ·  %d\n" % (head, len(rows)))
        if note:
            L.append("*%s*\n" % note)
        for r in rows[:60]:
            mm, ss = divmod(r["t"], 60)
            hh, mm = divmod(mm, 60)
            stamp = "%d:%02d:%02d" % (hh, mm, ss) if hh else "%d:%02d" % (mm, ss)
            title = r["title"] or r["video"]
            L.append("- [%s](%s) · %s\n  > %s\n"
                     % (stamp, r["url"], title[:70], r["passage"][:420]))
        if len(rows) > 60:
            L.append("\n*(%d more of this kind in "
                     "data/teaching/translation_method.json — this digest "
                     "shows the first 60 so the page stays readable; "
                     "nothing is discarded.)*\n" % (len(rows) - 60))
    open(OUT_MD, "w").write("\n".join(L))
    print("wrote %s" % os.path.relpath(OUT_MD, ROOT))


if __name__ == "__main__":
    if "--digest" in sys.argv:
        digest()
    else:
        main()
        digest()
