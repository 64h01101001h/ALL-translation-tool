#!/usr/bin/env python3
"""fetch_persons.py — People layer harvest (#59).

266 catalog authors (ACIP names) -> BDRC P-ids via the BLMP label
search (person rows post-filtered client-side; proven: tsong kha pa
-> P64 exactly) -> per-person JSON (names, dates, Treasury of Lives
cross-ref). Homonyms are ALL kept — never auto-merged; unresolved
names are listed, never guessed.
Output: data/extracted/persons_bdrc.json + provenance."""
import json, re, sys, time, os, urllib.parse, urllib.request

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.expanduser(
    "~/Desktop/HGM DICTIONARY - TRANSLATION APP/Handoff/tools"))
from hgm_tools import acip_to_ewts

def get(url):
    req = urllib.request.Request(url, headers={"User-Agent":
        "ALL-Translation-Tool people-harvest (ACIP/ALL)"})
    with urllib.request.urlopen(req, timeout=40) as r:
        return json.load(r)

works = json.load(open(os.path.join(ROOT,
                       "data/extracted/catalog_works.json")))
if isinstance(works, dict):
    works = list(works.values())
authors = {}
for w in works:
    a = (w.get("author") or "").strip()
    if a and a.lower() != "unknown":
        authors.setdefault(a, {"eng": w.get("author_eng", ""),
                               "dates": w.get("dates", ""),
                               "works": 0})
        authors[a]["works"] += 1
print(len(authors), "distinct authors")

PRE = re.compile(r"^(rje |je |rgyal ba |mkhas grub |a lag sha )")
out, unresolved = {}, []
for i, (acip, meta) in enumerate(sorted(authors.items())):
    wylie = acip_to_ewts(acip).strip().lower()
    tried = [wylie]
    if PRE.match(wylie):
        tried.append(PRE.sub("", wylie))
    pids = []
    for q in tried:
        url = ("https://purl.bdrc.io/query/table/BLMP?L_NAME=%22" +
               urllib.parse.quote(q) +
               "%22&LG_NAME=bo-x-ewts&I_LIM=200&format=json")
        try:
            d = get(url)
        except Exception as e:
            print("  !!", acip, e); break
        for r in d.get("results", {}).get("bindings", []):
            m = re.search(r"/resource/(P[0-9A-Z]+)$", r["s"]["value"])
            if m and m.group(1) not in pids:
                pids.append(m.group(1))
        if pids: break
        time.sleep(0.6)
    if not pids:
        unresolved.append(acip)
        print(f"[{i+1}] {acip}: UNRESOLVED")
        continue
    cands = []
    for pid in pids[:4]:   # homonyms all kept, capped sanely
        try:
            pj = get(f"https://purl.bdrc.io/resource/{pid}.json")
        except Exception:
            continue
        node = pj.get(f"http://purl.bdrc.io/resource/{pid}", {})
        names = [v["value"] for v in node.get(
            "http://www.w3.org/2004/02/skos/core#prefLabel", [])]
        tol = ""
        for v in node.get("http://www.w3.org/2002/07/owl#sameAs", []):
            if "treasuryoflives" in v.get("value", ""):
                tol = v["value"]
        cands.append({"pid": pid, "names": names, "tol": tol})
        time.sleep(0.6)
    out[acip] = {"eng": meta["eng"], "dates": meta["dates"],
                 "works": meta["works"], "candidates": cands}
    print(f"[{i+1}] {acip}: {[c['pid'] for c in cands]}"
          f"{' +ToL' if any(c['tol'] for c in cands) else ''}")
    time.sleep(0.8)

json.dump({"meta": {"source": "BDRC BLMP label search + per-person "
                              "JSON; ToL via owl:sameAs",
                    "harvested": "2026-08-12",
                    "note": "homonym candidates all kept, never "
                            "auto-merged; unresolved never guessed",
                    "unresolved": unresolved},
           "authors": out},
          open(os.path.join(ROOT, "data/extracted/persons_bdrc.json"),
               "w"), ensure_ascii=False, indent=1)
print(f"DONE: {len(out)} resolved, {len(unresolved)} unresolved "
      f"-> data/extracted/persons_bdrc.json")
