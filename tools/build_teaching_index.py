#!/usr/bin/env python3
"""build_teaching_index.py — teaching-moment index (#63).

Parses harvested VTT captions (data/teaching/dcc_captions/) +
video titles (dcc_videos.json) and finds term mentions.
Captions are a FINDING AID: matches produce timecoded links to the
recordings; caption text is machine-derived and labeled so.

Usage:
  build_teaching_index.py --sample "emptiness,bodhichitta"  # probe
  build_teaching_index.py --full  # -> data/teaching/teaching_moments.json
"""
import json, re, sys, glob, os

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
CAPS = os.path.join(ROOT, "data/teaching/dcc_captions")
VIDS = json.load(open(os.path.join(ROOT, "data/teaching/dcc_videos.json")))

TS = re.compile(r"(\d+):(\d\d):(\d\d)\.(\d\d\d) -->")
TAG = re.compile(r"<[^>]+>")

def cues(path):
    """Yield (seconds, text) — rolling auto-caption dupes collapsed."""
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
        elif line and "-->" not in line and not line.isdigit() \
                and line != "WEBVTT" and not line.startswith(("Kind:", "Language:")):
            buf.append(TAG.sub("", line))
    if t is not None and buf:
        txt = " ".join(buf).strip()
        if txt and txt != last:
            yield t, txt

def video_id(path):
    return os.path.basename(path).split(".")[0]

def index(terms, per_term_cap=None):
    # one caption file per video: prefer manual .en.vtt over -orig/auto
    byvid = {}
    for f in sorted(glob.glob(CAPS + "/*.vtt")):
        vid = video_id(f)
        cur = byvid.get(vid)
        if cur is None or (".en.vtt" in f and ".en.vtt" not in cur):
            byvid[vid] = f
    termset = set(terms)
    maxw = max(len(t.split()) for t in terms)
    out = {t: [] for t in terms}
    wordre = re.compile(r"[a-z\-']+")
    LANG = re.compile(r"\b(ENG|VIE|CHN|SPA|GER|RUS|UKR|POR|FRA|CZE)\b",
                      re.I)
    for vid, f in sorted(byvid.items()):
        title = VIDS.get(vid, {}).get("title", "")
        m = LANG.search(title)
        lang = (m.group(1).upper() if m else "?")
        seen_min = {}
        for sec, txt in cues(f):
            ws = wordre.findall(txt.lower())
            for i in range(len(ws)):
                for n in range(1, maxw + 1):
                    if i + n > len(ws): break
                    cand = " ".join(ws[i:i + n])
                    if cand in termset and                             sec - seen_min.get(cand, -100) >= 60:
                        seen_min[cand] = sec
                        out[cand].append({
                            "video": vid, "title": title, "t": sec,
                            "lang": lang,
                            "url": f"https://www.youtube.com/watch?v={vid}&t={sec}s",
                            "snippet": txt[:160],
                            "source": "DCC YouTube captions "
                                      "(machine-derived)"})
    # PRUNE (lesson of the first full run): HGM glosses include
    # ordinary English (right, think, because) — genuine equivalents,
    # useless as teaching probes. Single-word terms at conversational
    # frequency are dropped; every term is capped, sampled evenly.
    for t in out:
        out[t].sort(key=lambda h: (h.get("lang") != "ENG",))
    CONV = 2500
    CAP = per_term_cap or 60
    for t in list(out):
        if " " not in t and len(out[t]) > CONV:
            out[t] = []
        elif len(out[t]) > CAP:
            v = out[t]
            step = len(v) / CAP
            out[t] = [v[int(i * step)] for i in range(CAP)]
    return out, len(byvid)

if __name__ == "__main__":
    if "--sample" in sys.argv:
        terms = sys.argv[sys.argv.index("--sample") + 1].split(",")
        idx, nf = index(terms)
        print(f"[{nf} caption files scanned]")
        for t in terms:
            hits = idx[t]
            print(f"\n== {t}: {len(hits)} mention(s)")
            for h in hits[:3]:
                m, s = divmod(h["t"], 60)
                print(f"  {h['title'][:60]}")
                print(f"    @{m}:{s:02d}  {h['url']}")
                print(f"    \"{h['snippet'][:110]}\"")
    elif "--card" in sys.argv:
        # slim app-facing file: top 5 per term, ENG first
        d = json.load(open(os.path.join(ROOT,
                           "data/teaching/teaching_moments.json")))
        slim = {}
        for t, v in d["terms"].items():
            if not v: continue
            seen, keep = set(), []
            for h in v:
                if h["video"] in seen: continue
                seen.add(h["video"])
                keep.append({k: h.get(k, "?") for k in
                             ("title", "url", "t", "lang", "snippet")})
                if len(keep) == 5: break
            slim[t] = keep
        out = os.path.join(ROOT,
                           "data/teaching/teaching_moments_card.json")
        json.dump({"meta": d["meta"], "terms": slim},
                  open(out, "w"), ensure_ascii=False)
        print(len(slim), "terms ->", out,
              round(os.path.getsize(out) / 1e6, 1), "MB")
    elif "--full" in sys.argv:
        terms = [l.strip() for l in
                 open(os.path.join(ROOT, "data/teaching/terms.txt"))
                 if l.strip()]
        idx, nf = index(terms)
        out = os.path.join(ROOT, "data/teaching/teaching_moments.json")
        json.dump({"meta": {"files": nf,
                            "note": "machine-located candidates; the "
                                    "recording is the authority"},
                   "terms": idx}, open(out, "w"), ensure_ascii=False)
        total = sum(len(v) for v in idx.values())
        print(f"{nf} files, {len(terms)} terms, {total} moments -> {out}")
