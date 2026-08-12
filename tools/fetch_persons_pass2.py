#!/usr/bin/env python3
"""fetch_persons_pass2.py — People layer (#59), second resolution pass.

Pass 1 (fetch_persons.py) resolved 151/265 catalog authors with exact
quoted-phrase BLMP queries — and, as discovered in this pass, only
ever read PAGE 1 of the results (the server pages at 50 rows and
work titles crowd persons off the first page). This pass works the
114 leftovers with WIDER RETRIEVAL + STRICT VERIFICATION:

  retrieval  — quoted PHRASE WINDOWS of the name (the Lucene index
               tokenizes pa'i as one token, so phrases must respect
               it: the full name, then contiguous prefix/suffix
               windows >= 2 tokens), every query PAGED through the
               result pages; honest variants first (markup stripped,
               "aka" aliases split, leading titles dropped, one
               trailing honorific rje dropped, each labeled).
  acceptance — DETERMINISTIC, against the candidate's FULL label set
               (skos prefLabel + altLabel from the person's own
               JSON): accepted only when EVERY token of the variant
               appears in one label (genitive-tolerant: pa'i==pa).
               Homonyms are ALL kept, never auto-merged.
  review     — candidates missing exactly ONE token that differs
               only by aspiration class (k/kh, c/ch, t/th, p/ph,
               ts/tsh — a real ACIP-source spelling doubt) are
               RECORDED for Adam's review, never asserted.

Output: merges accepted authors into data/extracted/persons_bdrc.json
(tagged "pass": 2 with the variant and label that matched); review
candidates + still-unresolved stay in meta.
"""
import json, re, sys, time, os, urllib.parse, urllib.request

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.expanduser(
    "~/Desktop/HGM DICTIONARY - TRANSLATION APP/Handoff/tools"))
from hgm_tools import acip_to_ewts

UA = {"User-Agent":
      "ALL-Translation-Tool people-harvest-pass2 (ACIP/ALL)"}

def get(url):
    req = urllib.request.Request(url, headers=UA)
    with urllib.request.urlopen(req, timeout=60) as r:
        return json.load(r)

def blmp_paged(query, maxpages=6):
    """quoted phrase query, person rows from ALL result pages."""
    url = ("https://purl.bdrc.io/query/table/BLMP?L_NAME=%22" +
           urllib.parse.quote(query) +
           "%22&LG_NAME=bo-x-ewts&I_LIM=500&format=json")
    pids, pages = [], 0
    while url and pages < maxpages:
        try:
            d = get(url)
        except Exception as e:
            print("  !! blmp", query, e)
            return pids
        for r in d.get("results", {}).get("bindings", []):
            m = re.search(r"/resource/(P[0-9A-Z]+)$", r["s"]["value"])
            if m and m.group(1) not in pids:
                pids.append(m.group(1))
        url = d.get("pLinks", {}).get("nextGet")
        pages += 1
        time.sleep(0.4)
    return pids

def toks(s):
    s = s.lower().replace("/", " ").replace("_", " ")
    s = re.sub(r"[^a-z'+ ]", " ", s)
    return [t for t in s.split() if t]

def gfold(t):
    """genitive-tolerant token key: pa'i -> pa"""
    return t[:-2] if t.endswith("'i") and len(t) > 2 else t

ASP = [("tsh", "ts"), ("kh", "k"), ("ch", "c"), ("th", "t"),
       ("ph", "p")]
def deaspirate(t):
    for a, b in ASP:
        if t.startswith(a):
            return b + t[len(a):]
    return t

HONOR = {"rje"}   # honorific tokens the source may add that BDRC
                  # labels often omit (Lord Drom = 'brom ston rje)

def verify(query, labels):
    """all query tokens in one label (genitive-fold both sides,
    closed honorific set optional)?
    -> ('accept', label) / ('review', label, missing) / (None,)"""
    best = None
    for label in labels:
        lt = set()
        for t in toks(label):
            lt.add(t)
            lt.add(gfold(t))
        missing = [t for t in toks(query)
                   if t not in lt and gfold(t) not in lt]
        hard = [t for t in missing if t not in HONOR]
        if not hard:
            return ("accept", label)
        if len(hard) == 1 and any(
                deaspirate(hard[0]) == deaspirate(x) and
                hard[0] != x for x in lt):
            best = ("review", label, hard)
    return best or (None,)

PRE = re.compile(
    r"^(rje |je |rgyal ba |rgyal dbang |mkhas grub |a lag sha |"
    r"mkhan po |mkhan chen |dge bshes |slob dpon |pan chen |"
    r"khri byang |a kya |gung thang |btsun pa |grub chen |"
    r"sgom chen |ser byes )")

def variants(acip):
    """honest variants: markup stripped, aka aliases split, leading
    titles dropped, one trailing honorific rje dropped."""
    s = acip.strip()
    s = re.sub(r"\[.*?\]", " ", s)          # [?] flags
    s = re.sub(r"^\(\d+\)\s*", "", s)       # (1) markers
    s = re.sub(r"\(.*?\)", " ", s)          # parentheticals
    parts = re.split(r"\s+aka\s+", s, flags=re.IGNORECASE)
    out = []
    def add(label, w):
        w = re.sub(r"\s+", " ", w).strip()
        if w and len(w.split()) >= 2 and \
           all(w != x[1] for x in out):
            out.append((label, w))
    for p in parts:
        p = re.sub(r"\s+", " ", p).strip(" ,;")
        if not p:
            continue
        w = acip_to_ewts(p).strip().lower()
        add("as-written", w)
        stripped = w
        while PRE.match(stripped):
            stripped = PRE.sub("", stripped)
        if stripped != w:
            add("title-stripped", stripped)
    return out

