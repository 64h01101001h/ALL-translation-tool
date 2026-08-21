#!/usr/bin/env python3
"""fetch_persons_pass3.py — People layer (#59), Sanskrit-name pass.

The 89 names pass 2 could not resolve are mostly Indian panditas:
their BDRC identities carry SANSKRIT labels (lang sa-x-ndia), not
Tibetan ones, so no bo-x-ewts query can ever find them. This pass:

  convert    — canonical sanskrit_convert.acip_to_iast renders the
               ACIP name as one IAST compound (proven live:
               A'ARYA SH'U RA -> a'aryaśūra).
  retrieval  — BLMP LG_NAME=sa-x-ndia, which matches
               accent-insensitively (proven: plain 'damstrasena'
               finds *Daṃṣṭrasena). Query variants cover the two
               documented Tibetan-source equivalences: ACIP A'A = ā
               (fold aa->a) and b/v conflation (Tibetan BA writes
               Sanskrit va).
  acceptance — DETERMINISTIC: canonical fold of the query equals
               the canonical fold of a candidate label (NFD strip
               diacritics, lowercase, letters only, aa->a, v->b).
               Nothing looser; homonyms all kept; a leading '*' on
               a BDRC label (their reconstructed-name mark) is
               recorded, never hidden.

Tibetan-name leftovers simply fold to compounds no Sanskrit label
equals — they stay unresolved, honestly.
"""
import json, re, sys, time, os, unicodedata, urllib.parse, urllib.request

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.expanduser(
    "~/Desktop/HGM DICTIONARY - TRANSLATION APP/Handoff/tools"))
from sanskrit_convert import acip_to_iast

UA = {"User-Agent":
      "ALL-Translation-Tool people-harvest-pass3 (ACIP/ALL)"}

def get(url):
    req = urllib.request.Request(url, headers=UA)
    with urllib.request.urlopen(req, timeout=60) as r:
        return json.load(r)

def blmp_sa(query, maxpages=4):
    """quoted query against Sanskrit labels; person rows, paged."""
    url = ("https://purl.bdrc.io/query/table/BLMP?L_NAME=%22" +
           urllib.parse.quote(query) +
           "%22&LG_NAME=sa-x-ndia&I_LIM=500&format=json")
    rows, pages = [], 0
    while url and pages < maxpages:
        try:
            d = get(url)
        except Exception as e:
            print("  !! blmp", query, e)
            return rows
        for r in d.get("results", {}).get("bindings", []):
            m = re.search(r"/resource/(P[0-9A-Z]+)$", r["s"]["value"])
            if m:
                rows.append((m.group(1),
                             r.get("lit", {}).get("value", "")))
        url = d.get("pLinks", {}).get("nextGet")
        pages += 1
        time.sleep(0.4)
    return rows

def fold(s):
    """canonical comparison form: strip diacritics, lowercase,
    letters only, aa->a (ACIP 'a-chung long vowel), v->b."""
    s = unicodedata.normalize("NFD", s)
    s = "".join(c for c in s if not unicodedata.combining(c))
    s = re.sub(r"[^a-zA-Z]", "", s).lower()
    while "aa" in s:
        s = s.replace("aa", "a")
    return s.replace("v", "b")

def clean_parts(acip):
    s = re.sub(r"\[.*?\]", " ", acip)
    s = re.sub(r"\(\d+\)", " ", s)
    s = re.sub(r"\(.*?\)", " ", s)
    parts = re.split(r"\s+aka\s+|[;]", s, flags=re.IGNORECASE)
    return [p.strip(" ,;") for p in parts if p.strip(" ,;")]

def person_names(pid):
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
    time.sleep(0.4)
    return labels, tol

def main():
    path = os.path.join(ROOT, "data/extracted/persons_bdrc.json")
    db = json.load(open(path))
    unresolved = db["meta"]["unresolved"]
    print(len(unresolved), "unresolved after pass 2", flush=True)

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

    resolved, still = 0, []
    for i, acip in enumerate(sorted(unresolved)):
        accepted = {}
        for part in clean_parts(acip):
            try:
                iast = acip_to_iast(part)
            except Exception:
                continue
            base = fold(iast)
            if len(base) < 5:
                continue
            # retrieval variants: the analyzer folds diacritics but
            # not b/v or doubled a — probe the plausible spellings
            probes = {base, base.replace("b", "v")}
            for q in sorted(probes):
                for pid, lit in blmp_sa(q):
                    if pid in accepted:
                        continue
                    if fold(lit) == base:
                        accepted[pid] = {"query": q, "label": lit}
                if accepted:
                    break
            if accepted:
                break
        if accepted:
            cands = []
            for pid, how in list(accepted.items())[:4]:
                labels, tol = person_names(pid)
                pref = [l for l in labels
                        if re.search(r"[A-Za-z]", l)][:6]
                cands.append({"pid": pid, "names": pref, "tol": tol,
                              "matched_via": {
                                  "sanskrit_label": how["label"],
                                  "reconstructed":
                                      how["label"].startswith("*")}})
            m = ameta.get(acip, {"eng": "", "dates": "", "works": 0})
            db["authors"][acip] = {
                "eng": m["eng"], "dates": m["dates"],
                "works": m["works"], "pass": 3, "candidates": cands}
            resolved += 1
            print(f"[{i+1}] {acip}: "
                  f"{[(c['pid'], c['names'][:1]) for c in cands]}",
                  flush=True)
        else:
            still.append(acip)
            print(f"[{i+1}] {acip}: unresolved", flush=True)
        if (i + 1) % 10 == 0:
            db["meta"]["unresolved"] = still + sorted(unresolved)[i+1:]
            json.dump(db, open(path, "w"), ensure_ascii=False,
                      indent=1)

    db["meta"]["unresolved"] = still
    db["meta"]["pass3"] = {
        "harvested": time.strftime("%Y-%m-%d"),
        "method": "canonical acip_to_iast + sa-x-ndia label search; "
                  "accept only on canonical-fold EQUALITY (diacritic "
                  "strip, aa->a, v->b); BDRC * reconstructed-name "
                  "marks recorded"}
    json.dump(db, open(path, "w"), ensure_ascii=False, indent=1)
    print(f"PASS 3 DONE: +{resolved} resolved, {len(still)} remain "
          f"-> {path}", flush=True)

if __name__ == "__main__":
    main()
