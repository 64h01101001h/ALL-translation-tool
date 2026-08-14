#!/usr/bin/env python3
"""build_sungbum_scan_links.py — Sungbum scan routing for ALL major
authors via BDRC's own WorkVersionPerPerson query (found in
buda-base/lds-queries): person → every per-text MW node with its
Tibetan-unicode title. No outline walking needed.

Method per author (pid with ≥5 ACIP texts in Élie's catalog):
  1. page through WorkVersionPerPerson (unicode titles, mw nodes);
  2. principal instance = the most frequent plain MW-numeric prefix
     (the author's canonical gsung 'bum scan set — hash-suffixed
     0XL works and cross-edition dupes excluded by the prefix);
  3. our side: Élie-catalog wylie titles → unicode via the
     canonical engine; normalize both sides (strip shads/spaces);
  4. exact match within the principal instance, unique-only;
     ambiguous/unmatched listed, never guessed.

Merged with the MW22109 outline pilot (existing entries kept).

  output  data/extracted/sungbum_scan_links.json
"""
import collections, json, os, re, sys, time, urllib.request

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
HANDOFF = os.path.expanduser(
    "~/Desktop/HGM DICTIONARY - TRANSLATION APP/Handoff/tools")
sys.path.insert(0, HANDOFF)
from ewts_unicode import wylie_to_unicode   # canonical oracle

LINKS = os.path.join(ROOT, "data/extracted/acip_person_links.json")
PILOT = os.path.join(
    ROOT, "data/extracted/sungbum_mw22109_concordance.json")
OUT = os.path.join(ROOT, "data/extracted/sungbum_scan_links.json")
Q = ("https://purl.bdrc.io/query/table/WorkVersionPerPerson"
     "?R_RES=bdr:%s&format=json&pageSize=500&pageNumber=%d")


def norm_uni(t):
    t = re.sub(r"[།༎་]+$", "", t.strip())
    t = t.replace(" ", "")
    return t


def fetch_person(pid):
    rows, page = [], 1
    while True:
        try:
            with urllib.request.urlopen(Q % (pid, page),
                                        timeout=40) as r:
                d = json.load(r)
        except Exception as e:
            print("  fetch failed", pid, page, e)
            break
        for b in d.get("results", {}).get("bindings", []):
            mw = b.get("mw", {}).get("value", "").split("/")[-1]
            ti = b.get("mwlu", {}).get("value", "")
            if mw and ti:
                rows.append((mw, ti))
        if d.get("lastPage") or page >= int(
                d.get("numberOfPages", 1)):
            break
        page += 1
        time.sleep(1.0)
    return rows


def main():
    links = json.load(open(LINKS))["links"]
    by_pid = collections.defaultdict(dict)
    for s, v in links.items():
        if s.startswith("S") and v["title_wylie"]:
            by_pid[v["pid"]][s] = v["title_wylie"]
    pids = {p: t for p, t in by_pid.items() if len(t) >= 5}
    print(f"{len(pids)} authors with ≥5 texts")

    merged = {}
    if os.path.exists(PILOT):
        pj = json.load(open(PILOT))["s_to_node"]
        for s, v in pj.items():
            merged[s] = {"node": v["node"], "label": v["label"],
                         "pid": "P64",
                         "tier": "outline-" + v["tier"]}
    stats = []
    for pid, stexts in sorted(pids.items(),
                              key=lambda x: -len(x[1])):
        rows = fetch_person(pid)
        time.sleep(1.0)
        if not rows:
            stats.append((pid, len(stexts), 0, 0))
            continue
        pref = collections.Counter(
            m.split("_")[0] for m, _ in rows
            if re.fullmatch(r"MW\d+_[0-9A-F]+", m))
        if not pref:
            stats.append((pid, len(stexts), 0, 0))
            continue
        principal = pref.most_common(1)[0][0]
        cand = collections.defaultdict(set)
        for m, ti in rows:
            if m.startswith(principal + "_"):
                cand[norm_uni(ti)].add(m)
        matched = 0
        for s, wy in stexts.items():
            if s in merged:
                continue
            try:
                r = wylie_to_unicode(wy)
                uni = r[0] if isinstance(r, tuple) else r
            except Exception:
                continue
            key = norm_uni(uni)
            nodes = cand.get(key, set())
            if len(nodes) == 1:
                merged[s] = {"node": next(iter(nodes)),
                             "label": key, "pid": pid,
                             "tier": "wvpp-exact"}
                matched += 1
        stats.append((pid, len(stexts), len(rows), matched))
        print(f"  {pid}: {len(stexts)} S-texts · {len(rows)} BDRC "
              f"rows · principal {principal} · +{matched} matched")
    json.dump({
        "meta": {
            "method": "WorkVersionPerPerson per author; principal "
                      "gsung 'bum instance by MW-prefix majority; "
                      "unicode exact title match (canonical "
                      "engine on our side), unique-only; merged "
                      "with the MW22109 outline pilot; nothing "
                      "guessed",
            "authors": len(pids),
            "links": len(merged),
        },
        "s_to_node": merged,
    }, open(OUT, "w"), ensure_ascii=False, indent=1)
    print(f"TOTAL {len(merged)} Sungbum scan links -> {OUT}")


if __name__ == "__main__":
    main()