def windows(wylie, cap=7):
    """quoted-phrase retrieval windows: full name, then contiguous
    prefix/suffix windows (>= 2 tokens), longest first."""
    t = wylie.split()
    seen, out = set(), []
    def add(ws):
        q = " ".join(ws)
        if len(ws) >= 2 and q not in seen:
            seen.add(q)
            out.append(q)
    add(t)
    for k in range(len(t) - 1, 1, -1):
        add(t[:k])       # prefix window
        add(t[-k:])      # suffix window
    return out[:cap]

_pcache = {}
def person_labels(pid):
    """(all skos pref+alt labels, ToL xref) — cached per pid."""
    if pid in _pcache:
        return _pcache[pid]
    labels, tol = [], ""
    try:
        pj = get(f"https://purl.bdrc.io/resource/{pid}.json")
        node = pj.get(f"http://purl.bdrc.io/resource/{pid}", {})
        for key in ("http://www.w3.org/2004/02/skos/core#prefLabel",
                    "http://www.w3.org/2004/02/skos/core#altLabel"):
            labels += [v["value"] for v in node.get(key, [])]
        for v in node.get("http://www.w3.org/2002/07/owl#sameAs", []):
            if "treasuryoflives" in v.get("value", ""):
                tol = v["value"]
    except Exception as e:
        print("  !! person", pid, e)
    _pcache[pid] = (labels, tol)
    time.sleep(0.4)
    return _pcache[pid]

def main():
    path = os.path.join(ROOT, "data/extracted/persons_bdrc.json")
    db = json.load(open(path))
    unresolved = db["meta"]["unresolved"]
    print(len(unresolved), "unresolved from pass 1", flush=True)

    works = json.load(open(os.path.join(
        ROOT, "data/extracted/catalog_works.json")))
    if isinstance(works, dict):
        works = list(works.values())
    ameta = {}
    for w in works:
        a = (w.get("author") or "").strip()
        if a:
            ameta.setdefault(a, {"eng": w.get("author_eng", ""),
                                 "dates": w.get("dates", ""),
                                 "works": 0})
            ameta[a]["works"] += 1

    resolved, review, still = 0, {}, []
    for i, acip in enumerate(sorted(unresolved)):
        accepted, reviews = {}, []
        for vlabel, wylie in variants(acip):
            tried_pids = set()
            for q in windows(wylie):
                for pid in blmp_paged(q):
                    if pid in accepted or pid in tried_pids:
                        continue
                    if len(tried_pids) >= 60:   # generic-window guard
                        break
                    tried_pids.add(pid)
                    labels, tol = person_labels(pid)
                    v = verify(wylie, labels)
                    if v[0] == "accept":
                        accepted[pid] = {"variant": vlabel,
                                         "query": wylie,
                                         "label": v[1], "tol": tol}
                    elif v[0] == "review":
                        reviews.append({"pid": pid, "variant": vlabel,
                                        "query": wylie, "label": v[1],
                                        "aspiration_miss": v[2]})
                if accepted:
                    break
            if accepted:
                break
        if accepted:
            cands = []
            for pid, how in list(accepted.items())[:4]:
                labels, tol = person_labels(pid)
                pref = [l for l in labels if re.search(r"[a-z]", l)]
                cands.append({"pid": pid, "names": pref[:6],
                              "tol": tol,
                              "matched_via": {"variant": how["variant"],
                                              "label": how["label"]}})
            m = ameta.get(acip, {"eng": "", "dates": "", "works": 0})
            db["authors"][acip] = {
                "eng": m["eng"], "dates": m["dates"],
                "works": m["works"], "pass": 2, "candidates": cands}
            resolved += 1
            print(f"[{i+1}] {acip}: {[c['pid'] for c in cands]}"
                  f"{' +ToL' if any(c['tol'] for c in cands) else ''}",
                  flush=True)
        else:
            if reviews:
                seen, rv = set(), []
                for r in reviews:
                    if r["pid"] not in seen:
                        seen.add(r["pid"])
                        rv.append(r)
                review[acip] = rv[:4]
                print(f"[{i+1}] {acip}: REVIEW "
                      f"{[r['pid'] for r in review[acip]]}", flush=True)
            else:
                print(f"[{i+1}] {acip}: unresolved", flush=True)
            still.append(acip)
        # write-through every 10 names: resume-safe
        if (i + 1) % 10 == 0:
            db["meta"]["unresolved"] = still + sorted(unresolved)[i+1:]
            json.dump(db, open(path, "w"), ensure_ascii=False, indent=1)

    db["meta"]["unresolved"] = still
    db["meta"]["pass2"] = {
        "harvested": time.strftime("%Y-%m-%d"),
        "method": "paged quoted phrase-window retrieval + "
                  "deterministic all-tokens verification against the "
                  "candidate's full skos label set "
                  "(genitive-tolerant); aspiration near-misses "
                  "recorded for review, never asserted",
        "review_candidates": review}
    json.dump(db, open(path, "w"), ensure_ascii=False, indent=1)
    print(f"PASS 2 DONE: +{resolved} resolved, {len(review)} with "
          f"review-tier candidates, {len(still)} unresolved "
          f"-> {path}", flush=True)

if __name__ == "__main__":
    main()
